#include "sqlmanager.h"

Token Lexer::nextToken() {
    while (current_pos < query.size() && std::isspace(query[current_pos])) {
        current_pos++;
    }
    if (current_pos >= query.size()) return { TokenType::END, "" };
    char current_char = query[current_pos];
    if (std::isalnum(current_char)) {
        std::string identifier;
        while (current_pos < query.size() && (std::isalnum(query[current_pos]) || 
                                                query[current_pos] == '_' || 
                                                query[current_pos] == '.'  || 
            query[current_pos] == '=' || 
            query[current_pos] == '>' || 
            query[current_pos] == '<' || 
            query[current_pos] == '^'   )) {
            identifier += query[current_pos++];
        }
        auto keyword = keywords.find(identifier);
        if (keyword != keywords.end()) {
            return { keyword->second, identifier };
        }
        return { TokenType::IDENTIFIER, identifier };
    }
    if (current_char == '\'') {
        current_pos++;
        std::string str;
        while (current_pos < query.size() && query[current_pos] != '\'') {
            str += query[current_pos++];
        }
        current_pos++;
        return { TokenType::STRING_LITERAL, str };
    }
    if (current_char == ',') {
        current_pos++;
        return { TokenType::COMMA, "," };
    }
    if (current_char == ';') {
        current_pos++;
        return { TokenType::SEMICOLON, ";" };
    }
    if (current_char == '(') {
        current_pos++;
        return { TokenType::LEFT_PAREN, "(" };
    }
    if (current_char == ')') {
        current_pos++;
        return { TokenType::RIGHT_PAREN, ")" };
    }
    if (current_char == '=') {
        current_pos++;
        return { TokenType::EQUALS, "=" };
    }
    if (current_char == '*') {
        current_pos++;
        return { TokenType::ASTERISK, "*" };
    }
    if (current_char == '>') {
        current_pos++;
        return { TokenType::GREATER, ">" };
    }
    if (current_char == '<') {
        current_pos++;
        return { TokenType::LESS, "<" };
    }
    if (current_char == '^') {
        current_pos++;
        return { TokenType::INEQUAL, "^" };
    }
    return { TokenType::UNKNOWN, std::string(1, current_char) };
}

std::optional<std::vector<Token>> SQLManager::compile(const std::string& sql) {
    tokens.clear();
    lexer = Lexer(sql);
    Token temp = lexer.nextToken();
    tokens.push_back(temp);

    switch (temp.type){
    case TokenType::USE:
        temp = lexer.nextToken();
        if (temp.type != TokenType::IDENTIFIER) {
            return std::nullopt;
        }

        tokens.push_back(temp);
        temp = lexer.nextToken();
        if (temp.type != TokenType::SEMICOLON) {
            return std::nullopt;
        }
        return tokens;
        break;
    case TokenType::CREATE:

        temp = lexer.nextToken();
        if (temp.type == TokenType::DATABASE) {
            tokens.push_back(temp);
            temp = lexer.nextToken();
            if (temp.type != TokenType::IDENTIFIER) {
                return std::nullopt;
            }
            tokens.push_back(temp);
            temp = lexer.nextToken();
            if (temp.type != TokenType::SEMICOLON) {
                return std::nullopt;
            }
            return tokens;
        }
        else if (temp.type == TokenType::TABLE) {
            tokens.push_back(temp);
            temp = lexer.nextToken();
            if (temp.type != TokenType::IDENTIFIER) {
                return std::nullopt;
            }
            tokens.push_back(temp);
            temp = lexer.nextToken();
            if (temp.type != TokenType::LEFT_PAREN) {
                return std::nullopt;
            }
            temp = lexer.nextToken();
            bool hasValue = false;
            while (temp.type != TokenType::RIGHT_PAREN) {
                hasValue = true;
                for (int i = 0; i < 5; i++) {
                    if (temp.type != TokenType::STRING_LITERAL && i == 0) {
                        return std::nullopt;
                    }
                    if (temp.type != TokenType::IDENTIFIER && i != 0) {
                        return std::nullopt;
                    }
                    if (i == 1 && temp.value.compare("char") && temp.value.compare("int") && temp.value.compare("double")) {
                        return std::nullopt;
                    }
                    if (i > 2 && temp.value != "0" && temp.value != "1") {
                        return std::nullopt;
                    }
                    tokens.push_back(temp);
                    temp = lexer.nextToken();
                }
                if (temp.type == TokenType::COMMA) {
                    temp = lexer.nextToken();
                }
            }
            tokens.push_back(temp);
            temp = lexer.nextToken();
            if (temp.type != TokenType::SEMICOLON) {
                return std::nullopt;
            }
            return tokens;
        }
        else {
            return std::nullopt;
        }


        break;
    case TokenType::DROP:
        temp = lexer.nextToken();
        if (temp.type != TokenType::TABLE && temp.type != TokenType::DATABASE) {
            return std::nullopt;
        }
        tokens.push_back(temp);
        temp = lexer.nextToken();
        if (temp.type != TokenType::IDENTIFIER) {
            return std::nullopt;
        }
        tokens.push_back(temp);
        temp = lexer.nextToken();
        if (temp.type != TokenType::SEMICOLON) {
            return std::nullopt;
        }
        return tokens;

        break;
    case TokenType::SHOW:
        temp = lexer.nextToken();
        if (temp.type != TokenType::TABLES && temp.type != TokenType::DATABASES) {
            return std::nullopt;
        }
        tokens.push_back(temp);
        temp = lexer.nextToken();
        if (temp.type != TokenType::SEMICOLON) {
            return std::nullopt;
        }
        return tokens;
        break;
    case TokenType::DESC:
        temp = lexer.nextToken();
        if (temp.type != TokenType::IDENTIFIER) {
            return std::nullopt;
        }
        tokens.push_back(temp);
        temp = lexer.nextToken();
        if (temp.type != TokenType::SEMICOLON) {
            return std::nullopt;
        }
        return tokens;
        break;
    case TokenType::RENAME:
        temp = lexer.nextToken();
        if (temp.type != TokenType::TABLE) {
            return std::nullopt;
        }
        tokens.push_back(temp);
        temp = lexer.nextToken();
        if (temp.type != TokenType::IDENTIFIER) {
            return std::nullopt;
        }
        tokens.push_back(temp);
        temp = lexer.nextToken();
        if (temp.type != TokenType::IDENTIFIER) {
            return std::nullopt;
        }
        tokens.push_back(temp);
        temp = lexer.nextToken();
        if (temp.type != TokenType::SEMICOLON) {
            return std::nullopt;
        }
        return tokens;
        break;
    case TokenType::EDIT:
        temp = lexer.nextToken();
        if (temp.type != TokenType::TABLE) {
            return std::nullopt;
        }
        temp = lexer.nextToken();
        if (temp.type != TokenType::IDENTIFIER) {
            return std::nullopt;
        }
        tokens.push_back(temp);
        temp = lexer.nextToken();
        if (temp.type != TokenType::LEFT_PAREN) {
            return std::nullopt;
        }
        for (int i = 0; i < 5; i++) {
            temp = lexer.nextToken();
            if (temp.type != TokenType::IDENTIFIER) {
                return std::nullopt;
            }
            tokens.push_back(temp);
        }
        temp = lexer.nextToken();
        if (temp.type != TokenType::RIGHT_PAREN) {
            return std::nullopt;
        }
        temp = lexer.nextToken();
        if (temp.type != TokenType::IDENTIFIER) {
            return std::nullopt;
        }
        tokens.push_back(temp);
        temp = lexer.nextToken();
        if (temp.type != TokenType::SEMICOLON) {
            return std::nullopt;
        }
        return tokens;
        break;
    case TokenType::ADD:
        temp = lexer.nextToken();
        if (temp.type != TokenType::COL) {
            return std::nullopt;
        }
        temp = lexer.nextToken();
        if (temp.type != TokenType::IDENTIFIER) {
            return std::nullopt;
        }
        tokens.push_back(temp);
        temp = lexer.nextToken();
        if (temp.type != TokenType::LEFT_PAREN) {
            return std::nullopt;
        }
        for (int i = 0; i < 5; i++) {
            temp = lexer.nextToken();
            if (temp.type != TokenType::IDENTIFIER) {
                return std::nullopt;
            }
            tokens.push_back(temp);
        }
        temp = lexer.nextToken();
        if (temp.type != TokenType::RIGHT_PAREN) {
            return std::nullopt;
        }
        temp = lexer.nextToken();
        if (temp.type != TokenType::SEMICOLON) {
            return std::nullopt;
        }
        return tokens;
        break;
    case TokenType::INSERT:
        temp = lexer.nextToken();
        if (temp.type != TokenType::INTO) {
            return std::nullopt;
        }
        temp = lexer.nextToken();
        if (temp.type != TokenType::IDENTIFIER) {
            return std::nullopt;
        }
        tokens.push_back(temp);
        temp = lexer.nextToken();
        if (temp.type != TokenType::LEFT_PAREN) {
            return std::nullopt;
        }
        temp = lexer.nextToken();
        while(temp.type != TokenType::RIGHT_PAREN) {
            if (temp.type == TokenType::VALUES) {
                return std::nullopt;
            }
            if (temp.type != TokenType::IDENTIFIER) {
                return std::nullopt;
            }
            tokens.push_back(temp);
            temp = lexer.nextToken();
        }
        if (temp.type != TokenType::RIGHT_PAREN) {
            return std::nullopt;
        }
        temp = lexer.nextToken();
        if (temp.type != TokenType::VALUES) {
            return std::nullopt;
        }
        tokens.push_back(temp);
        temp = lexer.nextToken();
        if (temp.type != TokenType::LEFT_PAREN) {
            return std::nullopt;
        }
        temp = lexer.nextToken();
        while (temp.type != TokenType::RIGHT_PAREN) {
            if (temp.type == TokenType::SEMICOLON) {
                return std::nullopt;
            }
            if (temp.type != TokenType::IDENTIFIER) {
                return std::nullopt;
            }
            tokens.push_back(temp);
            temp = lexer.nextToken();
        }
        if (temp.type != TokenType::RIGHT_PAREN) {
            return std::nullopt;
        }
        temp = lexer.nextToken();

        if (temp.type != TokenType::SEMICOLON) {
            return std::nullopt;
        }
        tokens.push_back(temp);
        return tokens;




        break;
    case TokenType::UPDATE:
        temp = lexer.nextToken();
        while (temp.type != END) {
            tokens.push_back(temp);
            temp = lexer.nextToken();
        }
        return tokens;
        break;
    case TokenType::DELETE:
        temp = lexer.nextToken();
        if (temp.type == TokenType::COL) {
            tokens.push_back(temp);
            temp = lexer.nextToken();
            if (temp.type != TokenType::IDENTIFIER) {
                return std::nullopt;
            }
            tokens.push_back(temp);
            temp = lexer.nextToken();
            if (temp.type != TokenType::IDENTIFIER) {
                return std::nullopt;
            }
            tokens.push_back(temp);
            temp = lexer.nextToken();
            if (temp.type != TokenType::SEMICOLON) {
                return std::nullopt;
            }
            return tokens;
        }
        else if (temp.type == TokenType::FROM) {
            tokens.push_back(temp);
            temp = lexer.nextToken();
            if (temp.type != TokenType::IDENTIFIER) {
                return std::nullopt;
            }
            tokens.push_back(temp);
            temp = lexer.nextToken();
            if (temp.type != TokenType::WHERE) {
                return std::nullopt;
            }
            temp = lexer.nextToken();
            while (temp.type != END) {
                tokens.push_back(temp);
                temp = lexer.nextToken();
            }
            return tokens;
        }
        else {
            return std::nullopt;
        }
        break;
    case TokenType::SELECT:
        temp = lexer.nextToken();
        while(temp.type != END) {
            tokens.push_back(temp);
            temp = lexer.nextToken();
        }
        return tokens;
    case TokenType::ALTER:
        temp = lexer.nextToken();
        if (temp.type != TokenType::PASSWORDD) {
            return std::nullopt;
        }
        temp = lexer.nextToken();
        if (temp.type != TokenType::IDENTIFIER) {
            return std::nullopt;
        }
        tokens.push_back(temp);
        temp = lexer.nextToken();
        if (temp.type != TokenType::IDENTIFIER) {
            return std::nullopt;
        }
        tokens.push_back(temp);
        temp = lexer.nextToken();
        if (temp.type != TokenType::IDENTIFIER) {
            return std::nullopt;
        }
        tokens.push_back(temp);
        temp = lexer.nextToken();
        if (temp.type != TokenType::SEMICOLON) {
            return std::nullopt;
        }
        return tokens;
        break;
    default:
        return std::nullopt;
        break;
    }

}
