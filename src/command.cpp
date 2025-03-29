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

FILE* getFILE(Redirect::Kind kind)
{
    switch (kind)
    {
    case Redirect::ERR: return stderr;
    case Redirect::OUT: return stdout;
    default: nopath_case(Redirect::Kind);
    }
    return nullptr;
}

bool Redirect::load()
{
    if (m_loaded || filename.empty())
        return false;

    // get standard file pointer
    m_file = getFILE(kind);

    // copy original fd
    m_fd = dup(fileno(m_file));

    freopen(filename.c_str(), append ? "a" : "w", m_file);

    m_loaded = true;
    return true;
}

void Redirect::unload()
{
    if (m_loaded)
    {
        check(m_file != nullptr, "file must not be null");

        // complete all pending outputs
        fflush(m_file);

        //
        dup2(m_fd, fileno(m_file));
        close(m_fd);
        m_loaded = false;
    }
}

void Redirect::dbgPrint() const
{
    printf("<debug print not implemented>\n");
}


//
// Command -> implementation
//

bool Command::execSystem()
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
        check(!rdout.loaded(), "redirect should not be loaded rn");
        check(!rderr.loaded(), "redirect should not be loaded rn");

        // load redirects
        rdout.load();
        rderr.load();

        if (execvp(*ARG_BUFFER.data(), ARG_BUFFER.data()) == -1)
        {
            printf("%s: command not found\n", ARG_BUFFER[0]);
            exit(EXIT_FAILURE);
        }

        // unload redirects
        rdout.unload();
        rderr.unload();

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


    rdout.dbgPrint();
    rderr.dbgPrint();
    printf("\n");
}