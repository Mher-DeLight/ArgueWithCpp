#pragma once
#include "Common.h"
#include "Tokenizer.h"
#include <map>
#include <memory>
#include <vector>

class Parser {
private:
    std::map<std::string, std::string> vars_args;
    std::map<std::string, bool> vars_flags;

    std::vector<Token> tokens;
    std::unique_ptr<Object_Block> entry_point = std::make_unique<Object_Block>();

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
    std::unique_ptr<Object> parseStatement();

public:
    void load_tokens(std::vector<Token> tokens_);
    void parse();
};
