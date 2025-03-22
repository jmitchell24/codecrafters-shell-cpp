//
// Created by james on 19/03/25.
//

#pragma once

//
// ut
//
#include <ut/string.hpp>

//
// std
//
#include <vector>

#include "ut/check.hpp"

namespace sh
{
    class UserInput
    {
    public:
        using token_type = std::string;
        using tokens_type = std::vector<token_type>;

        UserInput(ut::strparam line)
            : m_tokens{tokenizeLine(line)}
        {}

        inline UserInput(UserInput const&)=default;
        inline UserInput(UserInput&&)=default;

        /// is there any usable data in user input (non-whitespace chars)?
        inline bool empty() const { return m_tokens.empty(); }
        inline bool count() const { return m_tokens.size(); }
        inline size_t arity() const { return empty() ? 0 : m_tokens.size()-1; }
        inline bool isUnary() const { return arity() == 1; }
        inline bool isBinary() const { return arity() == 2; }

        /// the first token of user input (builtin name, program name, etc...)
        inline ut::cstrview name() const
        {
            return empty() ? ut::cstrview{} : m_tokens[0];
        }

        inline ut::cstrview arg1() const
        {
            return arity() == 1 ? m_tokens[1] : ut::cstrview{};
        }

        /// the entire list of tokens, including the name
        inline tokens_type const& tokens() const { return m_tokens; }

        inline token_type const& tokenAt(size_t i) const
        {
            check(i < m_tokens.size(), "out of range");
            return m_tokens[i];
        }

    private:
        tokens_type m_tokens;

        static tokens_type tokenizeLine(ut::strparam line);

    };
}
