//
// Created by james on 22/03/25.
//

#pragma once

#include <ut/string.hpp>

//
// std
//
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <memory>
#include <stdexcept>
#include <variant>
#include <optional>
#include <variant>

namespace sh
{


// Forward declarations
class Node;
class CommandNode;
class PipelineNode;
class SequenceNode;
class RedirectionNode;
class SubshellNode;

    // Base Abstract Syntax Tree Node
    class Node {
    public:
        virtual ~Node() = default;
        virtual void print(std::ostream& os, int indent = 0) const = 0;
    };

    // Represents a simple command with arguments and optional redirections
    class CommandNode : public Node
    {
    public:
        std::string command;
        std::vector<std::string> args;
        std::optional<std::string> input_redirect;
        std::optional<std::string> output_redirect;
        bool append_output = false;

        void print(std::ostream& os, int indent = 0) const override
        {
            os << std::string(indent, ' ') << "Command: " << command << std::endl;

            if (!args.empty()) {
                os << std::string(indent, ' ') << "  Arguments:" << std::endl;
                for (const auto& arg : args) {
                    os << std::string(indent, ' ') << "    - " << arg << std::endl;
                }
            }

            if (input_redirect) {
                os << std::string(indent, ' ') << "  Input Redirect: " << *input_redirect << std::endl;
            }

            if (output_redirect) {
                os << std::string(indent, ' ')
                   << "  Output Redirect: " << *output_redirect
                   << (append_output ? " (append)" : " (overwrite)") << std::endl;
            }
        }
    };

// Represents a pipeline of commands (connected by |)
class PipelineNode : public Node {
public:
    std::vector<std::shared_ptr<CommandNode>> commands;

    void print(std::ostream& os, int indent = 0) const override {
        os << std::string(indent, ' ') << "Pipeline:" << std::endl;
        for (const auto& cmd : commands) {
            cmd->print(os, indent + 2);
        }
    }
};

// Represents a sequence of commands (connected by ; or &)
class SequenceNode : public Node {
public:
    enum class Type { SEQUENTIAL, BACKGROUND };

    std::vector<std::shared_ptr<Node>> commands;
    Type type;

    void print(std::ostream& os, int indent = 0) const override {
        os << std::string(indent, ' ')
           << "Sequence (" << (type == Type::SEQUENTIAL ? "sequential" : "background") << "):"
           << std::endl;

        for (const auto& cmd : commands) {
            cmd->print(os, indent + 2);
        }
    }
};

// Represents a subshell (commands within parentheses)
class SubshellNode : public Node
{
public:
    std::shared_ptr<Node> command;

    void print(std::ostream& os, int indent = 0) const override {
        os << std::string(indent, ' ') << "Subshell:" << std::endl;
        command->print(os, indent + 2);
    }
};

// Shell Grammar Parser
class ShellParser {
private:
    std::istringstream input;
    char current_char = ' ';

    // Lexer-like methods
    void consume() {
        input.get(current_char);
        if (input.eof()) current_char = '\0';
    }

    void skip_whitespace() {
        while (std::isspace(current_char)) consume();
    }

    std::string parse_word() {
        std::string word;
        bool in_quotes = false;
        char quote_char = '\0';

        while (current_char != '\0') {
            if (!in_quotes) {
                if (std::isspace(current_char) ||
                    current_char == '|' ||
                    current_char == ';' ||
                    current_char == '&' ||
                    current_char == '(' ||
                    current_char == ')' ||
                    current_char == '<' ||
                    current_char == '>') {
                    break;
                }

                if (current_char == '"' || current_char == '\'') {
                    in_quotes = true;
                    quote_char = current_char;
                    consume();
                    continue;
                }
            } else {
                if (current_char == quote_char) {
                    in_quotes = false;
                    consume();
                    break;
                }
            }

            word += current_char;
            consume();
        }

        return word;
    }

public:
    ShellParser(const std::string& cmd) : input(cmd) {
        consume(); // prime the first character
    }

    std::shared_ptr<Node> parse() {
        skip_whitespace();
        return parse_sequence();
    }

    std::shared_ptr<Node> parse_sequence() {
        auto seq_node = std::make_shared<SequenceNode>();
        seq_node->type = SequenceNode::Type::SEQUENTIAL;

        while (current_char != '\0' && current_char != ')') {
            auto command = parse_pipeline();
            seq_node->commands.push_back(command);

            skip_whitespace();

            // Check for sequence separators
            if (current_char == ';') {
                consume();
            } else if (current_char == '&') {
                seq_node->type = SequenceNode::Type::BACKGROUND;
                consume();
            } else {
                break;
            }

            skip_whitespace();
        }

        return seq_node;
    }

    std::shared_ptr<Node> parse_pipeline() {
        skip_whitespace();

        // Handle subshell
        if (current_char == '(') {
            consume(); // consume '('
            auto subshell = std::make_shared<SubshellNode>();
            subshell->command = parse_sequence();

            skip_whitespace();
            if (current_char != ')') {
                throw std::runtime_error("Expected closing parenthesis");
            }
            consume(); // consume ')'
            return subshell;
        }

        auto pipeline = std::make_shared<PipelineNode>();

        while (current_char != '\0') {
            auto cmd = parse_command();
            pipeline->commands.push_back(cmd);

            skip_whitespace();
            if (current_char != '|') break;

            consume(); // consume '|'
            skip_whitespace();
        }

        return pipeline->commands.size() == 1 ?
            std::static_pointer_cast<Node>(pipeline->commands[0]) :
            std::static_pointer_cast<Node>(pipeline);
    }

    std::shared_ptr<CommandNode> parse_command() {
        auto cmd_node = std::make_shared<CommandNode>();

        skip_whitespace();

        // Parse command and arguments
        if (current_char != '\0' && current_char != '|' &&
            current_char != ';' && current_char != '&') {
            cmd_node->command = parse_word();

            // Parse arguments
            while (current_char != '\0') {
                skip_whitespace();

                // Check for redirections before adding argument
                if (current_char == '<') {
                    consume(); // consume '<'
                    skip_whitespace();
                    cmd_node->input_redirect = parse_word();
                } else if (current_char == '>') {
                    consume(); // consume '>'

                    // Check for append mode
                    if (current_char == '>') {
                        consume();
                        cmd_node->append_output = true;
                    }

                    skip_whitespace();
                    cmd_node->output_redirect = parse_word();
                } else if (std::isspace(current_char) ||
                           current_char == '|' ||
                           current_char == ';' ||
                           current_char == '&' ||
                           current_char == ')') {
                    break;
                } else {
                    cmd_node->args.push_back(parse_word());
                }
            }
        }

        return cmd_node;
    }
};

#if 0 // version 2 (claude)
    class ShellParser {
private:
    std::istringstream input;
    char current_char = ' ';

    void consume() {
        input.get(current_char);
        if (input.eof()) current_char = '\0';
    }

    void skip_whitespace() {
        while (std::isspace(current_char)) consume();
    }

    std::string parse_quoted_word() {
        std::string word;
        bool in_single_quote = false;
        bool in_double_quote = false;
        bool escaped = false;

        while (current_char != '\0') {
            if (escaped) {
                // Handle escaped characters inside double quotes
                if (in_double_quote) {
                    // Specific escape sequences in double quotes
                    if (current_char == '$' ||
                        current_char == '`' ||
                        current_char == '"' ||
                        current_char == '\\' ||
                        current_char == '\n') {
                        word += current_char;
                    } else {
                        // Preserve the backslash and character
                        word += '\\';
                        word += current_char;
                    }
                    escaped = false;
                    consume();
                    continue;
                }
                // In single quotes, treat backslash as literal
                else if (in_single_quote) {
                    word += current_char;
                    escaped = false;
                    consume();
                    continue;
                }
            }

            // Handle escape character
            if (current_char == '\\' && !escaped) {
                if (in_single_quote) {
                    // In single quotes, backslash is literal
                    word += current_char;
                } else {
                    // In double quotes or unquoted, backslash escapes next char
                    escaped = true;
                }
                consume();
                continue;
            }

            // Handle quote transitions
            if (!in_single_quote && !in_double_quote) {
                if (current_char == '\'') {
                    in_single_quote = true;
                    consume();
                    continue;
                }
                if (current_char == '"') {
                    in_double_quote = true;
                    consume();
                    continue;
                }
            }

            // Handle quote endings
            if (in_single_quote && current_char == '\'') {
                in_single_quote = false;
                consume();
                break;
            }

            if (in_double_quote && current_char == '"' && !escaped) {
                in_double_quote = false;
                consume();
                break;
            }

            // Add character if in a quote context or unquoted
            if (in_single_quote || in_double_quote ||
                (!std::isspace(current_char) &&
                 current_char != '|' &&
                 current_char != ';' &&
                 current_char != '&' &&
                 current_char != '(' &&
                 current_char != ')' &&
                 current_char != '<' &&
                 current_char != '>')) {
                word += current_char;
            } else if (!in_single_quote && !in_double_quote) {
                break;
            }

            consume();
        }

        // Error checking for unclosed quotes
        if (in_single_quote) {
            throw std::runtime_error("Unclosed single quote");
        }
        if (in_double_quote) {
            throw std::runtime_error("Unclosed double quote");
        }

        return word;
    }

    std::string parse_word() {
        skip_whitespace();

        // If we encounter a quote, use quoted parsing
        if (current_char == '\'' || current_char == '"' || current_char == '\\') {
            return parse_quoted_word();
        }

        // Otherwise, parse an unquoted word
        std::string word;
        while (current_char != '\0') {
            if (std::isspace(current_char) ||
                current_char == '|' ||
                current_char == ';' ||
                current_char == '&' ||
                current_char == '(' ||
                current_char == ')' ||
                current_char == '<' ||
                current_char == '>') {
                break;
            }
            word += current_char;
            consume();
        }
        return word;
    }

public:
    // [Rest of the parser implementation remains the same]
    ShellParser(const std::string& cmd) : input(cmd) {
        consume(); // prime the first character
    }

    // [Other methods from previous implementation]
};

// Main function with extended test cases
int main() {
    std::vector<std::string> test_commands = {
        "ls -l",
        "echo 'Hello, world!'",
        "echo \"Hello, world!\"",
        "echo \"Escaped \\\" quote\"",
        "echo 'Single quoted string with spaces'",
        "echo \"Special chars: $HOME `date`\"",
        "grep 'pattern with spaces' file.txt",
        "echo \"Backslash at end\\\""
    };

    for (const auto& cmd : test_commands) {
        try {
            ShellParser parser(cmd);
            auto ast = parser.parse();

            std::cout << "Parsing command: " << cmd << std::endl;
            ast->print(std::cout);
            std::cout << "\n---\n";
        } catch (const std::exception& e) {
            std::cerr << "Error parsing '" << cmd << "': " << e.what() << std::endl;
        }
    }

    return 0;
}

#endif


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
