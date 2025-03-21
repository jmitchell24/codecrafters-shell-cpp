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
// Builtin Implementation
//

Builtin::Builtin()
    : m_kind{}, m_name{}, m_exec{nullptr}
{}

Builtin::Builtin(BuiltinKind kind, cstrparam name, exec_type exec)
    : m_kind{kind}, m_name{name}, m_exec{exec}
{}

void Builtin::exec(UserInput const& u) const
{
    (this->*m_exec)(u);
}


bool Builtin::find(strparam name, Builtin& builtin)
{
    static array const BUILTINS =
    {
#define BUILTIN(a_, b_)  Builtin{ BUILTIN_##a_, b_, &Builtin::exec##a_ },
SH_ENUM_BUILTINS
#undef BUILTIN
    };

    for (auto&& it: BUILTINS)
    {
        if (name == it.m_name)
        {
            builtin = it;
            return true;
        }
    }
    return false;
}

void Builtin::execECHO(UserInput const& u) const
{
    auto arg = u.argsText();
    printf("%.*s\n", arg.size(), arg.data());
}

void Builtin::execEXIT(UserInput const& u) const
{
     auto arg = u.argsText();
     int code = 0;

     if (from_chars(arg.begin(), arg.end(), code).ec != errc{})
         exit(EXIT_SUCCESS);
     exit(code);
}

void Builtin::execPWD(UserInput const& u) const
{
    static array<char, 1000> BUFFER;

    if (getcwd(BUFFER.data(), BUFFER.size()) == nullptr)
        BUFFER[0] = '\0';
    printf("%s\n", BUFFER.data());
}

void Builtin::execCD(UserInput const& u) const
{
    auto arg = u.argsText().str();
    if (chdir(arg.c_str()) == -1)
    {
        printf("%s: No such file or directory\n", arg.c_str());
    }
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

void Builtin::execTYPE(UserInput const& u) const
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