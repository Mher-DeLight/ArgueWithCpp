#pragma once
#include "Common.h"
#include "Tokenizer.h"
#include <map>
#include <memory>
#include <vector>

class Parser {
private:
    std::string buzzword_garbage = "kfkbkfhfausgnipcxuimhfdwjqigi"; // buzzword flags are saved here
    std::vector<Token> tokens;

    int cursor = 0;

    Token& peek(int offset = 0);
    Token& previous(int offset = 0);
    Token eat(TokenType type, const std::string& msg = "");
    bool isEnd();
    bool match(TokenType type);
    bool check(TokenType type);
    void advance(int offset = 1);
    void parserPanic(const std::string& msg, const SourceLocation& location = SourceLocation());

    std::unique_ptr<Object_Block> parseScope(bool require_brackets);
    std::unique_ptr<Object_Block> parseArgfile();
    std::unique_ptr<Object> parseStatement();
    std::unique_ptr<Object_Flag> parseFlag();
    std::unique_ptr<Object_Flag> parseBuzzword();

public:
    std::unique_ptr<Object_Block> entry_point = std::make_unique<Object_Block>();
    void load_tokens(std::vector<Token> tokens_);
    void parse();
    void print_tree();
};
