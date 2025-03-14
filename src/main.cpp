#include "builtins.hpp"
#include "scanner.hpp"
using namespace sh;


#include <iostream>
#include <cstdlib>
#include <span>
#include <charconv>
#include <array>
#include <cstdio>
#include <dirent.h>
using namespace std;

#include <ut/string.hpp>
#include <ut/check.hpp>
using namespace ut;





// void fnExit(Builtin const& b, Scanner& s)
// {
//     auto arg = s.takeAll().trim();
//     int code = 0;
//
//     if (from_chars(arg.begin(), arg.end(), code).ec != errc{})
//         cout << "warning: invalid exit code '" << arg << "\'\n";
//     exit(code);
// }
//
// void fnEcho(Builtin const& b, Scanner& s)
// {
//     auto arg = s.takeAll();
//     cout << arg << "\n";
// }
//
// void fnType(Builtin const& b, Scanner& s)
// {
//     auto arg = s.takeAll().trim();
//
//     for (auto&& b: BUILTINS)
//     {
//         if (arg == b.name)
//         {
//             cout << b.desc << endl;
//             return;
//         }
//     }
//
//     cout << arg << ": not found" << endl;
// }


void parseUserInput(strparam s)
{
    Scanner scanner{s};

    auto command = scanner.takeToken();

    if (Builtin b; Builtin::find(command, b))
    {
        b.fn(b, scanner);
    }
    else
    {
        cout << command << ": not found\n";
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

        auto user_input = cstrview(buffer.data());

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
