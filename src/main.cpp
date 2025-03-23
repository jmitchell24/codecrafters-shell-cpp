#include "builtins.hpp"
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

bool execSystem(UserInput const& u)
{
#if 0
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
#endif

    static array<char*, 128> ARG_BUFFER;

    size_t i = 0;
    for (; i < u.count(); ++i)
        ARG_BUFFER[i] = const_cast<char*>(u.tokenAt(i).data());
    ARG_BUFFER[i] = nullptr;

    auto pid = fork();

    if (pid < 0) // fork error
    {
        perror("shell");
    }
    else if (pid == 0) // child process
    {
        if (execvp(*ARG_BUFFER.data(), ARG_BUFFER.data()) == -1)
        {
            printf("%s: command not found\n", ARG_BUFFER[0]);
            fflush(stdout);
            exit(EXIT_FAILURE);
        }

        exit(EXIT_SUCCESS);
    }
    else // parent process
    {
        int status;
        do { waitpid(pid, &status, WUNTRACED); }
        while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return true;
}
#if 0 // https://brennan.io/2015/01/16/write-a-shell-in-c/
int lsh_launch(char **args)
{
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) {
        // Child process
        if (execvp(args[0], args) == -1) {
            perror("lsh");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Error forking
        perror("lsh");
    } else {
        // Parent process
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

#endif

/// return false if shell should stop
bool eval(UserInput const& u)
{
    if (u.empty())
        return false;

    if (Builtin b; Builtin::find(u.name(), b))
    {
        b.exec(u);
        return true;
    }

    if (execSystem(u))
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
