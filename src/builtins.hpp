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

#define SH_ENUM_BUILTINS  \
    BUILTIN(EXIT, "exit") \
    BUILTIN(ECHO, "echo") \
    BUILTIN(TYPE, "type") \
    BUILTIN(PWD , "pwd" ) \
    BUILTIN(CD  , "cd"  )

namespace sh
{
    //
    // Define Enums
    //

    enum BuiltinKind
    {
#define BUILTIN(a_, b_) BUILTIN_##a_,
SH_ENUM_BUILTINS
#undef BUILTIN
    };

    //
    // Builtin Structure
    //

    class Builtin
    {
    public:
        using exec_type = void(Builtin::*)(UserInput const&) const;

        Builtin();
        Builtin(BuiltinKind kind, ut::cstrparam name, exec_type exec);

        inline bool valid() const { return m_exec != nullptr; }
        void exec(UserInput const& u) const;

        static bool find(ut::strparam name, Builtin& builtin);

    private:
        BuiltinKind     m_kind;
        ut::cstrview    m_name;
        exec_type       m_exec;

        //
        // Forward Decls
        //

#define BUILTIN(a_, b_) void exec##a_(UserInput const&) const;
        SH_ENUM_BUILTINS
#undef BUILTIN
    };
}