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

    verify_binop(binop);

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

    if(unop->get_op() == "isnull")
        must_be_object(unop->get_expr());

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

    verify_if(if_);

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

    verify_method(method);

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

    must_have_the_same_amount_of_args(call);

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

void Print_visitor::must_be_object(Expr *expr){
    if(!must_use_table())
        return;

    type::Table *aux = table->find_expr_table(expr);

    if(!Literals_visitor::is_child_of(aux->get_type(), S_TYPE_OBJECT) && aux->get_type() != "Object")
        table->throw_error(expr, 
            "" + aux->get_type() + " is not an Object" 
        );
}

void Print_visitor::must_have_the_same_amount_of_args(Call *call){
    if(!must_use_table())
        return;

    type::Table *aux = table->find_expr_table(call);
    type::Table *object_table = table->find_expr_table(call->get_object());

    string object_name = aux->get_object_of_method(call->get_method());

    if(object_name == ""){
        object_name = object_table->get_type();
    }
    aux->set_type(Literals_visitor::get_dispatch_type(object_name, call->get_method()));

    if(Literals_visitor::number_of_args(object_name, call->get_method()) != call->get_arg_expr_list()->get_size()){
        table->throw_error(call, 
            call->get_method() + " does not have the good number of args : " + to_string(Literals_visitor::number_of_args(aux->get_object_of_method(call->get_method()), call->get_method()))
        );
    }

    List<Expr>* Expr_list = call->get_arg_expr_list();
    for (size_t i = 0; i < Expr_list->get_size(); i++){
        type::Table *arg_table = table->find_expr_table(Expr_list->get_element(i));
        if(!Literals_visitor::is_same_arg_type(object_name, call->get_method(), arg_table->get_type(), i))
            table->throw_error(call, 
                arg_table->get_type() + " is not the right arg type of " + call->get_method() + " at position " + to_string(i+1) 
            );
    }
}

void Print_visitor::verify_if(If *if_){
    if(!must_use_table())
        return;

    if(!if_->has_else_expr())
        return;

    type::Table *then_table = table->find_expr_table(if_->get_then_expr());
    type::Table *else_table = table->find_expr_table(if_->get_else_expr());

    string then_type = then_table->get_type();
    string else_type = else_table->get_type();

    if((then_table->is_primitive(then_type) || then_table->is_primitive(else_type))
    && !(then_type == S_TYPE_UNIT || else_type == S_TYPE_UNIT)
    && then_type != else_type){
        table->throw_error(if_, 
            "wrong types in condition"
        );
    }
}

void Print_visitor::verify_binop(Binop *binop){
    if(!must_use_table())
        return;

    type::Table *left_expr_table = table->find_expr_table(binop->get_left_expr());
    type::Table *right_expr_table = table->find_expr_table(binop->get_right_expr());


    string left_type = left_expr_table->get_type();
    string right_type = right_expr_table->get_type();

    if(binop->get_op() == "="){
        if((right_expr_table->is_primitive(left_type) || right_expr_table->is_primitive(right_type)) && left_type != right_type){
            table->throw_error(binop, 
                "binop = must have to same type to compare"
            );
        }
    }
}

void Print_visitor::verify_method(Method *method){
    if(!must_use_table())
        return;

    type::Table *block_table = table->find_expr_table(method->get_body_block());

    string block_type = block_table->get_type();
    string method_type = method->get_return_type();

    if(!(method_type == block_type || Literals_visitor::is_child_of(block_type, method_type)))
        table->throw_error(method, 
            "return type are different"
        );
}