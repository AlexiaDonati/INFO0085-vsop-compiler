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

void* Print_visitor::visit(Self* self){
    return new string("self");
}

void* Print_visitor::visit(Binop* binop){
    string left_result = this->get_value_from_void(binop->get_left_expr()->accept(this)),
           right_result = this->get_value_from_void(binop->get_right_expr()->accept(this));

    string result = "BinOp(" 
                  + binop->get_op() 
                  + ", " 
                  + left_result 
                  + ", " 
                  + right_result 
                  + ")";

    return new string(result);
}

void* Print_visitor::visit(Unop* unop){
    string expr_result = this->get_value_from_void(unop->get_expr()->accept(this));

    string result = "UnOp(" 
                  + unop->get_op() 
                  + ", " 
                  + expr_result 
                  + ")";

    return new string(result);
}

void* Print_visitor::visit(Assign* assign){
    string expr_result = this->get_value_from_void(assign->get_expr()->accept(this));

    string result = "Assign(" 
                  + assign->get_name() 
                  + ", " 
                  + expr_result 
                  + ")";

    return new string(result);
}

void* Print_visitor::visit(Let* let){
    string scope_expr_result = this->get_value_from_void(let->get_scope_expr()->accept(this)),
           init_expr_result;


    if(let->has_init_expr())
        init_expr_result = this->get_value_from_void(let->get_init_expr()->accept(this));

    string result = "Let("
                  + let->get_name()
                  + ", "
                  + let->get_type()
                  + ((let->has_init_expr()) ? ", " + init_expr_result : "")
                  + ", "
                  + scope_expr_result
                  + ")";

    return new string(result);
}

void* Print_visitor::visit(While* while_){
    string cond_result = this->get_value_from_void(while_->get_cond_expr()->accept(this)),
           body_result = this->get_value_from_void(while_->get_body_expr()->accept(this));

    string result = "While(" 
                  + cond_result 
                  + ", " 
                  + body_result 
                  + ")";

    return new string(result);
}

void* Print_visitor::visit(If* if_){
    string cond_result = this->get_value_from_void(if_->get_cond_expr()->accept(this)),
           then_result = this->get_value_from_void(if_->get_then_expr()->accept(this)),
           else_result;

    if(if_->has_else_expr())
        else_result = this->get_value_from_void(if_->get_else_expr()->accept(this));

    string result = "If(" 
                  + cond_result 
                  + ", " 
                  + then_result 
                  + ((if_->has_else_expr()) ? ", " + else_result : "")
                  + ")";

    return new string(result);
}

void* Print_visitor::visit(Program* program){
    return new string(this->print_list(program->get_class_list()));
}

void* Print_visitor::visit(Class* class_){
    string name = class_->get_name();
    string parent = class_->get_parent();
    string field_list_result = this->print_list(class_->get_field_list());
    string method_list_result = this->print_list(class_->get_method_list());

    string result = "Class(" 
                  + name + ", "
                  + parent + ", "
                  + field_list_result + ", "
                  + method_list_result
                  + ")";

    return new string(result);
}

void* Print_visitor::visit(Field* field){
    string name = field->get_name();
    string type = field->get_type();
    string init_expr_result;

    if(field->has_init_expr())
        init_expr_result = this->get_value_from_void(field->get_init_expr()->accept(this));

    string result = "Field(" 
                  + name + ", "
                  + type + ", "
                  + ((field->has_init_expr()) ? init_expr_result : "")
                  + ")";
    
    
    return new string(result);
}

void* Print_visitor::visit(Method* method){
    string name = method->get_name();
    string return_type = method->get_return_type();
    string body_block_result = this->get_value_from_void(method->get_body_block()->accept(this));
    string formal_list_result = this->print_list(method->get_formal_list());

    string result = "Method(" 
                    + name + ", "
                    + formal_list_result + ", "
                    + return_type + ", "
                    + body_block_result
                    + ")";

    return new string(result);
}

void* Print_visitor::visit(Formal* formal){
    string name = formal->get_name(),
           type = formal->get_type();

    string result = "" 
                  + name 
                  + " : " 
                  + type;

    return new string(result);
}

void* Print_visitor::visit(Block* block){
    return new string(this->print_list(block->get_expr_list()));
}

void* Print_visitor::visit(Call* call){
    string object_result = call->get_object();
    string method = call->get_method();
    string arg_expr_list_result = this->print_list(call->get_arg_expr_list());

    string result = "Call(" 
                  + object_result + ", "
                  + method + ", "
                  + arg_expr_list_result
                  + ")";

    return new string(result);
}