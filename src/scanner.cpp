//
// Created by james on 29/03/25.
//

//
// sh
//
#include "scanner.hpp"
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
// Scanner -> implementation
//

Scanner::Scanner()
{ }

Scanner::Scanner(strparam s)
    : m_literals{}, m_loc{0}
{
    tryUnquoteLiterals(s, m_literals);
}

Token Scanner::scan()
{
    Token token;

    if (!isAtEnd())
    {
        if (nextIf("1>"_sv) || nextIf(">"_sv))
            token = Token::makeTokenRedirect(TokenRedirect::OUT, next());
        else if (nextIf("1>>"_sv) || nextIf(">>"_sv))
            token = Token::makeTokenRedirect(TokenRedirect::OUT, next(), true);
        else if (nextIf("2>"_sv))
            token = Token::makeTokenRedirect(TokenRedirect::ERR, next());
        else if (nextIf("2>>"_sv))
            token = Token::makeTokenRedirect(TokenRedirect::ERR, next(), true);
        else
            token = Token::makeTokenWord(next());
    }

    return token;
}

strparam Scanner::next()
{
    if (m_loc < m_literals.size())
        return m_literals[m_loc++];
    return m_literals[m_loc];
}

bool Scanner::nextIf(strparam s)
{
    if (m_loc < m_literals.size() && s == m_literals[m_loc])
    {
        ++m_loc;
        return true;
    }

    return false;
}

bool Scanner::isAtEnd() const
{
    return !(m_loc < m_literals.size());
}

size_t Scanner::hasNext() const
{
    return m_loc < m_literals.size()-1;
}


strparam Scanner::peek() const
{
    check(m_loc < m_literals.size(), "no current literal to peek");
    return m_literals[m_loc];
}

strparam Scanner::peekNext() const
{
    check(m_loc < m_literals.size()-1, "no next literal to peek");
    return m_literals[m_loc+1];
}

bool Scanner::tryUnquoteLiterals(strparam s, literals_type& result)
{
    vector<std::string> literals;
    string current_literal;

    enum QuoteState { QS_NONE, QS_SINGLE, QS_DOUBLE };

    QuoteState quote_state = QS_NONE;
    bool escaped = false;

    for (auto&& it: s)
    {
        // Handle escape character
        if (escaped)
        {
            // In double quotes, only certain characters are treated specially when escaped
            if (quote_state == QS_DOUBLE)
            {
                // In double quotes, \ only escapes $, `, ", \, and newline
                if (it == '$' || it == '`' || it == '"' || it == '\\' || it == '\n')
                {
                    current_literal += it;
                }
                else
                {
                    // Otherwise, both the backslash and the character are preserved
                    current_literal += '\\';
                    current_literal += it;
                }
            }
            else
            {
                // Outside quotes or in single quotes, \ escapes any character
                current_literal += it;
            }
            escaped = false;
            continue;
        }

        // Check for escape character
        if (it == '\\' && quote_state != QS_SINGLE)
        {
            escaped = true;
            continue;
        }

        // Handle quotes
             if (it == '\'' && quote_state == QS_NONE)   { quote_state = QS_SINGLE; continue; }
        else if (it == '\'' && quote_state == QS_SINGLE) { quote_state = QS_NONE;   continue; }
        else if (it == '"'  && quote_state == QS_NONE)   { quote_state = QS_DOUBLE; continue; }
        else if (it == '"'  && quote_state == QS_DOUBLE) { quote_state = QS_NONE;   continue; }

        // Handle whitespace outside of quotes
        if (isspace(it) && quote_state == QS_NONE)
        {
            if (!current_literal.empty())
            {
                literals.push_back(current_literal);
                current_literal.clear();
            }
            continue;
        }

        // Add character to current token
        current_literal += it;
    }

    // Handle any remaining escaped character at the end
    if (escaped)
        current_literal += '\\';

    // Add the last token if not empty
    if (!current_literal.empty())
        literals.push_back(current_literal);

    // Error state: unclosed quotes (bash would prompt for more input)
    if (quote_state != QS_NONE)
        return false;

    result = std::move(literals);
    return true;
}

