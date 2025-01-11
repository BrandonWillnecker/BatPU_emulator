#pragma once

#include <string>
#include <vector>

#include "Lexer.h"

// Declarations
enum class DATA_TYPE { VOID = 0, BOOL, CHAR, FLOAT, INT, LONG, SHORT };

struct TYPE {
    bool is_signed = true;
    DATA_TYPE type = DATA_TYPE::VOID;
    std::string type_as_str;
    bool is_ptr = false;

    void set_type(const std::string& str_type) {
        type_as_str = str_type;
        if (str_type == "void") {
            type = DATA_TYPE::VOID;
        }
        else if (str_type == "bool") {
            type = DATA_TYPE::BOOL;
        }
        else if (str_type == "char") {
            type = DATA_TYPE::CHAR;
        }
        else if (str_type == "float") {
            type = DATA_TYPE::FLOAT;
        }
        else if (str_type == "int") {
            type = DATA_TYPE::INT;
        }
        else if (str_type == "long") {
            type = DATA_TYPE::LONG;
        }
        else if (str_type == "short") {
            type = DATA_TYPE::SHORT;
        }
    }

    void set_signed(const std::string& str_signed) {
        if (str_signed == "signed") {
            is_signed = true;
        }
        else if (str_signed == "unsigned") {
            is_signed = false;
        }
    }

    std::string str() const {
        std::string s;
        if (is_signed)
            s = "signed";
        else
            s = "unsigned";
        s += "_" + type_as_str;
        if (is_ptr) s += "_ptr";
        return s;
    }
};

struct STMT;
struct EXPR;
struct ARG;
struct PARSE_TREE;
struct RETURN;
struct ASSIGN;
struct RE_ASSIGN;
struct AUG_ASSIGN;
struct FOR;
struct WHILE;
struct IF;
struct BREAK;
struct CONTINUE;
struct CODE_BLOCK;

bool try_parse_function_dec(const std::vector<TOKEN>& tokens, PARSE_TREE& parse_tree);
bool try_parse_function_def(const std::vector<TOKEN>& tokens, PARSE_TREE& parse_tree);
bool try_parse_assign      (const std::vector<TOKEN>& tokens, PARSE_TREE& parse_tree);
bool try_parse_return      (const std::vector<TOKEN>& tokens, RETURN&     return_statement);
bool try_parse_assign      (const std::vector<TOKEN>& tokens, ASSIGN&     assign_statement);
bool try_parse_reassign    (const std::vector<TOKEN>& tokens, RE_ASSIGN&  reassign_statement);
bool try_parse_augassign   (const std::vector<TOKEN>& tokens, AUG_ASSIGN& augassign_statement);
bool try_parse_for         (const std::vector<TOKEN>& tokens, FOR&        for_statement);
bool try_parse_while       (const std::vector<TOKEN>& tokens, WHILE&      while_statement);
bool try_parse_if          (const std::vector<TOKEN>& tokens, IF&         if_statement);
bool try_parse_break       (const std::vector<TOKEN>& tokens, BREAK&      break_statement);
bool try_parse_continue    (const std::vector<TOKEN>& tokens, CONTINUE&   continue_statement);
bool try_parse_code_block  (const std::vector<TOKEN>& tokens, CODE_BLOCK& code_block);


// Tree Node Structs

struct STMT{
    virtual std::string str() const {
        return "Statement()";
    }
};

struct EXPR{
    virtual std::string str() const {
        return "Expression()";
    }
};

struct ARG {
    std::string name;
    TYPE type;

    ARG(std::string name, TYPE type) :
        name(name), type(type)
    {}

    std::string str() const {
        return "Arg(name = " + name + ", type = " + type.str() + ")";
    }

};

enum class EXPR_CONTEXT { LOAD, STORE };

static std::string expr_ctx_to_str(EXPR_CONTEXT expr_ctx) {
    switch (expr_ctx)
    {
    case EXPR_CONTEXT::LOAD: return "LOAD";
    case EXPR_CONTEXT::STORE: return "STORE";
    default: return "UNKNOWN_EXPRESSION_CONTEXT";
    }
}

enum class OP { ADD, SUB, MUL, DIV, MOD, RSH, LSH, BIT_OR, BIT_AND, BIT_XOR,
                AND, OR, XOR, NOT,
                EQUALS, NOT_EQUALS, LESS_THAN, LESS_THAN_OR_EQUALS, GREATER_THAN, GREATER_THAN_OR_EQUALS
             };

static std::string op_to_str(OP op) {
    switch (op)
    {
    case OP::ADD: return "ADD";
    case OP::SUB: return "SUB";
    case OP::MUL: return "MUL";
    case OP::DIV: return "DIV";
    case OP::MOD: return "MOD";
    case OP::RSH: return "RSH";
    case OP::LSH: return "LSH";
    case OP::BIT_OR: return "BIT_OR";
    case OP::BIT_AND: return "BIT_AND";
    case OP::BIT_XOR: return "BIT_XOR";
    case OP::AND: return "AND";
    case OP::OR: return "OR";
    case OP::XOR: return "XOR";
    case OP::NOT: return "NOT";
    case OP::EQUALS: return "EQUALS";
    case OP::NOT_EQUALS: return "NOT_EQUALS";
    case OP::LESS_THAN: return "LESS_THAN";
    case OP::LESS_THAN_OR_EQUALS: return "LESS_THAN_OR_EQUALS";
    case OP::GREATER_THAN: return "GREATER_THAN";
    case OP::GREATER_THAN_OR_EQUALS: "GREATER_THAN_OR_EQUALS";
    default: return "UNKNOWN_OP";
    }
}


//
// Statements
//
struct CODE_BLOCK : STMT {
    std::vector<std::shared_ptr<STMT>> body;

    std::string str() const override {
        std::string s = "CodeBlock(";
        for (const auto& statement : body) s += statement->str() + ", ";
        return s + ")";
    }
};

struct FUNCTION_DEC : STMT {
    TYPE return_type;
    std::string name;
    std::vector<ARG> args;

    std::string str() const override {
        std::string s = "FunctionDeclaration(return_type = " + return_type.str() + ", name = " + name + ", args = [";
        for (const ARG arg : args) s += arg.str() + ", ";
        return s + "])";
    }
};

struct FUNCTION_DEF : STMT {
    TYPE return_type;
    std::string name;
    std::vector<ARG> args;
    CODE_BLOCK code_block;

    std::string str() const override {
        std::string s = "FunctionDefinition(name = " + name + ", args = [";
        for (const ARG& arg : args) s += arg.str() + ", ";
        s += "], code_block = " + code_block.str();
        return s + "])";
    }

};

struct RETURN : STMT {
    std::shared_ptr<EXPR> value;

    std::string str() const override {
        return "Return(" + value->str() + ")";
    }
};

struct ASSIGN : STMT {
    TYPE type;
    std::shared_ptr<EXPR> target;
    std::shared_ptr<EXPR> value;

    std::string str() const override {
        return "Assignment(type = " + type.str() + ", target = " + target->str() + ", value = " + value->str() + ")";
    }
};

struct RE_ASSIGN : STMT {
    std::shared_ptr<EXPR> target;
    std::shared_ptr<EXPR> value;

    std::string str() const override {
        return "ReAssignment(target = " + target->str() + ", value = " + value->str() + ")";
    }
};

struct AUG_ASSIGN : STMT {
    std::shared_ptr<EXPR> target;
    OP op;
    std::shared_ptr<EXPR> value;

    std::string str() const override {
        return "AugmentedAssignment(target = " + target->str() + ", operator = " + op_to_str(op) + ", value = " + value->str() + ")";
    }
};

struct FOR : STMT {
    std::shared_ptr<STMT> init;
    std::shared_ptr<EXPR> condition;
    std::shared_ptr<EXPR> expr;
    CODE_BLOCK body;

    std::string str() const override {
        return "ForLoop(init = " + init->str() + ", condition = " + condition->str() + ", expr = " + expr->str() + ", code_block = " + body.str() + ")";
    }
};

struct WHILE : STMT {
    std::shared_ptr<EXPR> condition;
    CODE_BLOCK body;

    std::string str() const override {
        return "WhileLoop(condition = " + condition->str() + ", body = " + body.str() + ")";
    }
};

struct IF : STMT {
    std::shared_ptr<EXPR> condition;
    CODE_BLOCK body;
    CODE_BLOCK orelse;

    std::string str() const override {
        return "If(condition = " + condition->str() + ", body = " + body.str() + ", orelse = " + orelse.str() + ")";
    }
};

struct BREAK : STMT {
    std::string str() const override {
        return "Break()";
    }
};

struct CONTINUE : STMT {
    std::string str() const override {
        return "Continue()";
    }
};

//
// Expressions
//

struct BIN_OP : EXPR {
    std::shared_ptr<EXPR> left;
    OP op;
    std::shared_ptr<EXPR> right;

    std::string str() const override {
        return "BinaryOp(left = " + left->str() + ", operator = " + op_to_str(op) + ", right = " + right->str() + ")";
    }
};

struct UNARY_OP : EXPR {
    OP op;
    std::shared_ptr<EXPR> operand;

    std::string str() const override {
        return "UnaryOp(operator = " + op_to_str(op) + ", operand = " + operand->str() + ")";
    }
};

struct CALL : EXPR {
    std::shared_ptr<EXPR> func;
    std::vector<std::shared_ptr<EXPR>> args;

    std::string str() const override {
        std::string s = "Call( function = " + func->str() + ", arguments = [";
        for (const auto& expr : args) {
            s += expr->str() + ", ";
        }
        return s + "])";
    }
};

struct CONSTANT : EXPR {
    void* value;
    std::string str_value;

    CONSTANT(void* value, std::string str_value) : value(value), str_value(str_value) {}

    std::string str() const override {
        return "Constant(" + str_value + ")";
    }
};

struct SUBSCRIPT : EXPR {
    std::shared_ptr<EXPR> value;
    std::shared_ptr<EXPR> index;
    EXPR_CONTEXT ctx;

    std::string str() const override {
        return "Subscript(value = " + value->str() + ", index = " + index->str() + ", ctx = " + expr_ctx_to_str(ctx) + ")";
    }
};

struct ARRAY : EXPR {
    std::vector<std::shared_ptr<EXPR>> elements;

    std::string str() const override {
        std::string s = "Array(elements = [";
        for (const auto& expr : elements) {
            s += expr->str() + ", ";
        }
        return s + "])";
    }
};

struct NAME : EXPR {
    std::string id;
    EXPR_CONTEXT ctx;

    NAME(std::string id, EXPR_CONTEXT ctx) :
        id(id), ctx(ctx)
    {}

    std::string str() const override {
        return "Name(id = " + id + ")";
    }
};

struct PARSE_TREE {
    //global variables
    std::vector<ASSIGN> global_variable_assignments;
    //function declarations
    std::vector<FUNCTION_DEC> function_declarations;
    //function definitions
    std::vector<FUNCTION_DEF> function_definitions;
};


//
//  Statement Parsing
//

bool try_parse_function_dec(const std::vector<TOKEN>& tokens, PARSE_TREE& parse_tree) {
    if (tokens.empty()) return false;

    const std::unordered_set<std::string> valid_types = { "bool","char","int","long","short","void" };

    TYPE function_return_type;
    std::string function_name;
    std::vector<std::string> function_argument_types;
    std::vector<ARG> function_args;

    int index = 0;

    // type modifier
    if (tokens[index].value == "signed" || tokens[index].value == "unsigned") {
        function_return_type.set_signed(tokens[index].value);
        index++;
        if (index == tokens.size()) return false;
    }

    // type
    if (!valid_types.contains(tokens[index].value))
        return false;

    function_return_type.set_type(tokens[index].value);
    index++;
    if (index == tokens.size()) return false;

    // pointer ?
    if (tokens[index].value == "*") {
        function_return_type.is_ptr = true;
        index++;
        if (index == tokens.size()) return false;
    }

    // function name
    if (tokens[index].type != TOKEN_TYPE::IDENTIFIER)
        return false;

    function_name = tokens[index].value;
    index++;
    if (index == tokens.size()) return false;

    // function parameters
    if (tokens[index].value != "(")
        return false;

    index++;
    if (index == tokens.size()) return false;

    while ( tokens[index].value != ")" && index < tokens.size())
    {
        TYPE argument_type;

        //type modifier
        if (tokens[index].value == "signed" || tokens[index].value == "unsigned") {
            argument_type.set_signed(tokens[index].value);
            index++;
            if (index == tokens.size()) return false;
        }

        //type
        if (!valid_types.contains(tokens[index].value))
            return false;

        argument_type.set_type(tokens[index].value);
        index++;
        if (index == tokens.size()) return false;

        // pointer?
        if (tokens[index].value == "*") {
            argument_type.is_ptr = true;
            index++;
            if (index == tokens.size()) return false;
        }

        //save the type
        function_args.emplace_back("", argument_type);

        //parameter has name given, skip
        if (tokens[index].type == TOKEN_TYPE::IDENTIFIER) {
            index++;
            if (index == tokens.size()) return false;
        }

        //comma
        if (tokens[index].value == ",") {
            index++;
            if (index == tokens.size()) return false;
        }

    }

    //should be a ;
    index++;
    if (index == tokens.size()) return false;

    if (tokens[index].value != ";")
        return false;

    FUNCTION_DEC function_declaration;
    function_declaration.name = function_name;
    function_declaration.return_type = function_return_type; 
    function_declaration.args = function_args;

    parse_tree.function_declarations.push_back(function_declaration);

    return true;
}

bool try_parse_function_def(const std::vector<TOKEN>& tokens, PARSE_TREE& parse_tree) {
    if (tokens.empty()) return false;
    if (tokens.back().value != "}") return false;

    const std::unordered_set<std::string> valid_types = { "bool","char","int","long","short","void" };

    TYPE function_return_type;
    std::string function_name;
    std::vector<ARG> function_args;

    int index = 0;

    // type modifier
    if (tokens[index].value == "signed" || tokens[index].value == "unsigned") {
        function_return_type.set_signed(tokens[index].value);
        index++;
        if (index == tokens.size()) return false;
    }

    // type
    if (!valid_types.contains(tokens[index].value))
        return false;

    function_return_type.set_type(tokens[index].value);
    index++;
    if (index == tokens.size()) return false;

    // pointer ?
    if (tokens[index].value == "*") {
        function_return_type.is_ptr = true;
        index++;
        if (index == tokens.size()) return false;
    }

    // function name
    if (tokens[index].type != TOKEN_TYPE::IDENTIFIER)
        return false;

    function_name = tokens[index].value;
    index++;
    if (index == tokens.size()) return false;

    // function parameters
    if (tokens[index].value != "(")
        return false;

    //read until closing bracket

    index++;
    if (index == tokens.size()) return false;

    while (tokens[index].value != ")" && index < tokens.size())
    {
        TYPE argument_type;

        //type modifier
        if (tokens[index].value == "signed" || tokens[index].value == "unsigned") {
            argument_type.set_signed(tokens[index].value);
            index++;
            if (index == tokens.size()) return false;
        }

        //type
        if (!valid_types.contains(tokens[index].value))
            return false;

        argument_type.set_type(tokens[index].value);
        index++;
        if (index == tokens.size()) return false;

        // pointer?
        if (tokens[index].value == "*") {
            argument_type.is_ptr = true;
            index++;
            if (index == tokens.size()) return false;
        }

        //parameter name
        if (tokens[index].type == TOKEN_TYPE::IDENTIFIER) {
            std::string argument_name = tokens[index].value;
            function_args.emplace_back(argument_name, argument_type);
            index++;
            if (index == tokens.size()) return false;
        }

        //comma
        if (tokens[index].value == ",") {
            index++;
            if (index == tokens.size()) return false;
        }

    }


    //should be a function body code block {...}
    index++;
    if (index == tokens.size()) return false;

    if (tokens[index].value != "{")
        return false;

    index++;
    if (index == tokens.size()) return false;
    
    int start_index = index;
    int bracket_count = 1;

    while (bracket_count != 0 && index < tokens.size()) {
        if (tokens[index].value == "{") {
            bracket_count++;
            index++;
            if (index == tokens.size()) return false;
        }
        else if (tokens[index].value == "}") {
            bracket_count--;
            index++;
        }
        else {
            index++;
            if (index == tokens.size()) return false;
        }
    }

    if (index != tokens.size())
        return false;

    std::vector<TOKEN> code_block_tokens = std::vector<TOKEN>(tokens.begin() + start_index, tokens.end()-1);
    CODE_BLOCK code_block;

    if (try_parse_code_block(code_block_tokens, code_block)) {
        //save function definition
        FUNCTION_DEF function_definition;
        function_definition.name = function_name;
        function_definition.return_type = function_return_type;
        function_definition.args = function_args;
        function_definition.code_block = code_block;
        parse_tree.function_definitions.push_back(function_definition);
    }
    else {
        return false;
    }

    return true;
}

bool try_parse_assign(const std::vector<TOKEN>& tokens, PARSE_TREE& parse_tree) {
    // <type> <name> = <expression> ; OR <type> <name>;
    if (tokens.size() < 2) return false;

    const std::unordered_set<std::string> valid_types = { "bool","char","int","long","short","void" };

    TYPE variable_type;

    int index = 0;
    
    if (tokens[index].value == "signed" || tokens[index].value == "unsigned") {
        variable_type.set_signed(tokens[index].value);
        index++;
        if (index == tokens.size()) return false;
    }

    if (!valid_types.contains(tokens[index].value))
        return false;

    variable_type.set_type(tokens[index].value);
    index++;
    if (index == tokens.size()) return false;

    if (tokens[index].value == "*") {
        variable_type.is_ptr = true;
        index++;
        if (index == tokens.size()) return false;
    }

    std::string variable_name = tokens[index].value;
    index++;
    if (index == tokens.size()) return false;

    if (tokens[index].value == ";") {
        //no variable value given
        ASSIGN assignment;
        assignment.type = variable_type;
        assignment.target = std::make_shared<NAME>(variable_name, EXPR_CONTEXT::STORE);
        parse_tree.global_variable_assignments.push_back(assignment);
    }
    else if (tokens[index].value == "=") {
        //variable value given

    }
    else {
        return false;
    }

    return true;
}

bool try_parse_return(const std::vector<TOKEN>& tokens, RETURN& return_statement) {
    if (tokens.size() < 2) return false;

    if (tokens.front().value != "return") return false;
    if (tokens.back().value != ";") return false;

    std::vector<TOKEN> return_expr_tokens = std::vector<TOKEN>(tokens.begin() + 1, tokens.end() - 1);

    debug_print_tokens(return_expr_tokens);
    return false;
}

bool try_parse_assign(const std::vector<TOKEN>& tokens, ASSIGN& assign_statement) {
    return false;
}

bool try_parse_reassign(const std::vector<TOKEN>& tokens, RE_ASSIGN& reassign_statement) {
    return false;
}

bool try_parse_augassign(const std::vector<TOKEN>& tokens, AUG_ASSIGN& augassign_statement) {
    return false;
}

bool try_parse_for(const std::vector<TOKEN>& tokens, FOR& for_statement) {
    return false;
}

bool try_parse_while(const std::vector<TOKEN>& tokens, WHILE& while_statement) {
    return false;
}

bool try_parse_if(const std::vector<TOKEN>& tokens, IF& if_statement) {
    return false;
}

bool try_parse_break(const std::vector<TOKEN>& tokens, BREAK& break_statement) {
    return false;
}

bool try_parse_continue(const std::vector<TOKEN>& tokens, CONTINUE& continue_statement) {
    return false;
}

bool try_parse_code_block(const std::vector<TOKEN>& tokens, CODE_BLOCK& code_block) {
    return true;
}

//
//  Expression Parsing
//

bool try_parse_expression(const std::vector<TOKEN>& tokens, EXPR& expression) {
    return true;
}

void parse(const std::vector<TOKEN>& tokens, PARSE_TREE& parse_tree) {
    //Convert list of tokens into an Abstract Syntax Tree
    bool end_of_tokens = false;
    bool found = false;
    size_t start_index = 0;
    size_t end_index = 0;

    while (start_index < tokens.size()) {
        //Take a vector slice from start_index to end_index and try parse the slice
        for (; end_index <= tokens.size(); end_index++) {
            std::vector<TOKEN> tokens_slice = std::vector<TOKEN>(tokens.begin() + start_index, tokens.begin() + end_index);
            //Function declarations
            if (try_parse_function_dec(tokens_slice, parse_tree))
                break;
            //Function definitions
            if (try_parse_function_def(tokens_slice, parse_tree))
                break;
            //Global variable definitions
            if(try_parse_assign(tokens_slice,parse_tree))
                break;
        }
        start_index = end_index;
    }
}

//TODO - MAKE ALL THE POINTERS INTO SHARED POINTERS
// Still not sure about the pointing of nodes to other nodes in the tree
// The structs containing vectors of structs alredy takes care of the tree structure...BUT!!!! you cant have generics with 