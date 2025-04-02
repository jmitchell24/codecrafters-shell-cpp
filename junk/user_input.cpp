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

UserInput::UserInput(ut::strparam line) :
    m_rdin {},
    m_rdout{},
    m_rderr{},
    m_tokens{tokenizeLine(line)}
{ }

/*
std::pair<bool, int> parse_redirect(std::string_view input) {
    if (input.length() != 2 || input[1] != '>') return { false, -1 };
    const char* digits = "0123456789";
    if (digits.find(input[0]) == std::string::npos) return { false, -1 };
    return { true, input[0] - '0' };
}
 */



UserInput::tokens_type UserInput::tokenizeLine(strparam line)
{
    tokens_type tokens;


    if (texts_type texts; tryUnquoteLine(line, texts))
    {
        for (auto&& it: texts)
        {
            tokens.push_back(it);
        }
    }



    return tokens;
}

bool UserInput::tryUnquoteLine(strparam line, texts_type& result)
{
    texts_type texts;
    string current_text;

    enum QuoteState { QS_NONE, QS_SINGLE, QS_DOUBLE };

    QuoteState quote_state = QS_NONE;
    bool escaped = false;

    for (auto&& it: line)
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
                    current_text += it;
                }
                else
                {
                    // Otherwise, both the backslash and the character are preserved
                    current_text += '\\';
                    current_text += it;
                }
            }
            else
            {
                // Outside quotes or in single quotes, \ escapes any character
                current_text += it;
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
            if (!current_text.empty())
            {
                texts.push_back(current_text);
                current_text.clear();
            }
            continue;
        }

        // Add character to current token
        current_text += it;
    }

    // Handle any remaining escaped character at the end
    if (escaped)
        current_text += '\\';

    // Add the last token if not empty
    if (!current_text.empty())
        texts.push_back(current_text);

    // Error state: unclosed quotes (bash would prompt for more input)
    if (quote_state != QS_NONE)
        return false;

    result = std::move(texts);
    return true;
}

