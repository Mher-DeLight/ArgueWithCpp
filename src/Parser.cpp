#include "../include/Parser.h"
#include "../include/Common.h"
#include "../include/ErrorHandler.h"
#include "Tokenizer.h"
#include <iostream>

void Parser::load_tokens(std::vector<Token> tkns) {
    tokens = std::move(tkns);
}
void Parser::parse() {
    cursor = 0;
    entry_point = parseScope(false);
}
std::unique_ptr<Object_Block> Parser::parseScope(bool require_brackets) {
    std::unique_ptr<Object_Block> scope = std::make_unique<Object_Block>();

    if (require_brackets)
        eat(TokenType::LBrace, "expected '{' on scope entry");

    while (!isEnd()) {
        if (check(TokenType::Semicolon)) {
            advance();
            continue;
        }
        auto stmt = parseStatement();
        if (stmt == nullptr) {
            panic("invalid statement at " + std::to_string(peek().location.row) + ":" +
                  std::to_string(peek().location.column));
        } else {
            scope->children.push_back(std::move(stmt));
        }

        if (match(TokenType::EndOfFile))
            break;
        if (require_brackets && match(TokenType::RBrace))
            break;
    }
    return std::move(scope);
}

// == PARSE FUNCTIONS ==
std::unique_ptr<Object> Parser::parseStatement() {
    if (check(TokenType::ObjwordFlag))
        return parseFlag();
    else
        return nullptr;
}
std::unique_ptr<Object_Flag> Parser::parseFlag() {
    eat(TokenType::ObjwordFlag, "expected objword 'flag' for flag declaration");

    std::string variable_name =
        eat(TokenType::VariableIdentifier, "expected variable identifier after 'flag' keyword")
            .lexeme;

    std::string flag_text =
        eat(TokenType::StringLiteral, "expected flag text after variable name in 'flag' statement")
            .lexeme;

    bool is_required = false;
    bool is_ordered = true;

    while (!check(TokenType::LBrace) && !check(TokenType::Semicolon)) {

        if (check(TokenType::KeywordRequired)) {
            is_required = true;
        } else if (check(TokenType::KeywordNotRequired)) {
            is_required = false;
        } else if (check(TokenType::KeywordUnordered)) {
            is_ordered = false;
        } else {
            parserPanic("unexpected token '" + peek().lexeme + "' in flag declaration",
                        peek().location);
        }

        advance();
    }

    std::unique_ptr<Object_Block> block = nullptr;

    if (check(TokenType::LBrace)) {
        block = parseScope(true);
        block->is_ordered = is_ordered;
    }

    eat(TokenType::Semicolon, "expected semicolon after flag statement");

    return std::make_unique<Object_Flag>(variable_name, flag_text, is_required, std::move(block));
}

// == HELPERS ==
Token& Parser::peek(int offset) {
    if (cursor + offset > tokens.size()) {
        parserPanic("cannot peek into token number " + std::to_string(cursor + offset) +
                    "; such token does not exist.");
    }
    return tokens[cursor + offset];
}
Token& Parser::previous(int offset) {
    if (cursor - offset > tokens.size()) {
        parserPanic("cannot peek into previous token number " + std::to_string(cursor - offset) +
                    "; such token does not exist.");
    }
    return tokens[cursor - offset];
}
bool Parser::isEnd() {
    return check(TokenType::EndOfFile);
}
bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    } else {
        return false;
    }
}
bool Parser::check(TokenType type) {
    if (peek().type == type) {
        return true;
    }
    return false;
}
void Parser::advance(int offset) {
    if ((cursor + offset > tokens.size()) && offset > 0) {
        parserPanic("cannot advance to position " + std::to_string(cursor + offset) +
                    "; position is out of bounds.");
    }
    if ((cursor + offset < 0) && offset < 0) {
        parserPanic("cannot advance to position " + std::to_string(cursor + offset) +
                    "; position is out of bounds.");
    }
    cursor += offset;
}
Token Parser::eat(TokenType type, const std::string& msg) {
    if (!check(type))
        parserPanic(msg, peek().location);

    Token t = peek();
    advance();
    return t;
}
void Parser::parserPanic(const std::string& msg, const SourceLocation& src) {
    panic("[PARSER PANIC] " + msg + " [AT " + std::to_string(src.row) + ":" +
          std::to_string(src.column) + "]");
}
void Parser::print_tree() {
    entry_point->print(std::cout);
}