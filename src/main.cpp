#include "builtins.hpp"
#include "command.hpp"
#include "shell.hpp"
#include "scanner.hpp"
using namespace sh;


#include <iostream>
#include <cstdlib>
#include <span>
#include <cstdio>
#include <dirent.h>
using namespace std;

#include <ut/string.hpp>
#include <ut/check.hpp>
using namespace ut;


/// return false if shell should stop
bool eval(Command & c)
{
    if (c.empty())
        return false;

    if (Builtin b; Builtin::find(c.name(), b))
    {
        return b.exec(c);
    }

    if (c.execSystem())
    {
        return true;
    }

    return false;
}

bool tryMakeCommand(strparam s, Command& c)
{
    Scanner scanner{s};

    Command command;
    for (auto it = scanner.scan(); !it.empty(); it = scanner.scan())
    {
        if (it.isWord())
        {
             command.args.push_back(it.asWord().text);
        }
        else if (it.isRedirect())
        {
            auto&& rd = it.asRedirect();
            switch (rd.kind)
            {
                case TokenRedirect::OUT:
                    command.rdout.filename = rd.filename;
                    command.rdout.append = rd.append;
                    break;

                case TokenRedirect::ERR:
                    command.rderr.filename = rd.filename;
                    command.rderr.append = rd.append;
                    break;

                default: nopath_case(TokenRedirect::Kind);
            }
        }
    }

    c = command;
    return true;
}

int main()
{

    SHELL.on_tab = fn([]
    {

    });

    SHELL.on_line_changed = fn([]
    {

    });


    string line;
    while(SHELL.getLine(line))
    {
        if (Command c; tryMakeCommand(line, c))
        {
            eval(c);
        }
    }


    return EXIT_SUCCESS;
}
