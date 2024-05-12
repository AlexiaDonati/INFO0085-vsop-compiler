#include <iostream>
#include <string>
#include <map>

#include "driver.hpp"
#include "parser.hpp"

using namespace std;
using namespace VSOP;

/**
 * @brief Map a token type to a string.
 */
static const map<Parser::token_type, string> type_to_string = {
    {Parser::token::INTEGER_LITERAL, "integer-literal"},
    {Parser::token::TYPE_IDENTIFIER, "type-identifier"},

    {Parser::token::AND, "and"},
    {Parser::token::BOOL, "bool"},
    {Parser::token::CLASS, "class"},
    {Parser::token::DO, "do"},
    {Parser::token::ELSE, "else"},
    {Parser::token::EXTENDS, "extends"},
    {Parser::token::FALSE, "false"},
    {Parser::token::IF, "if"},
    {Parser::token::IN, "in"},
    {Parser::token::INT32, "int32"},
    {Parser::token::ISNULL, "isnull"},
    {Parser::token::LET, "let"},
    {Parser::token::NEW, "new"},
    {Parser::token::NOT, "not"},
    {Parser::token::SELF, "self"},
    {Parser::token::STRING, "string"},
    {Parser::token::THEN, "then"},
    {Parser::token::TRUE, "true"},
    {Parser::token::UNIT, "unit"},
    {Parser::token::WHILE, "while"},

    {Parser::token::OBJECT_IDENTIFIER, "object-identifier"},
    {Parser::token::STRING_LITERAL, "string-literal"},

    {Parser::token::LBRACE, "lbrace"},
    {Parser::token::RBRACE, "rbrace"},
    {Parser::token::LPAR, "lpar"},
    {Parser::token::RPAR, "rpar"},
    {Parser::token::COLON, "colon"},
    {Parser::token::SEMICOLON, "semicolon"},
    {Parser::token::COMMA, "comma"},
    {Parser::token::PLUS, "plus"},
    {Parser::token::MINUS, "minus"},
    {Parser::token::TIMES, "times"},
    {Parser::token::DIV, "div"},
    {Parser::token::POW, "pow"},
    {Parser::token::DOT, "dot"},
    {Parser::token::EQUAL, "equal"},
    {Parser::token::LOWER_EQUAL, "lower-equal"},
    {Parser::token::ASSIGN, "assign"},
    {Parser::token::LOWER, "lower"},
};

/**
 * @brief Print the information about a token
 *
 * @param token the token
 */
static void print_token(Parser::symbol_type token)
{
    position pos = token.location.begin;
    Parser::token_type type = (Parser::token_type)token.type_get();
    int value;
    string id;

    cout << pos.line << ","
         << pos.column << ","
         << type_to_string.at(type);

    switch (type)
    {
        case Parser::token::INTEGER_LITERAL:
            value = token.value.as<int>();
            cout << "," << value;
            break;
        case Parser::token::TYPE_IDENTIFIER:
        case Parser::token::OBJECT_IDENTIFIER:
            id = token.value.as<string>();
            cout << "," << id;
            break;
        case Parser::token::STRING_LITERAL:
            id = token.value.as<string>();
            cout << ",\"" << id << "\"";
            break;
        default:
            break;
    }

    cout << endl;
}

int Driver::lex()
{
    scan_begin();

    int error = 0;

    while (true)
    {
        Parser::symbol_type token = yylex();

        if ((Parser::token_type)token.type_get() == Parser::token::YYEOF)
            break;

        if ((Parser::token_type)token.type_get() != Parser::token::YYerror)
            tokens.push_back(token);

        else
            error = 1;
    }

    scan_end();

    return error;
}

int Driver::parse()
{
    scan_begin();

    parser = new Parser(*this);

    int res = parser->parse();

    scan_end(); 

    delete parser;

    return res;
}

int Driver::semantic_analysis()
{
    AST::Check_classes check_classes;
    int *res = (int *) ast->accept(&check_classes);
    make_type_table();

    if (*res == 0)
        return 1;

    delete res;

    AST::Post_sementic_analysis_visitor* post_analysis = new AST::Post_sementic_analysis_visitor(type_table);

    ast->accept(post_analysis);

    //print_ast();

    if(type_table->has_error()){
        std::cerr << "\n" << type_table->errors_to_string() << "\n";
        return 1;
    }

    return 0;
}

void Driver::generate_code(){
    if(type_table == NULL)
        make_type_table();
        
    AST::Code_generation_visitor* code_generator = new AST::Code_generation_visitor(type_table);

    ast->accept(code_generator);
}

void Driver::make_type_table(){
    AST::Literals_visitor type_visitor;
    AST::type::Table *table = (AST::type::Table *) ast->accept(&type_visitor);

    set_type_table(table);
}

void Driver::print_tokens()
{
    for (auto token : tokens)
        print_token(token);
}
