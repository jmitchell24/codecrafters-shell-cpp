//
// Created by james on 22/03/25.
//

#pragma once

#include <ut/string.hpp>

namespace sh
{
#if 0
    /// scans a line of input into the shell
    class Scanner
    {
    public:
        Scanner(ut::strparam line);

        /// returns true if token was found. false means scanner has reached end of line
        bool nextToken(std::string& token);


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
                    advance();
                else
                {
                    token = strview{token_begin, cursor}.str();
                    advance();
                    return true;
                }
            }

            return false;
        }

    private:
        char const* m_text;
        char const* m_text_end;
        char const* m_arg_start;
        char const* m_arg_current;


        bool whitespace();


        char next();
        bool nextIf(char expected);
        bool nextIf(ut::strparam expected);

        bool isAtEnd() const;
        char peek() const;
        char peekNext() const;
    };
#endif
}
