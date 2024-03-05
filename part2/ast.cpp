#include "ast.hpp"

using namespace AST;
using namespace std;

void* Print_visitor::visit(Integer_literal integer_literal){
    std::string* str = new std::string(std::to_string(integer_literal.getValue()));
    return str;
}

void* Print_visitor::visit(Type_identifier type_identifier){
    std::string* str = new std::string(type_identifier.getValue());
    return str;
}

void* Print_visitor::visit(Keyword keyword){
    return nullptr;
}

void* Print_visitor::visit(Object_identifier object_identifier){
    return nullptr;
}

void* Print_visitor::visit(String_literal string_literal){
    return nullptr;
}

