#include <iostream>
#include <cstdlib>
#include <span>
#include <charconv>
#include <array>

using namespace std;

#include <ut/string.hpp>
using namespace ut;

using arglist_t = span<strview>;

void cmdExit(arglist_t const& args)
{
    int code = EXIT_FAILURE;

    if (args.size() == 1)
    {
        auto&& a = args[0];
        from_chars(a.begin(), a.end(), code);
    }

    exit(code);
}

bool parseCommand(arglist_t const& args)
{
    if (args.empty())
        return false;

    auto&& command = args[0];
    if (command == "exit"_sv)
    {
        cmdExit(args.subspan(1));
        return true;
    }


    return false;
}

void parseUserInput(strparam s)
{
    auto args = trimsplit::containerWS(s);

    if (!parseCommand(args))
    {
        printf("%.*s: invalid command\n", s.size(), s.data());
    }
}

int main()
{
    array<char, 1000> buffer{};

    while (true)
    {
        printf("$ ");

        if (fgets(buffer.data(), buffer.size(), stdin) == nullptr)
        {
            printf("something went wrong...\n");
            return EXIT_FAILURE;
        }

        auto user_input = strview(buffer.data(), strlen(buffer.data())-1);

        parseUserInput(user_input);

    }


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
