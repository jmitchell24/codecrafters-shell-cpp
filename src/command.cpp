//
// Created by james on 29/03/25.
//

//
// sh
//
#include "command.hpp"
using namespace sh;

//
// std
//
#include <array>
using namespace std;

//
// unix
//
#include <unistd.h>
#include <sys/wait.h>

//
// Redirect -> implementation
//
Redirect::Redirect(char const* filename, char const* mode, FILE* file)
    : m_filename{filename}, m_mode{mode}, m_file{file}, m_fd{-1}, m_loaded{false}
{
    check(!m_mode.empty(), "mode must not be empty");
    check(m_file != nullptr, "file must not be null");
}

bool Redirect::load()
{
    if (m_loaded || m_filename.empty())
        return false;
    m_fd = dup(fileno(m_file));
    m_loaded = true;
    return freopen(m_filename.c_str(), m_mode.c_str(), m_file) != nullptr;
}

void Redirect::unload()
{
    if (m_loaded)
    {
        fflush(m_file);
        dup2(m_fd, fileno(m_file));
        close(m_fd);
        m_loaded = false;
    }
}

//
// Command -> implementation
//

bool Command::execSystem() const
{
    // https://brennan.io/2015/01/16/write-a-shell-in-c/
    static array<char*, 128> ARG_BUFFER;

    size_t i = 0;
    for (; i < count(); ++i)
        ARG_BUFFER[i] = const_cast<char*>(args[i].data());
    ARG_BUFFER[i] = nullptr;

    auto pid = fork();

    if (pid < 0) // fork error
    {
        perror("shell");
    }
    else if (pid == 0) // child process
    {
        auto _rdin = rdIn();
        auto _rdout = rdOut();
        auto _rderr = rdErr();

        // load redirects
        _rdin.load();
        _rdout.load();
        _rderr.load();

        if (execvp(*ARG_BUFFER.data(), ARG_BUFFER.data()) == -1)
        {
            printf("%s: command not found\n", ARG_BUFFER[0]);
            exit(EXIT_FAILURE);
        }

        // unload redirects
        _rdin.unload();
        _rdout.unload();
        _rderr.unload();

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

Redirect Command::rdIn () const { return Redirect{rdin.c_str(),"r", stdin}; }
Redirect Command::rdOut() const { return Redirect{rdout.c_str(),"w", stdout}; }
Redirect Command::rdErr() const { return Redirect{rderr.c_str(),"w", stderr}; }

void Command::dbgPrint() const
{
    printf("command: \n");

    if (args.empty())
    {
        printf("<no args>\n");
    }
    else
    {
        for (size_t i = 0; i < args.size(); ++i)
            printf("args %d: %s\n", i, args[i].c_str());
    }



    if (!rdout.empty())
        printf("rdout: %s\n", rdout.c_str());
    else
        printf("rdout: <not specified>\n");

    if (!rderr.empty())
        printf("rderr: %s\n", rderr.c_str());
    else
        printf("rderr: <not specified>\n");

    if (!rdin.empty())
        printf("rdin: %s\n", rdin.c_str());
    else
        printf("rdin: <not specified>\n");
    printf("\n");
}