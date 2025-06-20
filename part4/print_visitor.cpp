#include "ast.hpp"
#include "semantics_expressions.hpp"
#include <iostream>

using namespace AST;
using namespace std;

void* Print_visitor::visit(String* string_){
    string result = "\"" + string_->get_value() + "\"" + type_to_string(string_);
    return TO_VOID(result);
}

void* Print_visitor::visit(Integer* integer){
    string result = to_string(integer->get_value()) + type_to_string(integer);
    return TO_VOID(result);
}

void* Print_visitor::visit(Boolean* boolean){
    string result = (boolean->get_value() ? "true" : "false");
    result += type_to_string(boolean);
    return TO_VOID(result);
}

void* Print_visitor::visit(Unit* unit){
    // to remove the warning
    unit->get_line();
    string result = "()";
    result += type_to_string(unit);
    return TO_VOID(result);
}

void* Print_visitor::visit(Object* object){
    string result = object->get_name();

    result += type_to_string(object);

    return TO_VOID(result);
}

void* Print_visitor::visit(New* new_){
    return TO_VOID("New(" + new_->get_type() + ")" + type_to_string(new_));
}

void* Print_visitor::visit(Self* self){
    return TO_VOID("self" + type_to_string(self, S_TYPE_SELF));
}

void* Print_visitor::visit(Binop* binop){
    string left_result = ACCEPT(binop->get_left_expr());
    string right_result = ACCEPT(binop->get_right_expr());

    string result = "BinOp(" 
                  + binop->get_op() 
                  + ", " 
                  + left_result 
                  + ", " 
                  + right_result 
                  + ")"
                  + type_to_string(binop);

    return TO_VOID(result);
}

void* Print_visitor::visit(Unop* unop){
    string expr_result = ACCEPT(unop->get_expr());

    string result = "UnOp(" 
                  + unop->get_op() 
                  + ", " 
                  + expr_result 
                  + ")"
                  + type_to_string(unop);

    return TO_VOID(result);
}

void* Print_visitor::visit(Assign* assign){
    string expr_result = ACCEPT(assign->get_expr());

    string result = "Assign(" 
                  + assign->get_name() 
                  + ", " 
                  + expr_result 
                  + ")"
                  + type_to_string(assign);

    return TO_VOID(result);
}

void* Print_visitor::visit(Let* let){
    string scope_expr_result = ACCEPT(let->get_scope_expr());
    string init_expr_result;

    if(let->has_init_expr())
        init_expr_result = ACCEPT(let->get_init_expr());

    string result = "Let("
                  + let->get_name()
                  + ", "
                  + let->get_type()
                  + ((let->has_init_expr()) ? ", " + init_expr_result : "")
                  + ", "
                  + scope_expr_result
                  + ")"
                  + type_to_string(let);

    return TO_VOID(result);
}

void* Print_visitor::visit(While* while_){
    string cond_result = ACCEPT(while_->get_cond_expr());
    string body_result = ACCEPT(while_->get_body_expr());

    string result = "While(" 
                  + cond_result 
                  + ", " 
                  + body_result 
                  + ")"
                  + type_to_string(while_);

    return TO_VOID(result);
}

void* Print_visitor::visit(If* if_){
    string cond_result = ACCEPT(if_->get_cond_expr());
    string then_result = ACCEPT(if_->get_then_expr());
    string else_result;

    if(if_->has_else_expr())
        else_result = ACCEPT(if_->get_else_expr());

    string result = "If(" 
                  + cond_result 
                  + ", " 
                  + then_result 
                  + ((if_->has_else_expr()) ? ", " + else_result : "")
                  + ")"
                  + type_to_string(if_);

    return TO_VOID(result);
}

void* Print_visitor::visit(Program* program){
    return TO_VOID(ACCEPT_LIST(program->get_class_list()));
}

void* Print_visitor::visit(Class* class_){
    string name = class_->get_name();
    string parent = class_->get_parent();
    string field_list_result = ACCEPT_LIST(class_->get_field_list());
    string method_list_result = ACCEPT_LIST(class_->get_method_list());

    string result = "Class(" 
                  + name + ", "
                  + parent + ", "
                  + field_list_result + ", "
                  + method_list_result
                  + ")";

    return TO_VOID(result);
}

void* Print_visitor::visit(Field* field){
    string name = field->get_name();
    string type = field->get_type();
    string init_expr_result;

    if(field->has_init_expr())
        init_expr_result = ACCEPT(field->get_init_expr());

    string result = "Field(" 
                  + name + ", "
                  + type
                  + ((field->has_init_expr()) ? ", " + init_expr_result : "")
                  + ")";
    
    return TO_VOID(result);
}

void* Print_visitor::visit(Method* method){
    string name = method->get_name();
    string return_type = method->get_return_type();
    string body_block_result = ACCEPT(method->get_body_block());
    string formal_list_result = ACCEPT_LIST(method->get_formal_list());

    string result = "Method(" 
                    + name + ", "
                    + formal_list_result + ", "
                    + return_type + ", "
                    + body_block_result
                    + ")";

    return TO_VOID(result);
}

void* Print_visitor::visit(Formal* formal){
    string name = formal->get_name();
    string type = formal->get_type();

    string result = "" 
                  + name 
                  + " : "
                  + type;

    return TO_VOID(result);
}

void* Print_visitor::visit(Block* block){
    return TO_VOID(ACCEPT_LIST(block->get_expr_list()) + type_to_string(block));
}

void* Print_visitor::visit(Call* call){
    string object_result = ACCEPT(call->get_object());
    string method = call->get_method();
    string arg_expr_list_result = ACCEPT_LIST(call->get_arg_expr_list());

    string result = "Call(" 
                  + object_result + ", "
                  + method + ", "
                  + arg_expr_list_result
                  + ")"
                  + type_to_string(call);

    return TO_VOID(result);
}

/* ----- other ----- */

bool Print_visitor::must_use_table(){return (table != NULL);}

std::string Print_visitor::type_to_string(Expr *expr){
    if(table == NULL)
        return "";

    type::Table *aux = table->find_expr_table(expr);

    return " : " + aux->type_to_string();
}

std::string Print_visitor::type_to_string(Expr *expr, std::string name){
    if(!must_use_table())
        return "";

    type::Table *aux = table->find_expr_table(expr);

    return " : " + aux->type_to_string(name);
}