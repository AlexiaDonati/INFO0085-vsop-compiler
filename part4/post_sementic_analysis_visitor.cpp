#include "semantics_expressions.hpp"

using namespace AST;
using namespace std;

void* Post_sementic_analysis_visitor::visit(String* string_){
    string_->get_column();
    return NULL;
}

void* Post_sementic_analysis_visitor::visit(Integer* integer){
    integer->get_column();
    return NULL;
}

void* Post_sementic_analysis_visitor::visit(Boolean* boolean){
    boolean->get_column();
    return NULL;
}

void* Post_sementic_analysis_visitor::visit(Unit* unit){
    unit->get_column();
    return NULL;
}

void* Post_sementic_analysis_visitor::visit(Object* object){
    object->get_column();
    return NULL;
}

void* Post_sementic_analysis_visitor::visit(New* new_){
    new_->get_column();
    return NULL;
}

void* Post_sementic_analysis_visitor::visit(Self* self){
    self->get_column();
    return NULL;
}

void* Post_sementic_analysis_visitor::visit(Binop* binop){
    ACCEPT(binop->get_left_expr());
    ACCEPT(binop->get_right_expr());

    verify_binop(binop);
    return NULL;
}

void* Post_sementic_analysis_visitor::visit(Unop* unop){
    ACCEPT(unop->get_expr());

    if(unop->get_op() == "isnull")
        must_be_object(unop->get_expr());

    return NULL;
}

void* Post_sementic_analysis_visitor::visit(Assign* assign){
    ACCEPT(assign->get_expr());
    return NULL;
}

void* Post_sementic_analysis_visitor::visit(Let* let){
    ACCEPT(let->get_scope_expr());

    if(let->has_init_expr())
        ACCEPT(let->get_init_expr());
    
    return NULL;
}

void* Post_sementic_analysis_visitor::visit(While* while_){
    ACCEPT(while_->get_cond_expr());
    ACCEPT(while_->get_body_expr());
    return NULL;
}

void* Post_sementic_analysis_visitor::visit(If* if_){
    ACCEPT(if_->get_cond_expr());
    ACCEPT(if_->get_then_expr());

    if(if_->has_else_expr())
        ACCEPT(if_->get_else_expr());

    verify_if(if_);
    return NULL;
}

void* Post_sementic_analysis_visitor::visit(Program* program){
    ACCEPT_LIST(program->get_class_list());
    return NULL;
}

void* Post_sementic_analysis_visitor::visit(Class* class_){
    ACCEPT_LIST(class_->get_field_list());
    ACCEPT_LIST(class_->get_method_list());
    return NULL;
}

void* Post_sementic_analysis_visitor::visit(Field* field){
    if(field->has_init_expr())
        ACCEPT(field->get_init_expr());

    return NULL;
}

void* Post_sementic_analysis_visitor::visit(Method* method){
    ACCEPT(method->get_body_block());

    verify_method(method);

    return NULL;
}

void* Post_sementic_analysis_visitor::visit(Formal* formal){
    formal->get_column();
    return NULL;
}

void* Post_sementic_analysis_visitor::visit(Block* block){
    ACCEPT_LIST(block->get_expr_list());
    return NULL;
}

void* Post_sementic_analysis_visitor::visit(Call* call){
    ACCEPT(call->get_object());
    ACCEPT_LIST(call->get_arg_expr_list());

    must_have_the_same_amount_of_args(call);

    return NULL;
}

/* ----- other ----- */

void Post_sementic_analysis_visitor::must_be_object(Expr *expr){
    type::Table *aux = table->find_expr_table(expr);

    if(!Literals_visitor::is_child_of(aux->get_type(), S_TYPE_OBJECT) && aux->get_type() != "Object")
        table->throw_error(expr, 
            "" + aux->get_type() + " is not an Object" 
        );
}

void Post_sementic_analysis_visitor::must_have_the_same_amount_of_args(Call *call){
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

void Post_sementic_analysis_visitor::verify_if(If *if_){
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

void Post_sementic_analysis_visitor::verify_binop(Binop *binop){
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

void Post_sementic_analysis_visitor::verify_method(Method *method){
    type::Table *block_table = table->find_expr_table(method->get_body_block());

    string block_type = block_table->get_type();
    string method_type = method->get_return_type();

    if(!(method_type == block_type || Literals_visitor::is_child_of(block_type, method_type)))
        table->throw_error(method, 
            "return type are different"
        );
}