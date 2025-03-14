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

#define BUILTIN(a_, b_, c_) void fn##a_(Builtin const& b, Scanner& s);
SH_ENUM_BUILTINS
#undef BUILTIN

static array<Builtin, 3> const BUILTINS =
{
#define BUILTIN(a_, b_, c_)  Builtin{ BUILTIN_##a_, &fn##a_, b_, c_##_sv },
SH_ENUM_BUILTINS
#undef BUILTIN
};

bool Builtin::find(strparam s, Builtin& b)
{
    for (auto&& it: BUILTINS)
    {
        if (s == it.name)
        {
            b = it;
            return true;
        }
    }
    return false;
}

void fnECHO(Builtin const& b, Scanner& s)
{
    auto arg = s.takeAll().trim();
    cout << arg << "\n";
}

void fnEXIT(Builtin const& b, Scanner& s)
{
     auto arg = s.takeAll().trim();
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

void fnTYPE(Builtin const& b, Scanner& s)
{
    auto arg = s.takeAll().trim();

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




