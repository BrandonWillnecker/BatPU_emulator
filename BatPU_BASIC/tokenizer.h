#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <format>
#include <regex>

enum class token_t
{
	UNKNOWN,

	//Keywords
	KEYWORD_CLEAR,
	KEYWORD_DATA,
	KEYWORD_DEF,
	KEYWORD_DIM,
	KEYWORD_END,
	KEYWORD_ERASE,
	KEYWORD_ERR,
	KEYWORD_ERROR,
	KEYWORD_FOR,
	KEYWORD_TO,
	KEYWORD_STEP,
	KEYWORD_NEXT,
	KEYWORD_GOSUB,
	KEYWORD_GOTO,
	KEYWORD_IF,
	KEYWORD_THEN,
	KEYWORD_ELSE,
	KEYWORD_INPUT,
	KEYWORD_MID,
	KEYWORD_ON,
	KEYWORD_POKE,
	KEYWORD_PEEK,
	KEYWORD_PRINT,
	KEYWORD_RANDOMIZE,
	KEYWORD_READ,
	KEYWORD_REM,
	KEYWORD_RESTORE,
	KEYWORD_RESUME,
	KEYWORD_RETURN,
	KEYWORD_STOP,
	KEYWORD_SWAP,
	KEYWORD_WHILE,
	KEYWORD_WEND,

	//Operators
	OPERATOR_EXP,
	OPERATOR_NEG,
	OPERATOR_POS,
	OPERATOR_MUL,
	OPERATOR_FLT_DIV,
	OPERATOR_ADD,
	OPERATOR_SUB,
	OPERATOR_INT_DIV,
	OPERATOR_MOD,
	OPERATOR_EQU,
	OPERATOR_NEQ,
	OPERATOR_LSS,
	OPERATOR_GTR,
	OPERATOR_LEQ,
	OPERATOR_GEQ,
	OPERATOR_NOT,
	OPERATOR_AND,
	OPERATOR_OR,
	OPERATOR_XOR,
	OPERATOR_EQV,
	OPERATOR_IMP,
	OPERATOR_ASSIGN,
	
	//Literals
	LITERAL_STR,
	LITERAL_INT,
	LITERAL_FLT,
	LITERAL_DBL,
	
	//Other tokens
	IDENTIFIER,
	COMMA,
	COLON,
	SEMICOLON,
	OPEN_PAREN,
	CLOSE_PAREN,
	NEWLINE,
	END_OF_FILE,
};

struct token {
	token_t type = token_t::UNKNOWN;
	std::string value = "";
	uint32_t line_number = 0;

	token() {}

	token(token_t type) :
		type(type)
	{}

	token(token_t type, std::string value) :
		type(type), value(value)
	{}

};

static const std::unordered_map<std::string, token_t> token_type_map = {
{"CLEAR",token_t::KEYWORD_CLEAR},
{"DATA",token_t::KEYWORD_DATA},
{"DEF",token_t::KEYWORD_DEF},
{"DIM",token_t::KEYWORD_DIM},
{"END",token_t::KEYWORD_END},
{"ERASE",token_t::KEYWORD_ERASE},
{"ERR",token_t::KEYWORD_ERR},
{"ERROR",token_t::KEYWORD_ERROR},
{"FOR",token_t::KEYWORD_FOR},
{"TO",token_t::KEYWORD_TO},
{"STEP",token_t::KEYWORD_STEP},
{"NEXT",token_t::KEYWORD_NEXT},
{"GOSUB",token_t::KEYWORD_GOSUB},
{"GOTO",token_t::KEYWORD_GOTO},
{"IF",token_t::KEYWORD_IF},
{"THEN",token_t::KEYWORD_THEN},
{"ELSE",token_t::KEYWORD_ELSE},
{"INPUT",token_t::KEYWORD_INPUT},
{"MID",token_t::KEYWORD_MID},
{"ON",token_t::KEYWORD_ON},
{"POKE",token_t::KEYWORD_POKE},
{"PEEK",token_t::KEYWORD_PEEK},
{"PRINT",token_t::KEYWORD_PRINT},
{"RANDOMIZE",token_t::KEYWORD_RANDOMIZE},
{"READ",token_t::KEYWORD_READ},
{"REM",token_t::KEYWORD_REM},
{"RESTORE",token_t::KEYWORD_RESTORE},
{"RESUME",token_t::KEYWORD_RESUME},
{"RETURN",token_t::KEYWORD_RETURN},
{"STOP",token_t::KEYWORD_STOP},
{"SWAP",token_t::KEYWORD_SWAP},
{"WHILE",token_t::KEYWORD_WHILE},
{"WEND",token_t::KEYWORD_WEND},

{"^",token_t::OPERATOR_EXP},
{"-",token_t::OPERATOR_NEG},
{"+",token_t::OPERATOR_POS},
{"*",token_t::OPERATOR_MUL},
{"/",token_t::OPERATOR_FLT_DIV},
{"\\",token_t::OPERATOR_INT_DIV},
{"MOD",token_t::OPERATOR_MOD},

{"==",token_t::OPERATOR_EQU},
{"<>",token_t::OPERATOR_NEQ},
{">",token_t::OPERATOR_GTR},
{"<",token_t::OPERATOR_LSS},
{">=",token_t::OPERATOR_GEQ},
{"<=",token_t::OPERATOR_LEQ},

{"AND",token_t::OPERATOR_AND},
{"OR",token_t::OPERATOR_OR},
{"NOT",token_t::OPERATOR_NOT},
{"XOR",token_t::OPERATOR_XOR},
{"EQV",token_t::OPERATOR_EQV},
{"IMP",token_t::OPERATOR_IMP},

{"=",token_t::OPERATOR_ASSIGN},

{",",token_t::COMMA},
{":",token_t::COLON},
{";",token_t::SEMICOLON},
{"(",token_t::OPEN_PAREN},
{")",token_t::CLOSE_PAREN},
{"\n",token_t::NEWLINE}
};

static const std::unordered_set<char> valid_char_set = { ' ','a','b','c','d','e','f','g','h','i','j','k','l','m','n',
												         'o','p','q','r','s','t','u','v','w','x','y','z','.','!','?' };

static bool is_valid_str(const std::string& str) {
	std::regex pattern("^[a-zA-Z !?]*$");
	return std::regex_match(str, pattern);
}

static bool is_str_hex(const std::string& str) {
	if (str.empty()) return false;
	std::regex pattern("^&H[0-9A-Fa-f]+$");
	return std::regex_match(str, pattern);
}

static bool is_str_oct(const std::string& str) {
	std::regex pattern("^&O[0-7]+$");
	return std::regex_match(str, pattern);
}

static bool is_str_int(const std::string& str) {
	if (str.empty()) return false;
	if (is_str_hex(str)) return true;
	if (is_str_oct(str)) return true;
	std::regex pattern("^[-+]?[0-9]+$");
	return std::regex_match(str, pattern);
}

static bool is_str_flt(const std::string& str) {
	if (str.empty()) return false;
	std::regex pattern("^[-+]?([0-9]+(\\.[0-9]*)?|\\.[0-9]+)([eE][-+]?[0-9]+)?!?$");
	return std::regex_match(str, pattern);
}

static bool is_str_dbl(const std::string& str) {
	if (str.empty()) return false;
	std::regex pattern("^[-+]?([0-9]+(\\.[0-9]*)?|\\.[0-9]+)([eE][-+]?[0-9]+)?#?$");
	return std::regex_match(str, pattern);
}

std::vector<token> tokenize(const std::string& filename) {
	std::vector<token> tokens;

	std::ifstream file(filename + ".bas");
	std::string line;
	int line_number = 1;

	while (std::getline(file, line)) {
		if (line.empty()) continue;

		std::string token_value;

		for (size_t i = 0; i < line.size(); i++) {
			char c = line[i];
			if (line[i] == '"') {
				//read full string
				i++;
				size_t count = 0;
				while (line[i + count] != '"') count++;
				token token;
				token.line_number = line_number;
				token.type = token_t::LITERAL_STR;
				token.value = line.substr(i, count);
				tokens.push_back(token);
				i += count;
			}
			else if (std::isdigit(line[i])) {
				//read full numeric
				token numeric;
				numeric.line_number = line_number;
				std::string num_str;

				while (std::isdigit(line[i])){
					num_str += line[i];
					i++;
				} 

				if (line[i] == '%') {
					num_str += line[i];
					i++;
				}

				if (line[i] != '.') {
					//save int
					numeric.type = token_t::LITERAL_INT;
					numeric.value = num_str;
					tokens.push_back(numeric);
					i--;
					continue;
				}

				num_str += line[i];
				i++;
				while (std::isdigit(line[i])) {
					num_str += line[i];
					i++;
				}

				if (line[i] == 'E') {
					//exponential form
					num_str += line[i];
					i++;
					num_str += line[i];
					i++;
					while (std::isdigit(line[i])) {
						num_str += line[i];
						i++;
					}
				}

				if (line[i] == '#') {
					//save dbl
					num_str += line[i];
					i++;
					numeric.type = token_t::LITERAL_DBL;
					numeric.value = num_str;
					tokens.push_back(numeric);
					continue;
				}
				else if(line[i]=='!'){
					num_str += line[i];
					i++;
					numeric.type = token_t::LITERAL_FLT;
					numeric.value = num_str;
					tokens.push_back(numeric);
					continue;
				}

				if (num_str.size() <= 8) {
					//save flt
					numeric.type = token_t::LITERAL_FLT;
					numeric.value = num_str;
					tokens.push_back(numeric);
					continue;
				}
				else{
					//save dbl
					numeric.type = token_t::LITERAL_DBL;
					numeric.value = num_str;
					tokens.push_back(numeric);
					continue;
				}
					
			}
			else if (token_type_map.contains(std::string(1,line[i]))) {
				//read in operator
				
				//save the current token value
				if (!token_value.empty()) {
					token token;
					token.value = token_value;
					token.line_number = line_number;
					token.type = token_type_map.contains(token_value) ? token_type_map.at(token_value) : token_t::IDENTIFIER;
					tokens.push_back(token);
					token_value.clear();
				}

				std::string op2 = line.substr(i, 2);
				std::string op1 = line.substr(i, 1);

				token op;
				op.line_number = line_number;

				if (token_type_map.contains(op2)) {
					op.value = op2;
					op.type = token_type_map.at(op2);
					tokens.push_back(op);
				}
				else if (token_type_map.contains(op1)) {
					op.value = op1;
					op.type = token_type_map.at(op1);
					tokens.push_back(op);
				}
			}
			else if (line[i] == ' ') {
				//save the current token value
				if (!token_value.empty()) {
					token token;
					token.value = token_value;
					token.line_number = line_number;
					token.type = token_type_map.contains(token_value) ? token_type_map.at(token_value) : token_t::IDENTIFIER;
					tokens.push_back(token);
					token_value.clear();

					//handle comments
					if (token.type == token_t::KEYWORD_REM) {
						i++;
						std::string comment;
						while (line[i] != ':' && line[i] != 0) {
							comment += line[i];
							i++;
						}
						tokens.emplace_back(token_t::LITERAL_STR,comment);
					}
				}
			}
			else if (i == line.size() - 1) {
				//save the current token value
				token_value += line[i];
				token token;
				token.line_number = line_number;
				token.value = token_value;
				token.type = token_type_map.contains(token_value) ? token_type_map.at(token_value) : token_t::IDENTIFIER;
				tokens.push_back(token);
				token_value.clear();
			}
			else {
				token_value += line[i];
			}

		}
		tokens.push_back(token(token_t::NEWLINE));
		line_number++;
	}
	tokens.push_back(token(token_t::END_OF_FILE));

	//Do another pass to check for unary/binary op defs
	for (size_t i = 1; i < tokens.size(); i++) {
		if (tokens[i].type == token_t::OPERATOR_NEG) {
			if ((tokens[i - 1].type >= token_t::LITERAL_STR && tokens[i - 1].type <= token_t::LITERAL_DBL)
				|| (tokens[i - 1].type == token_t::IDENTIFIER)
				|| (tokens[i - 1].type == token_t::CLOSE_PAREN)) 
			{
				tokens[i].type = token_t::OPERATOR_SUB;
			}
		}
		else if (tokens[i].type == token_t::OPERATOR_POS) {
			if ((tokens[i - 1].type >= token_t::LITERAL_STR && tokens[i - 1].type <= token_t::LITERAL_DBL)
				|| (tokens[i - 1].type == token_t::IDENTIFIER)
				|| (tokens[i - 1].type == token_t::CLOSE_PAREN))
			{
				tokens[i].type = token_t::OPERATOR_ADD;
			}
		}
	}

	return tokens;
}