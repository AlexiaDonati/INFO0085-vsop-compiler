#include "ast.hpp"

#include <string>
#include <map>

using namespace AST;
using namespace std;

void* Print_visitor::visit(Integer_literal integer_literal){
    std::string* str = new std::string(std::to_string(integer_literal.get_value()));
    return str;
}

void* Print_visitor::visit(Type_identifier type_identifier){
    std::string* str = new std::string(type_identifier.get_value());
    return str;
}

void* Print_visitor::visit(Keyword keyword){
    KEYWORD kw = keyword.get_value();
    string result = "";
    
    switch (kw)
    {
        case CLASS:
        {
            Expr* name = keyword.get_child(0);
            Expr* parent = keyword.get_child(1);
            Expr* fields = keyword.get_child(2);
            Expr* methods = keyword.get_child(3);

            // Class must have a name, a parent, fields and methods
            if(!name || !parent || !fields || !methods)
                return nullptr;

            string* name_result = (string*)name->accept(this);
            string* parent_result = (string*)parent->accept(this);
            string* fields_result = (string*)fields->accept(this);
            string* methods_result = (string*)methods->accept(this);

            result = "Class("
                         + *name_result
                         + ", "
                         + *parent_result
                         + ", "
                         + *fields_result
                         + ", "
                         + *methods_result
                         + ")";

            delete name_result;
            delete parent_result;
            delete fields_result;
            delete methods_result;
            break;
        }
        default:
            break;
    }

    if (result == "")
        return nullptr;

    return new string(result);
}

void* Print_visitor::visit(Object_identifier object_identifier){
    object_identifier.get_line();
    return nullptr;
}

void* Print_visitor::visit(String_literal string_literal){
    string_literal.get_line();
    return nullptr;
}

void* Print_visitor::visit(Lbrace lbrace){
    lbrace.get_line();
    return nullptr;
}

void* Print_visitor::visit(Rbrace rbrace){
    rbrace.get_line();
    return nullptr;
}

void* Print_visitor::visit(Lpar lpar){
    lpar.get_line();
    return nullptr;
}

void* Print_visitor::visit(Rpar rpar){
    rpar.get_line();
    return nullptr;
}

void* Print_visitor::visit(Colon colon){
    colon.get_line();
    return nullptr;
}

void* Print_visitor::visit(Semicolon semicolon){
    semicolon.get_line();
    return nullptr;
}

void* Print_visitor::visit(Comma comma){
    comma.get_line();
    return nullptr;
}

void* Print_visitor::visit(Plus plus){
    plus.get_line();
    return nullptr;
}

void* Print_visitor::visit(Minus minus){
    minus.get_line();
    return nullptr;
}

void* Print_visitor::visit(Times times){
    times.get_line();
    return nullptr;
}

void* Print_visitor::visit(Div div){
    div.get_line();
    return nullptr;
}

void* Print_visitor::visit(Pow pow){
    pow.get_line();
    return nullptr;
}

void* Print_visitor::visit(Dot dot){
    dot.get_line();
    return nullptr;
}

void* Print_visitor::visit(Equal equal){
    equal.get_line();
    return nullptr;
}

void* Print_visitor::visit(Lower_equal lower_equal){
    lower_equal.get_line();
    return nullptr;
}

void* Print_visitor::visit(Assign assign){
    assign.get_line();
    return nullptr;
}

void* Print_visitor::visit(Lower lower){
    lower.get_line();
    return nullptr;
}