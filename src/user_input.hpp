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
        using tokens_type = std::vector<ut::strview>;

        UserInput(ut::strparam line)
            : m_line{line}, m_tokens{ut::trimsplit::containerWS(line)}
        {}

        inline UserInput(UserInput const&)=default;
        inline UserInput(UserInput&&)=default;

        /// is there any usable data in user input (non-whitespace chars)?
        inline bool empty() const { return m_tokens.empty(); }

        inline bool hasArgs() const { return m_tokens.size() > 1; }

        /// the first token of user input (builtin name, program name, etc...)
        inline ut::strview nameText() const { check(!empty(), "must not be empty"); return m_tokens[0]; }

        /// the text containing the beginning and end of all arguments (everything after the first token)
        inline ut::strview argsText() const { check(!empty(), "must not be empty"); return ut::strview(m_tokens[1].begin(), m_tokens.back().end()); }

        /// the entire text of user input
        inline ut::strview text() const { return m_line; }

        /// the entire list of tokens, including the name
        inline tokens_type const& tokens() const { return m_tokens; }

    private:
        ut::strview m_line;
        tokens_type m_tokens;
    };
}
