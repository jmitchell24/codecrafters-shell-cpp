//
// Created by james on 11/03/25.
//

#pragma once

//
// sh
//
#include "command.hpp"

//
// ut
//
#include <ut/string/view.hpp>

#define SH_EXPAND_ENUM_BUILTINS  \
    BUILTIN(EXIT, "exit") \
    BUILTIN(ECHO, "echo") \
    BUILTIN(TYPE, "type") \
    BUILTIN(PWD , "pwd" ) \
    BUILTIN(CD  , "cd"  ) \
    BUILTIN(TEST, "test") \
    BUILTIN(ERR , "err")


namespace sh
{
    //
    // Define Enums
    //

    enum BuiltinKind
    {
#define BUILTIN(a_, b_) BUILTIN_##a_,
SH_EXPAND_ENUM_BUILTINS
#undef BUILTIN
    };

    //
    // Builtin Structure
    //

    class Builtin
    {
    public:
        using exec_type = void(Builtin::*)(Command const&) const;

        Builtin();
        Builtin(BuiltinKind kind, ut::cstrparam name, exec_type exec);

        inline bool valid() const { return m_exec != nullptr; }
        bool exec(Command & c) const;

        static bool find(ut::strparam name, Builtin& builtin);

    private:
        BuiltinKind     m_kind;
        ut::cstrview    m_name;
        exec_type       m_exec;

        //
        // Forward Decls
        //

#define BUILTIN(a_, b_) void exec##a_(Command const&) const;
SH_EXPAND_ENUM_BUILTINS
#undef BUILTIN
    };

    //
    // helpers
    //

}