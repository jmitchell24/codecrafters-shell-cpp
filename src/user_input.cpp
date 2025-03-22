//
// Created by james on 21/03/25.
//


//
// sh
//
#include "user_input.hpp"
using namespace sh;

//
// ut
//
using namespace ut;

//
// std
//
using namespace std;


//
// UserInput Implementation
//

struct Scanner
{
    char const* text;
    char const* text_end;
    char const* cursor;


    static Scanner make(strparam line)
    {
        return Scanner{ line.begin(), line.end(), line.begin() };
    }

    /// returns true if token was found. false means scanner has reached end of line
    bool nextToken(string& token)
    {
        // advance through whitespace

        if (!takeWhitespace())
            return false;

        // quote handling

        if (peek() == '\'')
        {
            token = "";
            return takeQuotes(token);
        }

        // advance and take token

        char const* token_begin = cursor;
        while (!isAtEnd())
        {
            if (iswspace(peek()))
                break;
            advance();
        }

        token = strview{token_begin, cursor}.str();
        return true;
    }


    bool isAtEnd() const { return cursor == text_end; }
    void advance() { check(!isAtEnd(), "end of line"); ++cursor; }
    char peek() const { check(!isAtEnd(), "end of line"); return *cursor; }

    /// returns false if at end of line
    bool takeWhitespace()
    {
        while (!isAtEnd())
        {
            if (iswspace(peek()))
                advance();
            else
                return true;
        }

        return false;
    }

    /// returns false if at end of line
    bool takeQuotes(string& token)
    {
        check(peek() == '\'', "not quoted");
        advance();

        char const* token_begin = cursor;

        while (!isAtEnd())
        {
            if (peek() != '\'')
            {
                advance();
            }
            else
            {
                token += strview{token_begin, cursor}.str();
                advance();

                if (!isAtEnd() && peek() == '\'')
                    return takeQuotes(token);
                return true;
            }
        }

        return false;
    }
};

// string sanitize(strparam line)
// {
//     string s;
//
//     for (size_t i = 1; i < line.size(); ++i)
//     {
//         char x = line[i-1];
//         char y = line[i];
//
//         if (x == '\'' && y == '\'')
//     }
//
//     return s;
// }


UserInput::tokens_type UserInput::tokenizeLine(strparam line)
{

    auto scanner = Scanner{line.begin(), line.end(), line.begin() };

    tokens_type tokens;

    string token;
    while (scanner.nextToken(token))
        tokens.push_back(token);
    return tokens;


#if 0 //
    return trimsplit::containerWS(line);
#endif
}

