#include "builtins.hpp"
#include "scanner.hpp"
#include "user_input.hpp"
using namespace sh;


#include <iostream>
#include <cstdlib>
#include <span>
#include <charconv>
#include <array>
#include <cstdio>
#include <dirent.h>
#include <print>
#include <sys/wait.h>
using namespace std;

#include <ut/string.hpp>
#include <ut/check.hpp>
using namespace ut;

bool exec(UserInput const& u)
{
    static char  ARG_BUFFER_CHARS[1000];
    static char* ARG_BUFFER[1000];

    if (u.empty())
    {
        return false;
    }

    // load argument buffer for 'execvp'

    {
        char* dst = ARG_BUFFER_CHARS;
        char* end = ARG_BUFFER_CHARS + 1000;

        size_t i = 0;
        for (; i < u.tokens().size(); ++i)
        {
            auto&& tok = u.tokens()[i];
            size_t gap = end - dst;

            tok.strncpy(dst, gap);

            ARG_BUFFER[i] = dst;
            dst += tok.size() + 1;
        }
        ARG_BUFFER[i] = nullptr;
    }

    auto pid = fork();

    // error
    if (pid == -1)
    {
        return false;
    }

    // new proc
    if (pid == 0)
    {
        execvp(ARG_BUFFER[0], ARG_BUFFER);
        // proc will end inside 'execvp'
        return false;
    }

    // old proc
    if (int status; waitpid(pid, &status, 0) == pid)
    {
        return true;
    }

    return false;

}

/// return false if shell should stop
bool eval(UserInput const& u)
{
    if (u.empty())
        return false;

    if (Builtin b; Builtin::find(u.nameText(), b))
    {
        b.fn(b, u);
        return true;
    }

    if (exec(u))
    {
        return true;
    }

    return false;
}


static char const* const SHELL_PREFIX = "$ ";





int main()
{
    string user_input_text;

    do
    {
        if (auto u = UserInput(user_input_text); !u.empty())
        {
            if (!eval(u))
                break;
        }

        print("{}", SHELL_PREFIX);
    }
    while (getline(cin, user_input_text));

    return EXIT_FAILURE;
}
