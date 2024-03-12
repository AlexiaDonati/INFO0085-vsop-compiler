#include "ast.hpp"

#include <string>
#include <vector>
#include <map>

using namespace AST;
using namespace std;

template <typename T>
T get_value_from_void(void* void_value);
template <typename T>
string print_list(Visitor* visitor, List<T>* list);

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
    string left_result = get_value_from_void<string>(binop->get_left_expr()->accept(this)),
           right_result = get_value_from_void<string>(binop->get_right_expr()->accept(this));

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
    string expr_result = get_value_from_void<string>(unop->get_expr()->accept(this));

    string result = "UnOp(" 
                  + unop->get_op() 
                  + ", " 
                  + expr_result 
                  + ")";

    return new string(result);
}

void* Print_visitor::visit(Assign* assign){
    string expr_result = get_value_from_void<string>(assign->get_expr()->accept(this));

    string result = "Assign(" 
                  + assign->get_name() 
                  + ", " 
                  + expr_result 
                  + ")";

    return new string(result);
}

void* Print_visitor::visit(Let* let){
    string scope_expr_result = get_value_from_void<string>(let->get_scope_expr()->accept(this)),
           init_expr_result;


    if(let->has_init_expr())
        init_expr_result = get_value_from_void<string>(let->get_init_expr()->accept(this));

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
    string cond_result = get_value_from_void<string>(while_->get_cond_expr()->accept(this)),
           body_result = get_value_from_void<string>(while_->get_body_expr()->accept(this));

    string result = "While(" 
                  + cond_result 
                  + ", " 
                  + body_result 
                  + ")";

    return new string(result);
}

void* Print_visitor::visit(If* if_){
    string cond_result = get_value_from_void<string>(if_->get_cond_expr()->accept(this)),
           then_result = get_value_from_void<string>(if_->get_then_expr()->accept(this)),
           else_result;

    if(if_->has_else_expr())
        else_result = get_value_from_void<string>(if_->get_else_expr()->accept(this));

    string result = "If(" 
                  + cond_result 
                  + ", " 
                  + then_result 
                  + ((if_->has_else_expr()) ? ", " + else_result : "")
                  + ")";

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
    string name = formal->get_name(),
           type = formal->get_type();

    string result = "" 
                  + name 
                  + " : " 
                  + type;

    return new string(result);
}

void* Print_visitor::visit(Block* block){
    List<Expr>* expr_list = block->get_expr_list();

    return new string(print_list(this, expr_list));
}

void* Print_visitor::visit(Call* call){
    call->get_line();
    return nullptr;
}

template <typename T>
T get_value_from_void(void* void_value){
    // Specifiate the type of the value
    T* value = (T*) void_value;

    // Give this value to a non pointer variable
    T return_value = *value;

    // Free the pointer
    delete value;

    // Return the value as non pointer
    return return_value;
}

template <typename T>
string print_list(Visitor* visitor, List<T>* list){
    size_t size = list->get_size();

    string result = "Block[";

    for (size_t i = 0; i < size; i++){
        string expr_result = get_value_from_void<string>(list->accept_one(visitor, i));

        result += expr_result;
        result += (i+1 == size) ? "]" : ", ";
    }

    return result;
}