#include <iostream>
#include <cstdlib>
#include <span>
#include <charconv>

using namespace std;

#include <ut/string.hpp>
using namespace ut;

using arglist_t = span<strview>;

void cmdExit(arglist_t const& args)
{
    int code = EXIT_FAILURE;

    if (args.size() != 1)
    {
        auto&& a = args[0];
        from_chars(a.begin(), a.end(), code);
    }

    exit(code);
}

void parseCommand(arglist_t const& args)
{
    if (args.empty())
        return;

    auto&& command = args[0];
    if (command == "exit"_sv)
    {
        cmdExit(args.subspan(1));
    }
    else
    {

    }
}

void parseText(cstrparam s)
{
    auto args = trimsplit::containerWS(s);
    parseCommand(args);
}

int main()
{

    constexpr size_t BUFFER_SIZE = 2048;
    char buffer[BUFFER_SIZE];

    printf("$ ");

    if (fgets(buffer, BUFFER_SIZE, stdin) == nullptr)
    {
        printf("something went wrong...\n");
        return EXIT_FAILURE;
    }

    auto user_input = cstrview(buffer).trim();

    printf("%.*s: command not found\n", user_input.size(), user_input.begin());

    return EXIT_SUCCESS;

    // while (true)
    // {
    //     printf("$ ");
    //
    //     if (fgets(buffer, BUFFER_SIZE, stdin) == nullptr)
    //     {
    //         printf("something went wrong...\n");
    //         return EXIT_FAILURE;
    //     }
    //
    //     auto argv = trimsplit::containerWS(cstrview(buffer));
    //
    //     parseText(buffer);
    //
    // }


}
