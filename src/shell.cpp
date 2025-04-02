//
// Created by james on 30/03/25.
//

//
// sh
//
#include "shell.hpp"
#include "escapes.hpp"
#include "autocomplete.hpp"
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

void rawputs(char const* buf, size_t sz)
{
    if (write(STDOUT_FILENO, buf, sz) < 0)
        die("write");
}

void rawputs(strparam s)
{
    rawputs(s.data(), s.size());
}

void rawputc(char c)
{
    rawputs(&c, 1);
}

char rawgetc()
{
    char c;
    if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN)
        die("read");
    return c;
}

void rawsync()
{
    fsync(STDOUT_FILENO);
}

Shell::Shell()
    : m_loaded{false}
{ }


Shell& Shell::instance()
{
    static Shell x;
    return x;
}

static AutoComplete g_auto_complete;

bool Shell::getLine(string& line)
{
    enableRawMode();
    rawputs(prompt);

    string buffer;

    char prev_c=0, c=0;

    for (;;)
    {
        prev_c = c;
        c = rawgetc();

        switch (c)
        {
            case '\t': // tab
                switch (auto ac = g_auto_complete.onTab(prev_c == c, buffer); ac.type())
                {
                    case AutoComplete::DO_NOTHING:
                        break; // noop

                    case AutoComplete::RING_BELL:
                        rawputs(TERM_VISUAL_BELL);
                        rawputs(TERM_BELL);
                        rawputs(TERM_VISUAL_BELL_OFF);
                        break;

                    case AutoComplete::PRINT_HINTS:
                        rawputc('\n');
                        for (auto&& it: ac.asPrintHints().hints)
                        {
                            rawputs(it);
                            rawputs("  "_sv);
                        }

                        rawputc('\n');
                        rawputs(prompt);
                        rawputs(buffer);
                        break;

                    case AutoComplete::REPLACE_LINE:
                        {
                            auto ac_line = ac.asReplaceLine().line;
                            rawputs(TERM_CLEAR_LINE);
                            rawputs(TERM_CURSOR_COLUMN(0));
                            rawputs(prompt);
                            rawputs(ac_line);

                            buffer = ac_line;
                        }
                        break;
                }
                break;

            case 127: // backspace
                if (!buffer.empty())
                {
                    buffer.pop_back();
                    rawputs(TERM_CURSOR_LEFT(1));
                    rawputc(' ');
                    rawputs(TERM_CURSOR_LEFT(1));
                }
                break;

            case CTRL_KEY('c'): // ctrl+c
                exit(EXIT_SUCCESS);
                break;

            case '\0':
            case '\r':
            case '\n': // newline / eof
                line = buffer;
                rawputs(TERM_CURSOR_NEXT_LINE(1));

                disableRawMode();

                return true;

            default: // everything else...
                buffer += c;
                rawputc(c);

#ifdef DEBUG_PROMPT
                _do_debug(buffer);
#endif
                break;

        }
    }

    disableRawMode();
    return false;
}







