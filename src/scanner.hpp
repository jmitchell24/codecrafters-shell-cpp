//
// Created by james on 11/03/25.
//

#include <ut/string.hpp>

#pragma once

namespace sh
{
    class Scanner
    {
    public:
        Scanner(ut::strparam s);
        ut::strview takeToken();
        ut::strview takeAll();

    private:
        ut::strview m_text;
        size_t m_offset;
    };

}