//
// Created by james on 30/03/25.
//

//
// sh
//
#include "shell.hpp"

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
#include <set>
#include <variant>
#include <filesystem>
using namespace std;

//
// unix
//
#include <unistd.h>
#include <termios.h>

//https://viewsourcecode.org/snaptoken/kilo/index.html

#define CTRL_KEY(k) ((k) & 0x1f)

#define CALL_CHECK(_fn, ...) if (_fn(__VA_ARGS__) < 0) { die(#_fn); }

void die(char const* s)
{
    perror(s);
    exit(EXIT_FAILURE);
}

void disableRawMode()
{
    termios term{};

    if (tcgetattr(STDIN_FILENO, &term) < 0)
        die("tcgetattr");

    term.c_lflag |= (ICANON | ECHO);

    if (tcsetattr(STDIN_FILENO, TCSANOW, &term) < 0)
        die("tcsetattr");
}

void enableRawMode()
{
    termios term{};

    if (tcgetattr(STDIN_FILENO, &term) < 0)
        die("tcgetattr");

    term.c_lflag &= ~(ICANON | ECHO);

    if (tcsetattr(STDIN_FILENO, TCSANOW, &term) < 0)
        die("tcsetattr");

    atexit(disableRawMode);
}

void rawWrite(char const* buf, size_t sz)
{
    if (write(STDOUT_FILENO, buf, sz) < 0)
        die("write");
}

void rawWrite(strparam s)
{
    rawWrite(s.data(), s.size());
}

char rawRead()
{
    char c;
    if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN)
        die("read");
    return c;
}

Shell::Shell()
    : m_loaded{false}
{ }

void Shell::load()
{
    if (m_loaded)
        return;
    enableRawMode();
    m_loaded = true;
}

void Shell::unload()
{
    if (!m_loaded)
        return;
    disableRawMode();
    m_loaded = false;
}


Shell& Shell::instance()
{
    static Shell x;
    return x;
}

void Shell::addHistory(strparam line)
{

}

struct AutoCompleteResult
{

};

struct AutoComplete
{
    struct DoNothing    {};
    struct RingBell     {};
    struct ReplaceLine  { string line; };
    struct PrintHints   { vector<string> hints; };

    enum ResultType { DO_NOTHING, RING_BELL, REPLACE_LINE, PRINT_HINTS };

    using result_type = variant<DoNothing, RingBell, ReplaceLine, PrintHints>;

    using hints_type = vector<string>;
    hints_type hints;

    result_type onTab(bool is_repeat, strparam prefix)
    {
        if (prefix.empty())
            return DoNothing{};

        if (!is_repeat)
        {
            auto comps = makeComps();
            for (auto&& it: comps)
            {
                if (auto sv = strview(it); sv.size() >= prefix.size() && sv.beginsWith(prefix))
                {
                    hints.push_back(it);
                }
            }



            if (hints.size() == 1)
                return ReplaceLine{hints[0] + " "};

            if (hints.size() > 1)
                return RingBell{};
        }

        if (!hints.empty())
        {
            check(hints.size() > 1, "hints should never have just 1 element");
            auto res = PrintHints{hints};
            hints.clear();
            return res;
        }

        return RingBell{};
    }

    set<string> makeComps()
    {
        set<string> comps
        {
#define BUILTIN(a_, b_) b_,
    SH_EXPAND_ENUM_BUILTINS
#undef BUILTIN
        };

        for (auto&& it : getPathExes())
            comps.insert(it);
        return comps;
    }

    vector<string> getPathExes()
    {
        namespace fs = std::filesystem;

        auto path_var = string(getenv("PATH"));
        auto paths = trimsplit::container(path_var, [](auto&& it)
        {
           return it == ':';
        });

        vector<string> exes;

        for (auto&& it: paths)
        {
            auto path = it.str();

            if (fs::exists(path) && fs::is_directory(path))
            {
                for (auto&& entry : fs::directory_iterator(path))
                {
                    if (entry.is_regular_file())
                        exes.push_back(entry.path().filename().string());
                }
            }
        }

        return exes;
    }


} static g_auto_complete;

#if 0
enum AutoCompleteAction
{
    AC_REPLACE,
    AC_PROMPT
};

static AutoCompleteAction autoComplete(strparam prefix, string& line)
{
    static vector<string> complete_options;
    static bool armed=false;

    if (armed)
    {
        line = prefix.str();
        line += '\n';
        for (auto&& it: complete_options)
        {
            line += it;
            line += ' ';
        }
        return AC_REPLACE;
    }

    complete_options.clear();
    armed = false;

    if (prefix == "ech"_sv)
    {
        line = "echo "_sv;
        return AC_REPLACE;
    }

    if (prefix == "exi"_sv)
    {
        line = "exit "_sv;
        return AC_REPLACE;
    }

    for (auto&& it: getPathExes())
    {
        auto sv = strview(it);
        if (sv.size() >= prefix.size() && sv.beginsWith(prefix))
        {
            complete_options.push_back(it);
        }
    }

    if (complete_options.size() == 1)
    {
        line = complete_options[0];
        line += ' ';
        return AC_REPLACE;
    }
    else if (complete_options.size() > 1)
    {
        if (!armed)
        {
            armed = true;
        }
        else
        {

        }
        if (armed)
        {
            line +
            return AC_REPLACE;
        }
    }


    return false;
}
#endif

bool Shell::getLine(string& line)
{
    check(m_loaded, "is not loaded");

    enableRawMode();
    rawWrite(prompt);

    string buffer;

    char prev_c=0, c=0;

    for (;;)
    {
        prev_c = c;
        c = rawRead();

        switch (c)
        {
            case '\t': // tab
                switch (auto ac = g_auto_complete.onTab(prev_c == c, buffer); ac.index())
                {
                    case AutoComplete::DO_NOTHING:
                        break; // noop

                    case AutoComplete::RING_BELL:
                        rawWrite("\a"_sv);
                        break;

                    case AutoComplete::PRINT_HINTS:
                        rawWrite("\n"_sv);
                        for (auto&& it: get<AutoComplete::PrintHints>(ac).hints)
                        {
                            rawWrite(it);
                            rawWrite("  "_sv);
                        }
                        rawWrite("\n"_sv);
                        rawWrite(prompt);
                        rawWrite(buffer);
                        break;

                    case AutoComplete::REPLACE_LINE:
                        {
                            auto line_reset = "\x1B[0G\x1B[2K"_sv;
                            auto ac_line = get<AutoComplete::ReplaceLine>(ac).line;
                            rawWrite(line_reset);
                            rawWrite(prompt);
                            rawWrite(ac_line);
                            buffer = ac_line;
                        }
                        break;
                }
                break;

            case 127: // backspace
                if (!buffer.empty())
                {
                    buffer.pop_back();
                    rawWrite("\b \b"_sv);
                }
                break;

            case CTRL_KEY('c'): // ctrl+c
                exit(EXIT_SUCCESS);
                break;

            case '\0':
            case '\r':
            case '\n': // newline / eof
                line = buffer;
                rawWrite("\n"_sv);
                disableRawMode();

                return true;

            default: // everything else...
                buffer += c;
                rawWrite(&c, 1);
                break;
        }
    }

    disableRawMode();
    return false;
}

