//
// Created by james on 29/03/25.
//

#pragma once

//
// std
//
#include <vector>
#include <string>

//
// ut
//
#include <ut/string.hpp>
#include <ut/check.hpp>


namespace sh
{
    class Redirect
    {
    public:
        enum Kind { OUT, ERR } kind = OUT;
        std::string filename = "";
        bool append = false;

        inline bool loaded() const { return m_loaded; }

        bool load();
        void unload();

        void dbgPrint() const;

    private:
        FILE* m_file=nullptr;
        int   m_fd=-1;
        bool  m_loaded=false;
    };

    struct Command
    {
        using args_type = std::vector<std::string>;

        Redirect rdout;
        Redirect rderr;
        args_type   args;

        inline bool empty() const { return args.empty(); }
        inline size_t count() const { return args.size(); }
        inline size_t arity() const { return empty() ? 0 : args.size()-1; }
        inline bool isUnary() const { return arity() == 1; }
        inline bool isBinary() const { return arity() == 2; }

        inline ut::cstrview name() const
        { return empty() ? ut::cstrview{} : args[0]; }

        inline ut::cstrview arg1() const
        { return arity() == 1 ? args[1] : ut::cstrview{}; }

        bool execSystem();

        void dbgPrint() const;
    };
}