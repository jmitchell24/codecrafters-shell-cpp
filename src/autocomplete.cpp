//
// Created by james on 01/04/25.
//
#include "path.hpp"
#include "builtins.hpp"
#include "autocomplete.hpp"
using namespace sh;

//
// ut
//
#include <ut/check.hpp>
using namespace ut;

//
// std
//
#include <set>
#include <filesystem>
#include <unordered_map>
using namespace std;

//
// Trie
//
struct TrieNode
{
    using children_type = std::unordered_map<char, TrieNode*>;
    children_type children;

    char character=0;
    bool is_end = false;

    TrieNode()
    { }

    inline bool isUnary() const { return children.size() == 1; }
    inline bool isLeaf() const { return children.size() == 0; }

    TrieNode*& child(char c)
    {
        return children[c];
    }

    TrieNode*& unary()
    {
        check(isUnary(), "node is not unary");
        return children.begin()->second;
    }

    void clear()
    {
        for (auto&& [k,v]: children)
        {
            if (v != nullptr)
            {
                v->clear();
                delete v;
            }
        }
        children.clear();
    }
} ;

struct Trie
{
    TrieNode root;
    size_t count=0;

    void insert(string const& s)
    {
        TrieNode* current = &root;

        for (auto&& it : s)
        {
            auto&& node = current->child(it);
            if (node == nullptr)
            {
                node = new TrieNode{};
                node->character = it;
            }
            current = node;
        }
        current->is_end = true;
        ++count;
    }

    void clear()
    {
        root.clear();
        count=0;
    }

    TrieNode* search(string const& s)
    {
        TrieNode* current = &root;

        for (auto&& it : s)
        {
            if (auto&& node = current->child(it))
            {
                current = node;
            }
            else
            {
                current = nullptr;
                break;
            }
        }

        return current;
    }

    bool findSingleHint(TrieNode* current, string const& prefix, string& hint)
    {
        if (current != nullptr)
        {

            if (current->is_end)
            {
                hint = prefix;
                if (current->isLeaf())
                    hint += ' ';
                return true;
            }

            if (current->isUnary())
            {
                auto&& single = current->unary();
                return findSingleHint(single, prefix + single->character, hint);
            }

        }

        return false;
    }

    void findMultipleHints(TrieNode* current, string const& prefix, vector<string>& hints)
    {
        if (current->is_end)
            hints.push_back(prefix);

        for (auto&& [k,v]: current->children)
        {
            if (v != nullptr)
                findMultipleHints(v, prefix+v->character, hints);
        }

    }


} static g_trie;

//
// AutoComplete -> Implementation
//

static set<string> makeComps()
{
    set<string> comps
    {
#define BUILTIN(a_, b_) b_,
SH_EXPAND_ENUM_BUILTINS
#undef BUILTIN
    };

    for (auto&& it : getEnvExes())
        comps.insert(it);
    return comps;
}

AutoComplete::Result AutoComplete::onTab(bool is_repeat, string const& prefix)
{
    if (prefix.empty())
        return { DoNothing{} };

    auto comps = makeComps();

    g_trie.clear();
    for (auto&& it : comps)
        g_trie.insert(it);

    auto node = g_trie.search(prefix);

    // no match, no hints
    if (node == nullptr)
        return { RingBell{} };

    // exact match
    if (node->is_end)
        return { DoNothing{} };

    // single hint (with possible descendants)
    if (string single_hint; g_trie.findSingleHint(node, prefix, single_hint))
    {
        return { ReplaceLine{ single_hint } };
    }

    // multiple hints
    if (is_repeat)
    {
        hints_type hints;
        g_trie.findMultipleHints(node, prefix, hints);
        sort(hints.begin(), hints.end());
        return { PrintHints{hints} };
    }

    return { RingBell{} };
}
