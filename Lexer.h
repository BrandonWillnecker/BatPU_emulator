#pragma once
#include <string>

#include "numeric_parsing.h"

enum class TOKEN_TYPE {
    KEYWORD, OPERATOR, LITERAL, IDENTIFIER
};

struct TOKEN {
    std::string value;
    TOKEN_TYPE type;

    TOKEN(std::string value, TOKEN_TYPE type) :
        value(value), type(type)
    {
    }
};

std::string token_type_to_str(TOKEN_TYPE type) {
    switch (type)
    {
    case TOKEN_TYPE::KEYWORD: return "KEYWORD";
    case TOKEN_TYPE::OPERATOR: return "OPERATOR";
    case TOKEN_TYPE::LITERAL: return "LITERAL";
    case TOKEN_TYPE::IDENTIFIER: return "IDENTIFIER";
    default: return "UNKNOWN_TOKEN_TYPE";
    }
}

void debug_print_tokens(const std::vector<TOKEN>& tokens) {
    std::cout << "[";
    for (const TOKEN& token : tokens) std::cout << token_type_to_str(token.type) << " " << token.value << ", ";
    std::cout << "]\n";
}

void tokenize(const std::string& filename, std::vector<TOKEN>& tokens_vec) {
    //removal of comments
    //expansion of macros
    //expansion of included files

    std::ifstream source_file(filename);
    std::string source_code;
    char c;
    while (source_file.get(c)) source_code += c;

    std::unordered_map<std::string, std::string> preprocessor_defines = { {"true","1"},{"false","0"} };

    //Evaluate all #includes and save all defines
    for (size_t index = 0; index < source_code.length(); index++) {
        if (source_code[index] == '#') {
            //preprocessor macros
            std::string macro_type;
            while (source_code[index] != ' ') {
                macro_type += source_code[index];
                index++;
            }
            if (macro_type == "#include") {
                size_t start_index = index - macro_type.length();
                std::string include_filename;
                index++;
                while (source_code[index] != '\n' && index < source_code.length()) {
                    include_filename += source_code[index];
                    index++;
                }
                std::string include_source_code;
                std::ifstream include_file(include_filename.substr(1, include_filename.length() - 2));
                char c;
                while (include_file.get(c)) {
                    include_source_code += c;
                }
                source_code = source_code.erase(start_index, index - start_index);
                source_code.insert(start_index, include_source_code);
                index = start_index - 1;
            }
            else if (macro_type == "#define") {
                size_t start_index = index - macro_type.length();
                std::string define_name;
                index++;
                while (source_code[index] != ' ') {
                    define_name += source_code[index];
                    index++;
                }
                index++;
                std::string define_value;
                while (source_code[index] != '\n' && index < source_code.length()) {
                    define_value += source_code[index];
                    index++;
                }
                index++;
                preprocessor_defines[define_name] = define_value;
                source_code = source_code.erase(start_index, index - start_index);
                index = start_index - 1;
            }
            else if (macro_type == "#undef") {
                std::string define_name;
                index++;
                while (source_code[index] != ' ') {
                    define_name += source_code[index];
                    index++;
                }
                index++;
                preprocessor_defines[define_name] = "";
            }
        }
    }

    //Remove all comments and extra spacing
    std::string source_code_no_comments;
    for (size_t index = 0; index < source_code.length(); index++) {
        if (source_code[index] == '/' && source_code[index + 1] == '/') {
            //comment, skip to newline char
            while (source_code[index] != '\n' && index < source_code.length()) index++;
            index--;
        }
        else if (source_code[index] == '"') {
            //string, skip to end of string
            source_code_no_comments += source_code[index];
            index++;
            while (true) {
                if (source_code[index] == '"' && source_code[index - 1] != '\\') {
                    break;
                }
                source_code_no_comments += source_code[index];
                index++;
            }
            source_code_no_comments += source_code[index];
        }
        else if (source_code[index] == ' ' || source_code[index] == '\t') {
            //extra spaces or tabs
            source_code_no_comments += ' ';
            while (source_code[index] == ' ' || source_code[index] == '\t') index++;
            index--;
        }
        else {
            source_code_no_comments += source_code[index];
        }
    }
    source_code = source_code_no_comments;

    //Remove all blank lines, leading space and trailing space
    std::string source_code_no_space;
    for (size_t index = 0; index < source_code.length(); index++) {
        //get line
        std::string line;
        while (source_code[index] != '\n' && index < source_code.length()) {
            line += source_code[index];
            index++;
        }
        if (line != "") {
            //remove all leading and trailing space
            size_t start_index = 0;
            size_t end_index = line.length() - 1;
            while (line[start_index] == ' ' || line[start_index] == '\t') start_index++;
            while (end_index > 0 && (line[end_index] == ' ' || line[end_index] == '\t')) end_index--;
            source_code_no_space += line.substr(start_index, end_index - start_index + 1);
        }
    }
    source_code = source_code_no_space;

    //Tokenize
    std::unordered_set<std::string> keywords = { "break","bool","char","continue","do","else","float","for","goto","if","int","long","return","short","signed","unsigned","void","while" };
    std::unordered_set<std::string> operators = { "+","-","*","/","%","++","--","=","+=","-=","*=","/=","%=","==",">","<","!=",">=","<=","&&","||","!","&","|","^","~","<<",">>",",",".","(",")","{","}","[","]",";" };

    std::string token_value;
    std::vector<std::string> tokens;

    for (size_t index = 0; index < source_code.length(); index++) {
        if (source_code[index] == ' ') {
            //save current token
            if (token_value != "") {
                tokens.push_back(token_value);
                token_value = "";
            }
        }
        else if (operators.contains(std::string(1, source_code[index]))) {
            if (operators.contains(source_code.substr(index, 2))) {
                //double char operator
                if (token_value != "") {
                    tokens.push_back(token_value);
                    token_value = "";
                }
                tokens.push_back(source_code.substr(index, 2));
                index++;
            }
            else {
                //single char operator
                if (token_value != "") {
                    tokens.push_back(token_value);
                    token_value = "";
                }
                tokens.push_back(std::string(1, source_code[index]));
            }
        }
        else if (source_code[index] == '"') {
            //get full string
            token_value += source_code[index];
            index++;
            while (true) {
                if (source_code[index] == '"' && source_code[index - 1] != '\\')
                    break;
                token_value += source_code[index];
                index++;
            }
            token_value += source_code[index];
            if (token_value != "") {
                tokens.push_back(token_value);
                token_value = "";
            }
        }
        else {
            //just add char to current token
            token_value += source_code[index];
        }
    }

    //set the tokenizer types
    tokens_vec.reserve(tokens.size());

    for (std::string& token : tokens) {
        if (keywords.contains(token)) {
            tokens_vec.emplace_back(token, TOKEN_TYPE::KEYWORD);
        }
        else if (operators.contains(token)) {
            tokens_vec.emplace_back(token, TOKEN_TYPE::OPERATOR);
        }
        else if (preprocessor_defines.contains(token)) {
            token = preprocessor_defines[token];
            tokens_vec.emplace_back(token, TOKEN_TYPE::LITERAL);
        }
        else if (is_numeric(token) || (token.starts_with('"') && token.ends_with('"'))) {
            tokens_vec.emplace_back(token, TOKEN_TYPE::LITERAL);
        }
        else {
            tokens_vec.emplace_back(token, TOKEN_TYPE::IDENTIFIER);
        }
    }

}
