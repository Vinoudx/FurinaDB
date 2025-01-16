#ifndef _FURINADB_SQLMANAGER_
#define _FURINADB_SQLMANAGER_

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cctype>
#include <unordered_map>
#include <optional>
#include <stack>

#include "utils.h"


enum TokenType {
    LEFT_PAREN = 2,
    RIGHT_PAREN = 3,
    OR = 4,
    AND = 5,
    NOT = 6,

    INTO,
    USE,
    CREATE,
    DATABASE,
    DATABASES,
    TABLE,
    TABLES,
    DROP,
    SHOW,
    DESC,
    RENAME,
    EDIT,
    COL,
    ADD,
    INSERT,
    UPDATE,
    DELETE,
    SELECT,
    FROM,
    WHERE,
    IDENTIFIER,
    STRING_LITERAL,
    COMMA,
    SEMICOLON,
    KEYWORD,
    UNKNOWN,
    END,
    VALUES,
    ASTERISK,
    SET,
    JOIN,
    ON,
    PASSWORDD,
    ALTER,

    EQUALS,
    GREATER,
    LESS,
    INEQUAL
};

struct Token {
    TokenType type;
    std::string value;
};

class Lexer {
public:

    Lexer() = default;
    Lexer(const std::string& query) :query(query), current_pos(0) {}

    Token nextToken();

private:
    std::string query;
    size_t current_pos;
    std::unordered_map<std::string, TokenType> keywords = {
        {"USE", TokenType::USE},
        {"CREATE", TokenType::CREATE},
        {"DATABASE", TokenType::DATABASE},
        {"DATABASES", TokenType::DATABASES},
        {"TABLE", TokenType::TABLE},
        {"TABLES", TokenType::TABLES},
        {"DROP", TokenType::DROP},
        {"SHOW", TokenType::SHOW},
        {"DESC", TokenType::DESC},
        {"EDIT", TokenType::EDIT},
        {"ADD", TokenType::ADD},
        {"OR", TokenType::OR},
        {"INTO", TokenType::INTO},
        {"SET", TokenType::SET},
        {"AND", TokenType::AND},
        {"NOT", TokenType::NOT},
        {"COL", TokenType::COL},
        {"ALTER", TokenType::ALTER},
        {"PASSWORD", TokenType::PASSWORDD},
        {"VALUES", TokenType::VALUES},
        {"RENAME", TokenType::RENAME},
        {"INSERT", TokenType::INSERT},
        {"UPDATE", TokenType::UPDATE},
        {"DELETE", TokenType::DELETE},
        {"SELECT", TokenType::SELECT},
        {"JOIN", TokenType::JOIN},
        {"ON", TokenType::ON},
        {"FROM", TokenType::FROM},
        {"WHERE", TokenType::WHERE}
    };


};

class SQLManager {
public:


    SQLManager() :tokens({}) {};
    std::optional<std::vector<Token>> compile(const std::string& sql);
    

private:
    Lexer lexer;
    std::vector<Token> tokens;
};

#endif 
