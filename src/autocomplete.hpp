//
// Created by james on 01/04/25.
//

#pragma once

//
// ut
//
#include <ut/string.hpp>

//
// std
//
#include <string>
#include <variant>
#include <vector>

namespace sh
{

    struct AutoComplete
    {
        using hints_type = std::vector<std::string>;

        struct DoNothing    {};
        struct RingBell     {};
        struct ReplaceLine  { std::string line; };
        struct PrintHints   { hints_type hints; };

        enum ResultType { DO_NOTHING, RING_BELL, REPLACE_LINE, PRINT_HINTS };

        struct Result
        {
            using data_type = std::variant<DoNothing, RingBell, ReplaceLine, PrintHints>;
            data_type data;

            inline ResultType type() const { return static_cast<ResultType>(data.index()); }

            inline DoNothing& asDoNothing() { return std::get<DO_NOTHING>(data); }
            inline RingBell& asRingBell() { return std::get<RING_BELL>(data); }
            inline ReplaceLine& asReplaceLine() { return std::get<REPLACE_LINE>(data); }
            inline PrintHints& asPrintHints() { return std::get<PRINT_HINTS>(data); }

            inline bool isDoNothing() const { return data.index() == DO_NOTHING; }
            inline bool isRingBell() const { return data.index() == RING_BELL; }
            inline bool isReplaceLine() const { return data.index() == REPLACE_LINE; }
            inline bool isPrintHints() const { return data.index() == PRINT_HINTS; }
        };

        Result onTab(bool is_repeat, std::string const& prefix);
        //void debug(std::string& output, ut::strparam prefix);

    private:
        hints_type m_hints;
        bool       m_armed=false;
    };
}