#pragma once
#include <string>

enum class TOKEN_TYPE {
    KEYWORD, OPERATOR, LITERAL, IDENTIFIER
};

static std::string token_type_to_str(TOKEN_TYPE type) {
    switch (type)
    {
    case TOKEN_TYPE::KEYWORD: return "KEYWORD";
    case TOKEN_TYPE::OPERATOR: return "OPERATOR";
    case TOKEN_TYPE::LITERAL: return "LITERAL";
    case TOKEN_TYPE::IDENTIFIER: return "IDENTIFIER";
    default: return "UNKNOWN_TOKEN_TYPE";
    }
}

struct TOKEN {
    std::string value;
    TOKEN_TYPE type;

    TOKEN(std::string value, TOKEN_TYPE type) :
        value(value), type(type)
    {
    }
};
