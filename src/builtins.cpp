//
// Created by james on 11/03/25.
//

//
// sh
//
#include "builtins.hpp"
using namespace sh;

//
// ut
//
#include "ut/check.hpp"
using namespace ut;

//
// std
//
#include <iostream>
#include <cstdio>
#include <dirent.h>
using namespace std;

//
// Forward Decls
//

#define BUILTIN(a_, b_, c_) void fn##a_(Builtin const&, UserInput const&);
SH_ENUM_BUILTINS
#undef BUILTIN

static array<Builtin, 3> const BUILTINS =
{
#define BUILTIN(a_, b_, c_)  Builtin{ BUILTIN_##a_, &fn##a_, b_, c_##_sv },
SH_ENUM_BUILTINS
#undef BUILTIN
};

//
// Builtin Implementation
//

bool Builtin::find(strparam name, Builtin& builtin)
{
    for (auto&& it: BUILTINS)
    {
        if (name == it.name)
        {
            builtin = it;
            return true;
        }
    }
    return false;
}

void fnECHO(Builtin const& b, UserInput const& u)
{
    auto arg = u.argsText();
    cout << arg << "\n";
}

void fnEXIT(Builtin const& b, UserInput const& u)
{
     auto arg = u.argsText();
     int code = 0;

     if (from_chars(arg.begin(), arg.end(), code).ec != errc{})
         cout << "warning: invalid exit code '" << arg << "\'\n";
     exit(code);
}


//
// 'type' builtin implementation
//

bool fileExists(cstrparam dir_path, strparam filename)
{
#ifndef _DIRENT_HAVE_D_TYPE
    return false; // don't bother trying if we can't check for regular files.
#endif

    if (DIR* dir = opendir(dir_path); dir != nullptr)
    {
        dirent* ent = nullptr;

        while ((ent = readdir(dir)) != nullptr)
        {
            if (ent->d_type == DT_REG)
                if (filename == cstrparam(ent->d_name))
                    return true;
        }

        closedir(dir);
    }

    return false;
}

void fnTYPE(Builtin const& b, UserInput const& u)
{
    auto arg = u.argsText();

    if (arg.empty())
        return;

    //
    // check for builtin
    //

    if (Builtin b; Builtin::find(arg, b))
    {
        cout << arg << " is a shell builtin\n";
        return;
    }

    //
    // search PATH
    //

    auto path_var = string(getenv("PATH"));
    auto paths = trimsplit::container(path_var, [](auto&& it)
    {
       return it == ':';
    });

    for (auto&& it : paths)
    {
        auto s_dir = it.str();
        if (fileExists(s_dir, arg))
        {
            cout << arg << " is " << s_dir << "/" << arg << "\n";
            return;
        }
    }

    cout << arg << ": not found\n";
}




//
// Program Implementation
//

bool Program::exec(Program::args_type const& args)
{
    if (file.empty())
        return false;

    vector<char const*> argbuf;
    argbuf.push_back(file.c_str());
    for (auto&& it: args)
        argbuf.push_back(it.c_str());
    argbuf.push_back(nullptr);

    return execvp(argbuf[0], const_cast<char *const *>(argbuf.data())) != -1;
}


bool Program::findInDirectory(cstrparam dir_str, strparam prog_str, Program& p)
{
#ifndef _DIRENT_HAVE_D_TYPE
    return false; // don't bother trying if we can't check for regular files.
#endif

    if (DIR* dir = opendir(dir_str); dir != nullptr)
    {
        dirent* ent = nullptr;

        while ((ent = readdir(dir)) != nullptr)
        {
            if (ent->d_type == DT_REG)
            {
                if (prog_str == cstrparam(ent->d_name))
                {
                    p.file.append(dir_str.begin(), dir_str.end());
                    p.file += filesystem::path::preferred_separator;
                    p.file.append(prog_str.begin(), prog_str.end());
                    return true;
                }
            }
        }

        closedir(dir);
    }

    return false;
}

bool Program::findInPath(strparam prog_str, Program& p)
{
    for (auto&& it: getPathDirs())
        if (findInDirectory(it.str(), prog_str, p))
            return true;
    return false;
}

bool Program::existsInPath(strparam prog_str)
{
    for (auto&& it: getPathDirs())
        if (existsInDirectory(it.str(), prog_str))
            return true;
    return false;
}

bool Program::existsInDirectory(cstrparam dir_str, strparam prog_str)
{
#ifndef _DIRENT_HAVE_D_TYPE
    return false; // don't bother trying if we can't check for regular files.
#endif

    if (DIR* dir = opendir(dir_str); dir != nullptr)
    {
        dirent* ent = nullptr;

        while ((ent = readdir(dir)) != nullptr)
        {
            if (ent->d_type == DT_REG)
                if (prog_str == cstrparam(ent->d_name))
                    return true;
        }

        closedir(dir);
    }

    return false;
}

cstrparam Program::getPathVar()
{
    static auto path_var = cstrview(getenv("PATH"));
    return path_var;
}


Program::dirs_type Program::getPathDirs()
{
    static dirs_type path_dirs = trimsplit::container(getPathVar(), [](auto&& it){ return it == ':'; });
    return path_dirs;
}

