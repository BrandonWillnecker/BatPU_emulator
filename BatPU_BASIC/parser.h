#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <exception>
#include <unordered_map>
#include <unordered_set>
#include <deque>

#include "tokenizer.h"

enum class node_t {
	UNKNOWN,
	ROOT,
	VARIABLE,
	STMT_ASSIGN,
	STMT_CLEAR,
	STMT_DATA,
	STMT_DEF_FN,
	STMT_DIM,
	STMT_END,
	STMT_ERASE,
	STMT_ERROR,
	STMT_FOR,
	STMT_GOSUB,
	STMT_GOTO,
	STMT_IF,
	STMT_INPUT,
	STMT_MID,
	STMT_ON_ERROR_GOTO,
	STMT_ON_EXPR_GOTO,
	STMT_ON_EXPR_GOSUB,
	STMT_POKE,
	STMT_PEEK,
	STMT_PRINT,
	STMT_PRINT_SPACE,
	STMT_PRINT_NOSPACE,
	STMT_RANDOMIZE,
	STMT_READ,
	STMT_REM,
	STMT_RESTORE,
	STMT_RESUME,
	STMT_RESUME_NEXT,
	STMT_STOP,
	STMT_SWAP,
	STMT_WHILE,
	EXPR,
	OP_FUNC_CALL,
	OP_INDEXING,
	OP_NEG,
	OP_POS,
	OP_MUL,
	OP_FLT_DIV,
	OP_INT_DIV,
	OP_MOD,
	OP_ADD,
	OP_SUB,
	OP_EXP,
	OP_AND,
	OP_OR,
	OP_NOT,
	OP_XOR,
	OP_EQV,
	OP_IMP,
	OP_EQU,
	OP_GTR,
	OP_LSS,
	OP_LEQ,
	OP_GEQ,
	OP_NEQ,
	LITERAL_STR,
	LITERAL_INT,
	LITERAL_FLT,
	LITERAL_DBL,
};

static std::string node_t_to_str[] = {
	"UNKNOWN",
	"ROOT",
	"VARIABLE",
	"STMT_ASSIGN","STMT_CLEAR","STMT_DATA","STMT_DEF_FN","STMT_DIM","STMT_END",
	"STMT_ERASE","STMT_ERROR","STMT_FOR","STMT_GOSUB","STMT_GOTO","STMT_IF","STMT_INPUT","STMT_MID","STMT_ON_ERROR_GOTO",
	"STMT_ON_EXPR_GOTO","STMT_ON_EXPR_GOSUB","STMT_POKE","STMT_PEEK","STMT_PRINT","STMT_PRINT_SPACE","STMT_PRINT_NOSPACE",
	"STMT_RANDOMIZE","STMT_READ","STMT_REM","STMT_RESTORE","STMT_RESUME","STMT_RESUME_NEXT","STMT_STOP","STMT_SWAP","STMT_WHILE",
	"EXPR",
	"OP_FUNC_CALL","OP_INDEXING","OP_NEG","OP_POS","OP_MUL","OP_FLT_DIV","OP_INT_DIV","OP_MOD","OP_ADD","OP_SUB","OP_EXP",
	"OP_AND","OP_OR","OP_NOT","OP_XOR","OP_EQV","OP_IMP","OP_EQU","OP_GTR","OP_LSS","OP_LEQ","OP_GEQ","OP_NEQ",
	"LITERAL_STR","LITERAL_INT","LITERAL_FLT","LITERAL_DBL" };

struct node {
	node_t type = node_t::UNKNOWN;
	std::vector<node> child_nodes;
	int line_number = 0;

	union node_value {
		char str_val[255];
		float flt_val;
		double dbl_val;
		int int_val;
	} value = {};

	node() {}

	node(node_t type) : type(type) {}

	std::string str() const {
		std::string ret_str = node_t_to_str[(int)type] + "(";
			
		if (type == node_t::LITERAL_STR) {
			ret_str += "str_value = \""  + std::string(value.str_val) + "\"";
		}
		else if (type == node_t::LITERAL_INT) {
			ret_str += "int_value = " + std::to_string(value.int_val);
		}
		else if (type == node_t::LITERAL_FLT) {
			ret_str += "flt_value = " + std::to_string(value.flt_val);
		}
		else if (type == node_t::LITERAL_DBL) {
			ret_str += "dbl_value = " + std::to_string(value.dbl_val);
		}
		else {
			if (type != node_t::ROOT) {
				ret_str += "line_number = " + std::to_string(line_number) + ", ";
			}
			for (const node& child_node : child_nodes) {
				ret_str += child_node.str() + ", ";
			}
		}
		return ret_str + ")";	
	}
};


node_t get_decl_type(const std::string& name) {
	switch (name.back())
	{
	case '%': return node_t::LITERAL_INT;
	case '!': return node_t::LITERAL_FLT;
	case '#': return node_t::LITERAL_DBL;
	case '$': return node_t::LITERAL_STR;
	default: return node_t::LITERAL_FLT; //MSBASIC specification says default type is FLT
	}
}

//
//	LITERALS
//
bool try_parse_int(const std::string& str, int32_t& result) {
	try {
		result = std::stoi(str);
		// Conversion succeeded
		return true;
	}
	catch (const std::invalid_argument&) {
		// Conversion failed
		return false;
	}
}

bool try_parse_flt(const std::string& str, float& result) {
	std::string trimmed_str = str;
	if (trimmed_str.back() == '!') {
		trimmed_str.pop_back(); // Remove the trailing '!'
	}
	try {
		result = std::stof(trimmed_str);
		return true;
	}
	catch (const std::invalid_argument&) {
		return false;
	}
	catch (const std::out_of_range&) {
		return false;
	}
}

bool try_parse_dbl(const std::string& str, double& result) {
	std::string trimmed_str = str;
	if (trimmed_str.back() == '!') {
		trimmed_str.pop_back(); // Remove the trailing '!'
	}
	try {
		result = std::stod(trimmed_str);
		return true;
	}
	catch (const std::invalid_argument&) {
		return false;
	}
	catch (const std::out_of_range&) {
		return false;
	}
}

node create_literal_str(const std::string& str_value) {
	node str_node;
	str_node.type = node_t::LITERAL_STR;
	strcpy_s(str_node.value.str_val, str_value.c_str());
	return str_node;
}

node create_literal_int(int int_value) {
	node int_node;
	int_node.type = node_t::LITERAL_INT;
	int_node.value.int_val = int_value;
	return int_node;
}

node create_literal_flt(float flt_value) {
	node flt_node;
	flt_node.type = node_t::LITERAL_INT;
	flt_node.value.flt_val = flt_value;
	return flt_node;
}

node create_literal_dbl(double dbl_value) {
	node dbl_node;
	dbl_node.type = node_t::LITERAL_INT;
	dbl_node.value.dbl_val = dbl_value;
	return dbl_node;
}

node create_literal_from_token(const token& token) {
	node literal_node;
	switch (token.type)
	{
	case token_t::LITERAL_STR:
	{
		return create_literal_str(token.value);
	}
	case token_t::LITERAL_INT:
	{
		literal_node.type = node_t::LITERAL_INT;
		try_parse_int(token.value, literal_node.value.int_val);
		return literal_node;
	}
	case token_t::LITERAL_DBL:
	{
		literal_node.type = node_t::LITERAL_DBL;
		try_parse_dbl(token.value, literal_node.value.dbl_val);
		return literal_node;
	}
	case token_t::LITERAL_FLT:
	{
		literal_node.type = node_t::LITERAL_FLT;
		try_parse_flt(token.value, literal_node.value.flt_val);
		return literal_node;
	}
	default:
		throw std::runtime_error(std::format("Unable to parse the token {} into a literal", token.value));
	}
}

//
//	VARIABLE
//
node create_variable(const std::string& variable_name, node variable_value) {
	node variable;
	variable.type = node_t::VARIABLE;
	variable.child_nodes.push_back(create_literal_str(variable_name));
	variable.child_nodes.push_back(node(get_decl_type(variable_name)));
	variable.child_nodes.push_back(variable_value);
	return variable;
}

node create_variable(const std::string& variable_name) {
	node variable;
	variable.type = node_t::VARIABLE;
	variable.child_nodes.push_back(create_literal_str(variable_name));
	variable.child_nodes.push_back(node(get_decl_type(variable_name)));
	return variable;
}

void variable_set_name(node& variable, const std::string& variable_name) {
	strcpy_s(variable.child_nodes[0].value.str_val,variable_name.c_str());
}

std::string variable_get_name(node& variable) {
	return std::string(variable.child_nodes[0].value.str_val);
}

node_t variable_get_type(node& variable) {
	return variable.child_nodes[1].type;
}

node variable_get_value(node& variable) {
	return variable.child_nodes[2];
}

void variable_check_decl_type_match(node& variable, int line_number = 0) {
	node_t declared_type = variable.child_nodes[1].type;
	node_t variable_type = variable.child_nodes[2].type;
	if (declared_type != variable_type && variable_type!= node_t::EXPR) {
		//Attemp type conversion
		variable.child_nodes[2].type = declared_type;

		if (declared_type == node_t::LITERAL_INT) {
			switch (variable_type)
			{
			case node_t::LITERAL_STR: {
				//STR --> INT
				throw std::runtime_error(std::format("Unable to perform type conversion STR --> INT on line {}", line_number));
			}
			case node_t::LITERAL_FLT: {
				//FLT --> INT
				//MSBASIC says that FLT --> INT is done with rounding
				variable.child_nodes[2].value.int_val = (int)round(variable.child_nodes[2].value.flt_val);
				break;
			}
			case node_t::LITERAL_DBL: {
				//DBL --> INT
				//MSBASIC says that FLT --> INT is done with rounding
				// lets just do the same with DBL --> INT
				variable.child_nodes[2].value.int_val = (int)round(variable.child_nodes[2].value.dbl_val);
				break;
			}
			default:
				break;
			}
		}
		else if (declared_type == node_t::LITERAL_FLT) {
			switch (variable_type)
			{
			case node_t::LITERAL_STR: {
				//STR --> FLT
				throw std::runtime_error(std::format("Unable to perform type conversion STR --> FLT on line {}", line_number));
			}
			case node_t::LITERAL_INT: {
				//INT --> FLT
				variable.child_nodes[2].value.flt_val = (float)variable.child_nodes[2].value.int_val;
				break;
				break;
			}
			case node_t::LITERAL_DBL: {
				//DBL --> FLT
				variable.child_nodes[2].value.flt_val = (float)variable.child_nodes[2].value.dbl_val;
				break;
			}
			default:
				break;
			}
		}
		else if (declared_type == node_t::LITERAL_DBL) {
			switch (variable_type)
			{
			case node_t::LITERAL_STR: {
				//STR --> DBL
				throw std::runtime_error(std::format("Unable to perform type conversion STR --> DBL on line {}", line_number));
			}
			case node_t::LITERAL_INT: {
				//INT --> DBL
				variable.child_nodes[2].value.dbl_val = (double)variable.child_nodes[2].value.int_val;
				break;
			}
			case node_t::LITERAL_FLT: {
				//FLT --> DBL
				variable.child_nodes[2].value.dbl_val = (double)variable.child_nodes[2].value.flt_val;
				break;
			}
			default:
				break;
			}
		}
		else {
			// ? --> STR
			throw std::runtime_error(std::format("Unable to perform type conversion to STR on line {}", line_number));
		}
	}
}


//
//	FUNCTION
//
node create_function(const std::string& function_name, const std::vector<node> param_list, node function_expression) {
	node function;
	function.type = node_t::STMT_DEF_FN;
	function.child_nodes.push_back(create_literal_str(function_name));
	function.child_nodes.push_back(get_decl_type(function_name));
	for (node param : param_list) {
		function.child_nodes.push_back(param);
	}
	function.child_nodes.push_back(function_expression);
	return function;

}

void function_set_name(node& function, const std::string& function_name) {
	strcpy_s(function.child_nodes[0].value.str_val, function_name.c_str());
}

std::string function_get_name(node& function) {
	return std::string(function.child_nodes[0].value.str_val);
}

node_t function_get_type(node& function) {
	return function.child_nodes[1].type;
}

void function_set_type(node& function, node_t function_type) {
	function.child_nodes[1].type = function_type;
}

size_t function_param_count(const node& function) {
	return function.child_nodes.size() - 3;
}

void function_set_param_value(node& function, size_t param_index, node param_value) {
	function.child_nodes[2 + param_index].child_nodes[1] = param_value;
}

node function_get_param_value(const node& function, size_t param_index) {
	return function.child_nodes[2 + param_index].child_nodes[1];
}

void function_set_param_value(node& function, const std::string& param_name, node param_value) {
	for (size_t index = 3; index < function.child_nodes.size() - 1; index++) {
		if (strcmp(function.child_nodes[index].child_nodes[0].value.str_val, param_name.c_str()) == 0) {
			function.child_nodes[index].child_nodes[1] = param_value;
			return;
		}
	}
}

node function_get_param_value(const node& function, const std::string& param_name) {
	for (size_t index = 3; index < function.child_nodes.size() - 1; index++) {
		if (strcmp(function.child_nodes[index].child_nodes[0].value.str_val, param_name.c_str()) == 0) {
			return function.child_nodes[index].child_nodes[1];
		}
	}
	return {};
}

//
//	TYPES
//
node_t get_type(const node& node) {
	if (node.type == node_t::VARIABLE) {
		return node.child_nodes[1].type;
	}
	else if (node.type == node_t::LITERAL_DBL ||
			 node.type == node_t::LITERAL_FLT || 
		     node.type == node_t::LITERAL_INT || 
		     node.type == node_t::LITERAL_STR) 
	{
		return node.type;
	}
	return {};
}


//
// OPERATORS
//
node create_expression(token op, node operand_1, node operand_2 = {}) {
	node expr;
	expr.type = node_t::EXPR;

	switch (op.type)
	{
		case token_t::OPERATOR_EXP:
		{
			break;
		}
		case token_t::OPERATOR_NEG:
		{
			expr.type = node_t::OP_NEG;
			expr.child_nodes.push_back(operand_1);
			break;
		}
		case token_t::OPERATOR_POS:
		{
			expr.type = node_t::OP_POS;
			expr.child_nodes.push_back(operand_1);
			break;
		}
		case token_t::OPERATOR_MUL:
		{
			node op_mul;
			op_mul.type = node_t::OP_MUL;
			op_mul.child_nodes.push_back(operand_1);
			op_mul.child_nodes.push_back(operand_2);
			expr.child_nodes.push_back(op_mul);
			break;
		}
		case token_t::OPERATOR_FLT_DIV:
		{
			node op_flt_div;
			op_flt_div.type = node_t::OP_MUL;
			op_flt_div.child_nodes.push_back(operand_1);
			op_flt_div.child_nodes.push_back(operand_2);
			expr.child_nodes.push_back(op_flt_div);
			break;
		}
		case token_t::OPERATOR_ADD:
		{
			node op_add;
			op_add.type = node_t::OP_ADD;
			op_add.child_nodes.push_back(operand_1);
			op_add.child_nodes.push_back(operand_2);
			expr.child_nodes.push_back(op_add);
			break;
		}
		case token_t::OPERATOR_SUB:
		{
			node op_sub;
			op_sub.type = node_t::OP_SUB;
			op_sub.child_nodes.push_back(operand_1);
			op_sub.child_nodes.push_back(operand_2);
			expr.child_nodes.push_back(op_sub);
			break;
		}
		case token_t::OPERATOR_INT_DIV:
		{
			node op_int_div;
			op_int_div.type = node_t::OP_MUL;
			op_int_div.child_nodes.push_back(operand_1);
			op_int_div.child_nodes.push_back(operand_2);
			expr.child_nodes.push_back(op_int_div);
			break;
		}
		case token_t::OPERATOR_MOD:
		{
			node op_mod;
			op_mod.type = node_t::OP_SUB;
			op_mod.child_nodes.push_back(operand_1);
			op_mod.child_nodes.push_back(operand_2);
			expr.child_nodes.push_back(op_mod);
			break;
		}
		case token_t::OPERATOR_EQU:
		{
			break;
		}
		case token_t::OPERATOR_NEQ:
		{
			break;
		}
		case token_t::OPERATOR_LSS:
		{
			break;
		}
		case token_t::OPERATOR_GTR:
		{
			break;
		}
		case token_t::OPERATOR_LEQ:
		{
			break;
		}
		case token_t::OPERATOR_GEQ:
		{
			break;
		}
		case token_t::OPERATOR_NOT:
		{
			break;
		}
		case token_t::OPERATOR_AND:
		{
			break;
		}
		case token_t::OPERATOR_OR:
		{
			break;
		}
		case token_t::OPERATOR_XOR:
		{
			break;
		}
		case token_t::OPERATOR_EQV:
		{
			break;
		}
		case token_t::OPERATOR_IMP:
		{
			break;
		}
	default:
		break;
	}

	return expr;
}


std::vector<int> jmp_list; //keep track of lines that need a label for (GOTO) jmp instructions
std::unordered_map<std::string, node> map_name_to_variable;
std::unordered_map<std::string, node> map_name_to_function;

std::unordered_map<node_t, int> map_op_to_precedence =
{
	{node_t::OP_FUNC_CALL,22},
	{node_t::OP_INDEXING,21},
	{node_t::OP_NEG,20},
	{node_t::OP_POS,19},
	{node_t::OP_EXP,18},
	{node_t::OP_MUL,17},
	{node_t::OP_FLT_DIV,16},
	{node_t::OP_INT_DIV,15},
	{node_t::OP_MOD,14},
	{node_t::OP_SUB,13},
	{node_t::OP_ADD,12},
	{node_t::OP_NOT,11},
	{node_t::OP_AND,10},
	{node_t::OP_OR,9},
	{node_t::OP_XOR,8},
	{node_t::OP_EQV,7},
	{node_t::OP_IMP,6},
	{node_t::OP_EQU,5},
	{node_t::OP_GTR,4},
	{node_t::OP_LSS,3},
	{node_t::OP_LEQ,2},
	{node_t::OP_GEQ,1},
	{node_t::OP_NEQ,0}
};

std::unordered_set<node_t> node_ops = {
	node_t::OP_FUNC_CALL,
	node_t::OP_INDEXING,
	node_t::OP_NEG,
	node_t::OP_POS,
	node_t::OP_MUL,
	node_t::OP_FLT_DIV,
	node_t::OP_INT_DIV,
	node_t::OP_MOD,
	node_t::OP_ADD,
	node_t::OP_SUB,
	node_t::OP_EXP,
	node_t::OP_AND,
	node_t::OP_OR,
	node_t::OP_NOT,
	node_t::OP_XOR,
	node_t::OP_EQV,
	node_t::OP_IMP,
	node_t::OP_EQU,
	node_t::OP_GTR,
	node_t::OP_LSS,
	node_t::OP_LEQ,
	node_t::OP_GEQ,
	node_t::OP_NEQ
};

std::unordered_set<token_t> token_ops = {
	token_t::OPERATOR_EXP,
	token_t::OPERATOR_NEG,
	token_t::OPERATOR_POS,
	token_t::OPERATOR_MUL,
	token_t::OPERATOR_FLT_DIV,
	token_t::OPERATOR_ADD,
	token_t::OPERATOR_SUB,
	token_t::OPERATOR_INT_DIV,
	token_t::OPERATOR_MOD,
	token_t::OPERATOR_EQU,
	token_t::OPERATOR_NEQ,
	token_t::OPERATOR_LSS,
	token_t::OPERATOR_GTR,
	token_t::OPERATOR_LEQ,
	token_t::OPERATOR_GEQ,
	token_t::OPERATOR_NOT,
	token_t::OPERATOR_AND,
	token_t::OPERATOR_OR,
	token_t::OPERATOR_XOR,
	token_t::OPERATOR_EQV,
	token_t::OPERATOR_IMP
};


node parse_expression(std::vector<token>& tokens, int line_number, const node& func = {}) {
	if (tokens.empty())
		throw std::runtime_error("Attempting to evaluate an empty expression");

	//Check for unwanted surrounding brackets
	if (tokens.front().type == token_t::OPEN_PAREN && tokens.back().type == token_t::CLOSE_PAREN) {
		int bracket_count = 1;
		for (size_t i = 1; i < tokens.size(); i++) {
			if (tokens[i].type == token_t::OPEN_PAREN)
				bracket_count++;
			else if (tokens[i].type == token_t::CLOSE_PAREN)
				bracket_count--;
		}
		if (bracket_count == 0) {
			tokens = std::vector<token>(tokens.begin() + 1, tokens.end() - 1);
		}
	}

	//Base case
	if (tokens.size() == 1) {
		//Single token to evaluate
		if (tokens[0].type >= token_t::LITERAL_STR && tokens[0].type <= token_t::LITERAL_DBL) {
			return create_literal_from_token(tokens[0]);
		}
		else if (tokens[0].type == token_t::IDENTIFIER) {
			std::string variable_name = tokens[0].value;

			if (function_param_count(func) != 0) {
				return function_get_param_value(func, variable_name);
			}
			else if (map_name_to_variable.contains(variable_name)) {
				return map_name_to_variable[variable_name];
			}
			else {
				throw std::runtime_error(std::format("Variable, {}, on line {} used before defined", tokens[0].value, line_number));
			}	
		}
	}
	else if (tokens.size() == 2 && token_ops.contains(tokens[0].type)) {
		//Unary Expression
		token op = tokens[0];
		std::vector<token> op_tokens_1 = { tokens[1] };
		node operand_1 = parse_expression(op_tokens_1, line_number, func);
		return create_expression(op, operand_1);
	}
	else if (tokens.size() == 3 && token_ops.contains(tokens[1].type)) {
		//Binary Expressionss
		std::vector<token> op_tokens_1 = { tokens[0] };
		std::vector<token> op_tokens_2 = { tokens[2] };
		token op = tokens[1];

		node operand_1 = parse_expression(op_tokens_1, line_number, func);
		node operand_2 = parse_expression(op_tokens_2, line_number, func);
		node expr = create_expression(op, operand_1, operand_2);
		return expr ;
	}

	//Take tokens and turn into an EXPR
	// EXPR has one child node that is either some Binary Operation or some Unary Operation which forms the root of the AST for the EXPR

	//NOTE : on a function call (1) update the functions parameter values with given values
	//                          (2) replace the OP_FUNC_CALL node with parse_expression(function.child_nodes.last(),func)
	
	int bracket_count = 0;
	for (size_t i = 0; i < tokens.size(); i++) {
		if (tokens[i].type == token_t::OPEN_PAREN) {
			bracket_count++;
		}
		else if (tokens[i].type == token_t::CLOSE_PAREN) {
			bracket_count--;
		}
		else if (token_ops.contains(tokens[i].type) && bracket_count == 0) {
			//Binary operator found
			//Read left and right for operands
			token op = tokens[i];
			std::vector<token> left_op_tokens = std::vector<token>(tokens.begin(), tokens.begin() + i);
			std::vector<token> right_op_tokens = std::vector<token>(tokens.begin() + i + 1, tokens.end());
			node left_op = parse_expression(left_op_tokens, line_number, func);
			node right_op = parse_expression(right_op_tokens, line_number, func);
			node expression = create_expression(op, left_op, right_op);
			return expression;
		}
	}

	return {};
}

node parse_line(const std::vector<token>& line, int line_number) {
	node stmt;
	stmt.line_number = line_number;

	if (line[0].type >= token_t::KEYWORD_CLEAR && line[0].type <= token_t::KEYWORD_WEND) {
		//parse cmd
		switch (line[0].type)
		{
		case token_t::KEYWORD_CLEAR:{
			stmt.type = node_t::STMT_CLEAR;
			break;
		}
		
		case token_t::KEYWORD_DATA:
		{
			stmt.type = node_t::STMT_DATA;
			for (size_t i = 1; i < line.size() - 1; i++) {
				node data_node = create_literal_from_token(line[i]);
				stmt.child_nodes.push_back(data_node);
				i++;
				if (line[i].type != token_t::COMMA) {
					throw std::runtime_error(std::format("Data values should be separated by a comma on line {}", line_number));
				}
			}
			node data_node = create_literal_from_token(line.back());
			stmt.child_nodes.push_back(data_node);
			break;
		}
		
		case token_t::KEYWORD_END:
		{
			stmt.type = node_t::STMT_END;
			break;
		}
		
		case token_t::KEYWORD_GOSUB:
		{
			stmt.type = node_t::STMT_GOSUB;
			break;
		}
		
		case token_t::KEYWORD_GOTO:
		{
			stmt.type = node_t::STMT_GOTO;
			node goto_location;
			goto_location.type = node_t::LITERAL_INT;
			if (!try_parse_int(line[1].value, goto_location.value.int_val))
				throw std::runtime_error(std::format("Invalid GOTO loaction {} on line {}", line[1].value, line_number));
			jmp_list.push_back(goto_location.value.int_val);
			stmt.child_nodes.push_back(goto_location);
			break;
		}
		
		case token_t::KEYWORD_PRINT: 
		{
			stmt.type = node_t::STMT_PRINT;
			std::vector<token> print_expression_tokens;
			for (size_t i = 1; i < line.size(); i++) {
				if (line[i].type == token_t::COMMA) {
					node print_stmt;
					print_stmt.type = node_t::STMT_PRINT_SPACE;
					print_stmt.child_nodes.push_back(parse_expression(print_expression_tokens, line_number));
					stmt.child_nodes.push_back(print_stmt);
					print_expression_tokens.clear();
				}
				else if (line[i].type == token_t::SEMICOLON) {
					node print_stmt;
					print_stmt.type = node_t::STMT_PRINT_NOSPACE;
					print_stmt.child_nodes.push_back(parse_expression(print_expression_tokens, line_number));
					stmt.child_nodes.push_back(print_stmt);
					print_expression_tokens.clear();
				}
				else {
					print_expression_tokens.push_back(line[i]);
				}
			}
			node print_stmt;
			print_stmt.type = node_t::STMT_PRINT_NOSPACE;
			print_stmt.child_nodes.push_back(parse_expression(print_expression_tokens, line_number));
			stmt.child_nodes.push_back(print_stmt);
			print_expression_tokens.clear();
			break;
		}
		
		case token_t::KEYWORD_REM: 
		{
			stmt.type = node_t::STMT_REM;
			node comment = create_literal_str(line[1].value);
			stmt.child_nodes.push_back(comment);
			break;
		}
		
		case token_t::KEYWORD_STOP: {
			stmt.type = node_t::STMT_STOP;
			break;
		}
		
		case token_t::KEYWORD_ERASE: {
			stmt.type = node_t::STMT_ERASE;
			break;
		}
		
		case token_t::KEYWORD_DEF: {
			//DEF FN<NAME>(param1, param2,...) = <EXPR>
			
			//Check function name
			std::string function_name = line[1].value;
			if (function_name.substr(0, 2) != "FN") {
				throw std::runtime_error(std::format("Function name, {}, on line {} needs to start with FN", function_name, line_number));
			}

			//Check parameter list
			std::vector<node> param_list;

			if (line[2].type != token_t::OPEN_PAREN) {
				throw std::runtime_error(std::format("The function, {}, on line {} needs parenthesis", function_name, line_number));
			}

			std::string param_name;
			size_t i = 3;
			for (; line[i].type != token_t::CLOSE_PAREN; i++) {
				if (line[i].type == token_t::COMMA) {
					//save param
					param_list.push_back(create_variable(param_name));
				}
				else if (line[i].type == token_t::IDENTIFIER) {
					param_name = line[i].value;
				}
				else {
					throw std::runtime_error(std::format("The function, {}, on line {} does not have valid parameter names", function_name, line_number));
				}	
			}
			param_list.push_back(create_variable(param_name));

			i++;
			if (line[i].type != token_t::OPERATOR_ASSIGN) {
				throw std::runtime_error(std::format("The function, {}, on line {} does not have a valid definition", function_name, line_number));
			}
			i++;

			std::vector<token> function_expr_tokens = std::vector<token>(line.begin() + i, line.end());
			node function_expr = parse_expression(function_expr_tokens, line_number, stmt);

			stmt = create_function(function_name, param_list, function_expr);
			map_name_to_function[function_name] = stmt;
			break;
		}

		default:
			throw std::runtime_error(std::format("Unknown command, {}, on line {}", line[0].value, line_number));
		}
	}
	else if (line[0].type == token_t::IDENTIFIER && line[1].type == token_t::OPERATOR_ASSIGN) {
		//parse variable assign
		stmt.type = node_t::STMT_ASSIGN;

		//Get name and expression value
		std::string variable_name = line[0].value;
		std::vector<token> variable_value_token_expr(line.begin() + 2, line.end());
		node variable_value = parse_expression(variable_value_token_expr, line_number);

		//Create variable object and add it to assignent object
		node variable = create_variable(variable_name,variable_value);
		variable_check_decl_type_match(variable, line_number);

		map_name_to_variable[variable_name] = variable;
		stmt.child_nodes.push_back(variable);
	}
	else {
		throw std::runtime_error(std::format("Line {} does not seem to be a command or variable assignment", line_number));
	}

	return stmt;
}

node parse(const std::string& filename) {
	node root;
	root.type = node_t::ROOT;

	std::vector<token> tokens = tokenize(filename);
	std::vector<token> line;

	int line_number = 0;
	if (!try_parse_int(tokens[0].value, line_number))
		throw std::runtime_error(std::format("No line number on first line"));

	for (size_t i = 1; i < tokens.size(); i++) {
		if (tokens[i].type == token_t::NEWLINE && !line.empty()) {
			node stmt = parse_line(line, line_number);
			root.child_nodes.push_back(stmt);
			line.clear();
			i++;
			if (tokens[i].type != token_t::END_OF_FILE) {
				if (!try_parse_int(tokens[i].value, line_number))
					throw std::runtime_error(std::format("Expected a line number : {}", tokens[i].value));
			}			
		}
		else if (tokens[i].type == token_t::COLON && !line.empty()) {
			node stmt = parse_line(line, line_number);
			root.child_nodes.push_back(stmt);
			line.clear();
		}
		else {
			line.push_back(tokens[i]);
		}
	}

	return root;
}