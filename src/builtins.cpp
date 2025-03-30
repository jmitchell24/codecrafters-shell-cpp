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

bool Builtin::exec(Command & c) const
{

    c.rdout.load();
    c.rderr.load();

    (this->*m_exec)(c);

    c.rdout.unload();
    c.rderr.unload();
    return true;
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

void Builtin::execECHO(Command const& c) const
{
    auto&& tokens = c.args;
    for (size_t i = 1; i < tokens.size(); ++i)
    {
        auto&& it = tokens[i];
        printf("%.*s ", it.size(), it.data());
    }
    printf("\n");
}

void Builtin::execEXIT(Command const& c) const
{
     if (c.isUnary())
     {
         auto arg_text = c.arg1();
         int code = 0;
         if (from_chars(arg_text.begin(), arg_text.end(), code).ec == errc{})
             exit(code);
     }

     exit(EXIT_SUCCESS);
}

void Builtin::execPWD(Command const& c) const
{
    static array<char, 1000> BUFFER;

    if (getcwd(BUFFER.data(), BUFFER.size()) == nullptr)
        BUFFER[0] = '\0';
    printf("%s\n", BUFFER.data());
}

void Builtin::execCD(Command const& c) const
{
    static char const* ENV_HOME = []
    {
        if (char const* value = getenv("HOME"))
            return value;
        return "";
    }();

    if (c.isUnary())
    {
        string arg;

        for (auto&& it: c.arg1())
        {
            if (it == '~')
                arg += ENV_HOME;
            else
                arg += it;
        }

        if (chdir(arg.c_str()) == -1)
        {
            printf("%s: No such file or directory\n", arg.c_str());
        }
    }


}

void Builtin::execTEST(Command const& c) const
{
    c.dbgPrint();

}

void Builtin::execERR(Command const& c) const
{
    auto&& tokens = c.args;
    for (size_t i = 1; i < tokens.size(); ++i)
    {
        auto&& it = tokens[i];
        fprintf(stderr, "%.*s ", it.size(), it.data());
    }
    fprintf(stderr, "\n");
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

void Builtin::execTYPE(Command const& c) const
{
    if (c.arity() > 0)
    {
        auto&& arg = c.arg1();

        //
        // check for builtin
        //

        if (Builtin b; find(arg, b))
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


}