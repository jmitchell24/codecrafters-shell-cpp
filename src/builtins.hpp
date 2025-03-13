//
// Created by james on 11/03/25.
//

#pragma once

#include "scanner.hpp"
#include <ut/string/view.hpp>
#include <array>

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
        using fn_type = void(*)(Builtin const&, Scanner&);

        BuiltinKind kind;
        fn_type     fn;

        ut::cstrview    name;
        ut::cstrview    desc;

        static bool find(ut::strparam s, Builtin& b);
    };
}