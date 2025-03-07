#include <iostream>
#include <cstdlib>
#include <span>
#include <charconv>
#include <array>

using namespace std;

#include <ut/string.hpp>
using namespace ut;

class Scanner
{
public:
    strview takeToken()
    {
        // find beginning of token
        size_t i = m_offset;
        for (; i < m_text.size(); ++i)
        {
            if (istoken(m_text[i]))
                break;
        }

        // find end of token
        size_t j = i;
        for (; j < m_text.size(); ++j)
        {
            if (!istoken(m_text[j]))
                break;
        }

        m_offset = j;
        return m_text.withIndices(i, m_offset);
    }

    strview takeAll()
    {
        // find beginning of first token
        size_t i = m_offset;
        for (; i < m_text.size(); ++i)
        {
            if (istoken(m_text[i]))
                break;
        }

        // find end of last token
        size_t j = m_text.size()-1;
        for (; j > i; --j)
        {
            if (istoken(m_text[j]))
                break;
        }

        m_offset = j+1;
        return m_text.withIndices(i, m_offset);
    }

    Scanner(strparam s)
        : m_text{s}, m_offset{0}
    {}
private:
    strview m_text;
    size_t m_offset;

    static bool istoken(char c)
    {
        return
            (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9') || (c >= 33 && c <= 47) ||
            (c >= 58 && c <= 64) || (c >= 91 && c <= 96) ||
            (c >= 123 && c <= 126);
    }
};

void parseUserInput(strparam s)
{
    Scanner scanner{s};


    auto command = scanner.takeToken();

    if (command == "exit"_sv)
    {
        auto arg = scanner.takeAll().trim();
        int code = 0;

        if (from_chars(arg.begin(), arg.end(), code).ec != errc{})
            cout << "warning: invalid exit code '" << arg << "\'\n";
        exit(code);
    }
    else if (command == "echo"_sv)
    {
        auto arg = scanner.takeAll();
        cout << arg << "\n";
    }
    else
    {
        cout << command << ": command not found\n";
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
