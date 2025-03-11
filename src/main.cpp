#include <iostream>
#include <cstdlib>
#include <span>
#include <charconv>
#include <array>

using namespace std;

#include <ut/string.hpp>
#include <ut/check.hpp>
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


enum BuiltinKind
{
    BUILTIN_EXIT, BUILTIN_ECHO, BUILTIN_TYPE
};

struct Builtin;

struct Builtin
{
    using fn_type = void(*)(Builtin const&, Scanner&);

    BuiltinKind kind;
    fn_type     fn;

    cstrview    name;
    cstrview    desc;

};

void fnExit(Builtin const& b, Scanner& s);
void fnEcho(Builtin const& b, Scanner& s);
void fnType(Builtin const& b, Scanner& s);

array<Builtin, 3> static const BUILTINS =
{
    Builtin{ BUILTIN_EXIT, &fnExit, "exit", "exit is a shell builtin"_sv },
    Builtin{ BUILTIN_ECHO, &fnEcho, "echo", "echo is a shell builtin"_sv },
    Builtin{ BUILTIN_TYPE, &fnType, "type", "type is a shell builtin"_sv }
};

void fnExit(Builtin const& b, Scanner& s)
{
    auto arg = s.takeAll().trim();
    int code = 0;

    if (from_chars(arg.begin(), arg.end(), code).ec != errc{})
        cout << "warning: invalid exit code '" << arg << "\'\n";
    exit(code);
}

void fnEcho(Builtin const& b, Scanner& s)
{
    auto arg = s.takeAll();
    cout << arg << "\n";
}

void fnType(Builtin const& b, Scanner& s)
{
    auto arg = s.takeAll().trim();

    for (auto&& b: BUILTINS)
    {
        if (arg == b.name)
        {
            cout << b.desc << endl;
            return;
        }
    }

    cout << arg << ": not found" << endl;
}

void parseUserInput(strparam s)
{
    Scanner scanner{s};


    auto command = scanner.takeToken();

    for (auto&& b: BUILTINS)
    {
        if (command == b.name)
        {
            b.fn(b, scanner);
            return;
        }
    }

    cout << command << ": not found" << endl;
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
