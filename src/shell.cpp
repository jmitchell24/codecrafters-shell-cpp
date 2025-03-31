//
// Created by james on 30/03/25.
//

//
// sh
//
#include "shell.hpp"
using namespace sh;

//
// ut
//
#include "ut/check.hpp"
using namespace ut;

//
// std
//
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

static bool autoComplete(strparam prefix, string& line)
{
    if (prefix == "ech"_sv)
    {
        line = "echo "_sv;
        return true;
    }

    if (prefix == "exi"_sv)
    {
        line = "exit "_sv;
        return true;
    }

    for (auto&& it: getPathExes())
    {
        auto sv = strview(it);
        if (sv.size() >= prefix.size() && sv.beginsWith(prefix))
        {
            line = it;
            line += ' ';
            return true;
        }
    }

    return false;
}

bool Shell::getLine(string& line)
{
    check(m_loaded, "is not loaded");

    enableRawMode();
    rawWrite(prompt);

    string buffer;
    for (;;)
    {
        char c = rawRead();

        switch (c)
        {
            case '\t':
                //nopath_impl;
                if (string ac_line; autoComplete(buffer, ac_line))
                {
                    auto line_reset = "\x1B[0G\x1B[2K"_sv;
                    rawWrite(line_reset);
                    rawWrite(prompt);
                    rawWrite(ac_line);
                    buffer = ac_line;

                }
                else
                {
                    rawWrite("\a"_sv);
                }
                break;

            case 127:
                if (!buffer.empty())
                {
                    buffer.pop_back();
                    rawWrite("\b \b"_sv);
                }
                break;

            case CTRL_KEY('c'):
                exit(EXIT_SUCCESS);
                break;

            case '\0':
            case '\r':
            case '\n':
                line = buffer;
                rawWrite("\n"_sv);
                disableRawMode();

                return true;

            default:
                buffer += c;
                rawWrite(&c, 1);
                break;
        }
    }

    disableRawMode();
    return false;
}

