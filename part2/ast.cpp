#include "ast.hpp"

#include <string>
#include <vector>
#include <map>

using namespace AST;
using namespace std;

void* Print_visitor::visit(String* string_){
    return new string(string_->get_value());
}

void* Print_visitor::visit(Integer* integer){
    return new string(to_string(integer->get_value()));
}

void* Print_visitor::visit(Boolean* boolean){
    return new string(boolean->get_value() ? "true" : "false");
}

void* Print_visitor::visit(Unit* unit){
    // to remove the warning
    unit->get_line();
    return new string("()");
}

void* Print_visitor::visit(Object* object){
    return new string(object->get_name());
}

void* Print_visitor::visit(New* new_){
    return new string("New(" + new_->get_type() + ")");
}

void* Print_visitor::visit(Binop* binop){
    string *left_result = (string*)binop->get_left_expr()->accept(this),
           *right_result = (string*)binop->get_right_expr()->accept(this);

    string result = "BinOp(" 
                  + binop->get_op() 
                  + ", " 
                  + *left_result 
                  + ", " 
                  + *right_result 
                  + ")";
    
    delete left_result;
    delete right_result;

    return new string(result);
}

void* Print_visitor::visit(Unop* unop){
    string *expr_result = (string*)unop->get_expr()->accept(this);

    string result = "UnOp(" 
                  + unop->get_op() 
                  + ", " 
                  + *expr_result 
                  + ")";
    
    delete expr_result;

    return new string(result);
}

void* Print_visitor::visit(Assign* assign){
    string *expr_result = (string*)assign->get_expr()->accept(this);

    string result = "Assign(" 
                  + assign->get_name() 
                  + ", " 
                  + *expr_result 
                  + ")";
    
    delete expr_result;

    return new string(result);
}

void* Print_visitor::visit(Let* let){
    string *scope_expr_result = (string*)let->get_scope_expr()->accept(this),
           *init_expr_result;


    if(let->has_init_expr())
        init_expr_result = (string*)let->get_init_expr()->accept(this);

    string result = "Let("
                  + let->get_name()
                  + ", "
                  + let->get_type()
                  + ((let->has_init_expr()) ? ", " + *init_expr_result : "")
                  + ", "
                  + *scope_expr_result
                  + ")";

    delete scope_expr_result;
    if (let->has_init_expr())
        delete init_expr_result;

    return new string(result);
}

void* Print_visitor::visit(While* while_){
    string *cond_result = (string*)while_->get_cond_expr()->accept(this),
           *body_result = (string*)while_->get_body_expr()->accept(this);

    string result = "While(" 
                  + *cond_result 
                  + ", " 
                  + *body_result 
                  + ")";
    
    delete cond_result;
    delete body_result;

    return new string(result);
}

void* Print_visitor::visit(If* if_){
    string *cond_result = (string*)if_->get_cond_expr()->accept(this),
           *then_result = (string*)if_->get_then_expr()->accept(this),
           *else_result;

    if(if_->has_else_expr())
        else_result = (string*)if_->get_else_expr()->accept(this);

    string result = "If(" 
                  + *cond_result 
                  + ", " 
                  + *then_result 
                  + ((if_->has_else_expr()) ? ", " + *else_result : "")
                  + ")";
    
    delete cond_result;
    delete then_result;
    if (if_->has_else_expr())
        delete else_result;

    return new string(result);
}

void* Print_visitor::visit(Program* program){
    program->get_line();
    return nullptr;
}
void* Print_visitor::visit(Class* class_){
    class_->get_line();
    return nullptr;
}
void* Print_visitor::visit(Field* field){
    field->get_line();
    return nullptr;
}
void* Print_visitor::visit(Method* method){
    method->get_line();
    return nullptr;
}
void* Print_visitor::visit(Formal* formal){
    formal->get_line();
    return nullptr;
}
void* Print_visitor::visit(Block* block){
    List<Expr>* expr_list = block->get_expr_list();
    size_t size = expr_list->get_size();

    string result = "Block[";

    for (size_t i = 0; i < size; i++){
        string* expr_result = (string*)expr_list->accept_one(this, i);

        result += *expr_result;
        result += (i+1 == size) ? "]" : ", ";

        delete expr_result;
    }

    return new string(result);
}

void* Print_visitor::visit(Call* call){
    call->get_line();
    return nullptr;
}