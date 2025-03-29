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

        Redirect(char const* filename, char const* mode, FILE* file);
        bool load();
        void unload();

    private:
        std::string     m_filename;
        std::string     m_mode;
        FILE*           m_file;
        int             m_fd;
        bool            m_loaded;
    };

    struct Command
    {
        using args_type = std::vector<std::string>;

        std::string rdin;
        std::string rdout;
        std::string rderr;
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

        bool execSystem() const;

        Redirect  rdIn() const;
        Redirect  rdOut() const;
        Redirect  rdErr() const;

        void dbgPrint() const;
    };
}