//
// Created by james on 30/03/25.
//

//
// ut
//
#include <ut/string.hpp>

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

        static Shell& instance();

        Shell();

        inline bool loaded() const { return m_loaded; }

        void load();
        void unload();

        bool getLine(std::string& line);
        void addHistory(ut::strparam line);

    private:
        bool m_loaded;
    };

    [[maybe_unused]]
    static Shell& SHELL = Shell::instance();
}