//
// Created by james on 29/03/25.
//

#pragma once

//
// std
//
#include <variant>
#include <vector>

//
// ut
//
#include <ut/string.hpp>
#include <ut/check.hpp>

//
// sh
//



namespace sh
{
    struct TokenWord
    {
        std::string text = "";
    };

    struct TokenRedirect
    {
        enum Kind { OUT, IN, ERR } kind = OUT;
        std::string filename;
    };

    struct Token
    {
        using variant_type = std::variant<
            std::monostate,
            TokenWord,
            TokenRedirect
            >;

        inline bool empty() const { return data.index() == 0; }
        inline bool isWord() const { return data.index() == 1; }
        inline bool isRedirect() const { return data.index() == 2; }

        inline TokenWord& asWord() { return get<1>(data); }
        inline TokenRedirect& asRedirect() { return get<2>(data); }

        inline static Token makeEmptyToken()
        { return Token{}; }

        inline static Token makeTokenWord(ut::strparam s)
        { return Token{TokenWord{s.str()}}; }

        inline static Token makeTokenRedirect(TokenRedirect::Kind kind, ut::strparam filename)
        { return Token{TokenRedirect{kind, filename.str()}}; }

        inline operator bool() const { return !empty(); }

        variant_type data;
    };

    class Scanner
    {
    public:
        Scanner();
        Scanner(ut::strparam s);

        Token scan();

    private:
        using literals_type = std::vector<std::string>;

        literals_type   m_literals;
        size_t          m_loc;

        static bool tryUnquoteLiterals(ut::strparam s, literals_type& result);

        ut::strparam next();
        bool nextIf(ut::strparam s);
        bool isAtEnd() const;
        size_t hasNext() const;
        ut::strparam peek() const;
        ut::strparam peekNext() const;
    };
}