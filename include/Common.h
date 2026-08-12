#pragma once
#include <array>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

enum class TokenType {
    None,

    VariableIdentifier,
    StringLiteral,

    Plus,
    Minus,
    Star,
    Slash,

    LParen,
    RParen,
    LBrace,
    RBrace,

    Comma,
    Period,

    Colon,
    Semicolon,
    Ampersand,
    Exclamation,
    Hashtag,
    Money,

    EndOfFile
};
class SourceLocation {
public:
    int row = -1;
    int column = -1;

    SourceLocation(int r, int c) : row(r), column(c) {}
    SourceLocation() = default;
};
const std::unordered_map<std::string, TokenType> word_table{
    {{"__END_OF_FILE__", TokenType::EndOfFile},

     {"+", TokenType::Plus},
     {"-", TokenType::Minus},
     {"*", TokenType::Star},
     {"/", TokenType::Slash},

     {"(", TokenType::LParen},
     {")", TokenType::RParen},
     {"{", TokenType::LBrace},
     {"}", TokenType::RBrace},
     {"&", TokenType::Ampersand},
     {":", TokenType::Colon},
     {";", TokenType::Semicolon},
     {"!", TokenType::Exclamation},
     {",", TokenType::Comma},
     {"#", TokenType::Hashtag},
     {".", TokenType::Period},
     {",", TokenType::Comma},
     {"$", TokenType::Money}}};

class Object {
public:
    virtual ~Object() = default;
};
class Object_Block : public Object {
public:
    std::vector<std::unique_ptr<Object>> children;

    Object_Block(std::vector<std::unique_ptr<Object>> children_) : children(std::move(children_)) {}
    Object_Block() = default;
};