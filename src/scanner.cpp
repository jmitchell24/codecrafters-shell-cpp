//
// Created by james on 3/13/25.
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

inline bool istoken(char c)
{
    return
            (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9') || (c >= 33 && c <= 47) ||
            (c >= 58 && c <= 64) || (c >= 91 && c <= 96) ||
            (c >= 123 && c <= 126);
}

Scanner::Scanner(strparam s)
        : m_text{s}, m_offset{0}
{}

strview Scanner::takeToken()
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

strview Scanner::takeAll()
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




