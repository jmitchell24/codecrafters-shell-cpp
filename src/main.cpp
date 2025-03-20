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
#include <sys/wait.h>
using namespace std;

#include <ut/string.hpp>
#include <ut/check.hpp>
using namespace ut;

bool exec(UserInput const& u)
{
    static array<char, 1000>  ARG_BUFFER_CHARS;
    static array<char*, 1000> ARG_BUFFER;

    if (u.empty())
    {
        return false;
    }

    // load argument buffer for 'execvp'

    {
        char* dst = ARG_BUFFER_CHARS.data();
        char* end = ARG_BUFFER_CHARS.data() + ARG_BUFFER_CHARS.size();

        size_t i = 0;
        for (; i < u.tokens().size(); ++i)
        {
            auto&& tok = u.tokens()[i];
            size_t gap = end - dst;

            tok.strncpy(dst, gap);
            ARG_BUFFER[i] = dst;

            dst += tok.size();
            *dst = '\0';
            dst += 1;
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
        execvp(*ARG_BUFFER.data(), ARG_BUFFER.data());
        // if there is no error, proc will end inside 'execvp'

        printf("%s: command not found\n", *ARG_BUFFER.data());
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
        b.exec(u);
        return true;
    }

    if (exec(u))
    {
        return true;
    }

    return false;
}


static const auto SHELL_PREFIX = "$ "_sv;





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

        printf("%.*s", SHELL_PREFIX.size(), SHELL_PREFIX.data());
    }
    while (getline(cin, user_input_text));

    return EXIT_FAILURE;
}
