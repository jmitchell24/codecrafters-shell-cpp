#include "builtins.hpp"
#include "command.hpp"
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
#include <sstream>
using namespace std;

#include <ut/string.hpp>
#include <ut/check.hpp>
using namespace ut;

//
// linenoise
//
extern "C"
{
    #include "linenoise.h"
}

//
// rang
//
#include "rang.hpp"


/// return false if shell should stop
bool eval(Command const& c)
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

static const auto SHELL_PREFIX = string("$ ");



#if 0
int linenoiseHistoryAdd(const char *line);
int linenoiseHistorySetMaxLen(int len);
int linenoiseHistorySave(const char *filename);
int linenoiseHistoryLoad(const char *filename);
#endif

#include "scanner.hpp"
using namespace sh;

#if 0
int main()
{
    linenoiseHistorySetMaxLen(1000);

    // std::vector<std::string> test_commands = {
    //     "ls -l",
    //     "echo 'Hello, world!'",
    //     "echo \"Hello, world!\"",
    //     "echo \"Escaped \\\" quote\"",
    //     "echo 'Single quoted string with spaces'",
    //     "echo \"Special chars: $HOME `date`\"",
    //     "grep 'pattern with spaces' file.txt",
    //     "echo \"Backslash at end\\\""
    // };

    char* line = nullptr;
    while ((line = linenoise("$ ")) != nullptr)
    {
        try {
            ShellParser parser(line);
            auto ast = parser.parse();

            std::cout << "Parsing command: " << line << std::endl;
            ast->print(std::cout);
            std::cout << "\n---\n";

            linenoiseHistoryAdd(line);

        } catch (const std::exception& e) {
            std::cerr << "Error parsing '" << line << "': " << e.what() << std::endl;
        }
    }

    return 0;
}
#endif

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
                case TokenRedirect::OUT: command.rdout = rd.filename; break;
                case TokenRedirect::ERR: command.rderr = rd.filename; break;
                case TokenRedirect::IN : command.rdin = rd.filename;
                default: nopath_case(TokenRedirect::Kind);
            }
        }
    }

    c = command;
    return true;
}

int main()
{

    linenoiseHistorySetMaxLen(1000);



    char* line = nullptr;
    while((line = linenoiseCodeCrafters(SHELL_PREFIX.c_str())) != nullptr)
    {
        if (Command c; tryMakeCommand(cstrparam(line), c))
        {
            //c.dbgPrint();
            if (eval(c))
                linenoiseHistoryAdd(line);
        }

        linenoiseFree(line);
    }



    return EXIT_SUCCESS;
//    string user_input_text;
//
//    do
//    {
//        if (auto u = UserInput(user_input_text); !u.empty())
//        {
//            if (!eval(u))
//                break;
//        }
//
//        printf("%.*s", SHELL_PREFIX.size(), SHELL_PREFIX.data());
//    }
//    while (getline(cin, user_input_text));
//
//    return EXIT_FAILURE;
}
