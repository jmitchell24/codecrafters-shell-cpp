//
// Created by james on 30/03/25.
//

//
// ut
//
#include <ut/string.hpp>
#include <ut/func/delegate.hpp>


//
// std
//
#include <string>

namespace sh
{
    class Shell
    {
    public:
        std::string prompt = "$ ";

        using debug_func_type = ut::func<std::string(std::string const&)>;
        debug_func_type debug;

        ut::func<void()> on_line_changed;
        ut::func<void()> on_tab;

        static Shell& instance();

        Shell();

        bool getLine(std::string& line);

        inline std::string const& lineBuffer() const
        { return m_line_buffer; }



    private:
        std::string m_line_buffer;
        bool m_loaded;
    };

    [[maybe_unused]]
    static Shell& SHELL = Shell::instance();
}