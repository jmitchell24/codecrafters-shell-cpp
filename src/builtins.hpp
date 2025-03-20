//
// Created by james on 11/03/25.
//

#pragma once

//
// sh
//
#include "user_input.hpp"

//
// ut
//
#include <ut/string/view.hpp>

//
// std
//
#include <array>
#include <string>
#include <vector>
#include <filesystem>

#define SH_ENUM_BUILTINS \
    BUILTIN(EXIT, "exit", "exit is a shell builtin") \
    BUILTIN(ECHO, "echo", "echo is a shell builtin") \
    BUILTIN(TYPE, "type", "type is a shell builtin")

namespace sh
{
    //
    // Define Enums
    //

    enum BuiltinKind
    {
#define BUILTIN(a_, b_, c_) BUILTIN_##a_,
SH_ENUM_BUILTINS
#undef BUILTIN
    };

    //
    // Builtin Structure
    //

    struct Builtin
    {
        using fn_type = void(*)(Builtin const&, UserInput const&);

        BuiltinKind kind;
        fn_type     fn;

        ut::cstrview    name;
        ut::cstrview    desc;

        static bool find(ut::strparam name, Builtin& builtin);
    };

    struct Program
    {
        using dirs_type = std::vector<ut::strview>;
        using args_type = std::vector<std::string>;

        std::string file;

        bool exec(args_type const& args);

        static bool findInDirectory(ut::cstrparam dir_str, ut::strparam prog_str, Program& p);
        static bool findInPath(ut::strparam prog_str, Program& p);

        static bool existsInPath(ut::strparam prog_str);
        static bool existsInDirectory(ut::cstrparam dir_str, ut::strparam prog_str);

        static ut::cstrparam getPathVar();
        static dirs_type getPathDirs();
    };
}