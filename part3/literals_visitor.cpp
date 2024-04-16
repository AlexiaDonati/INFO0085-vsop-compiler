#include "semantics_expressions.hpp"

using namespace AST;
using namespace std;

bool is_unit(std::string type);
bool is_boolean(std::string type);
bool is_integer(std::string type);
bool is_string(std::string type);
bool is_none(std::string type);
bool is_primitive(std::string type);

enum BINOP {EQ, LT, LEQ, ADD, SUB, MUL, DIV, POW, AND};

const map<string, BINOP> binop_to_enum = {
    {"=", EQ},
    {"<", LT},
    {"<=", LEQ},
    {"+", ADD},
    {"-", SUB},
    {"*", MUL},
    {"/", DIV},
    {"^", POW},
    {"and", AND},
} ;

void* Literals_visitor::visit(Program* program) {
    // to remove the warning
    program->get_line();
    return NULL;
}

void* Literals_visitor::visit(Class* class_) {
    // to remove the warning
    class_->get_line();
    return NULL;
}

void* Literals_visitor::visit(Field* field) {
    // to remove the warning
    field->get_line();
    return NULL;
}

void* Literals_visitor::visit(Method* method) {
    // to remove the warning
    method->get_line();
    return NULL;
}

void* Literals_visitor::visit(Formal* formal) {
    // to remove the warning
    formal->get_line();
    return NULL;
}

void* Literals_visitor::visit(Block* block) {
    // to remove the warning
    block->get_line();
    return NULL;
}

void* Literals_visitor::visit(If* if_) {
    // to remove the warning
    if_->get_line();
    return NULL;
}

void* Literals_visitor::visit(While* while_) {
    // to remove the warning
    while_->get_line();
    return NULL;
}

void* Literals_visitor::visit(Let* let) {
    // to remove the warning
    let->get_line();
    return NULL;
}

void* Literals_visitor::visit(Assign* assign) {
    // to remove the warning
    assign->get_line();
    return NULL;
}

void* Literals_visitor::visit(Self* self) {
    // to remove the warning
    self->get_line();
    return NULL;
}

void* Literals_visitor::visit(Unop* unop) {
    // to remove the warning
    unop->get_line();
    return NULL;
}

void* Literals_visitor::visit(Binop* binop) {
    string op = binop->get_op();
    type::Table left_expr_table = ACCEPT(binop->get_left_expr());
    type::Table right_expr_table = ACCEPT(binop->get_right_expr());

    type::Table *returned_table;

    switch (binop_to_enum.at(op)){
    case EQ:   
        returned_table = new type::Table(BOOLEAN);

        if(is_primitive(left_expr_table.get_return_type()) != is_primitive(right_expr_table.get_return_type()))
            returned_table->throw_error(binop, 
                "Both types must be classes: " 
                + left_expr_table.get_return_type()
                + op
                + right_expr_table.get_return_type()
            );

        if(left_expr_table.get_return_type() != right_expr_table.get_return_type())
            returned_table->throw_error(binop, 
                "Both types must be the same: " 
                + left_expr_table.get_return_type()
                + op
                + right_expr_table.get_return_type()
            );

        break;
    case LT:
    case LEQ:
        returned_table = new type::Table(BOOLEAN);

        if(is_none(left_expr_table.get_return_type()))
            left_expr_table.set_type(INTEGER);
        if(is_none(right_expr_table.get_return_type()))
            right_expr_table.set_type(INTEGER);

        if(!is_integer(left_expr_table.get_return_type())
        && !is_integer(right_expr_table.get_return_type()))
            returned_table->throw_error(binop, 
                "Both types must be integer: " 
                + left_expr_table.get_return_type()
                + op
                + right_expr_table.get_return_type()
            );
        
        break;
    case ADD:
    case SUB:
    case MUL:
    case DIV:
    case POW:
        break;
    case AND:
        returned_table = new type::Table(BOOLEAN);

        if(is_none(left_expr_table.get_return_type()))
            left_expr_table.set_type(BOOLEAN);
        if(is_none(right_expr_table.get_return_type()))
            right_expr_table.set_type(BOOLEAN);

        if(!is_boolean(left_expr_table.get_return_type())
        && !is_boolean(right_expr_table.get_return_type()))
            returned_table->throw_error(binop, 
                "Both types must be integer: " 
                + left_expr_table.get_return_type()
                + op
                + right_expr_table.get_return_type()
            );
        
        break;
    default:
        break;
    }

    return NULL;
}

void* Literals_visitor::visit(Call* call) {
    string object = ACCEPT(call->get_object()).get_return_type();
    string method = call->get_method();
    
    type::Table *returned_table = new type::Table(NONE);

    returned_table->set_type(object, method, NONE);

    return returned_table;
}

void* Literals_visitor::visit(New* new_) {
    return new type::Table(new_->get_type());
}

void* Literals_visitor::visit(String* string_) {
    // to remove the warning
    string_->get_line();
    return new type::Table(STRING);
}

void* Literals_visitor::visit(Integer* integer) {
    // to remove the warning
    integer->get_line();
    return new type::Table(INTEGER);
}

void* Literals_visitor::visit(Boolean* boolean) {
    // to remove the warning
    boolean->get_line();
    return new type::Table(BOOLEAN);
}

void* Literals_visitor::visit(Unit* unit) {
    // to remove the warning
    unit->get_line();
    return new type::Table(UNIT);
}

void* Literals_visitor::visit(Object* object) {
    return new type::Table(object->get_name());
}

/* Local Functions */

bool is_unit(std::string type){
    return type == UNIT;
}

bool is_boolean(std::string type){
    return type == BOOLEAN;
}

bool is_integer(std::string type){
    return type == INTEGER;
}

bool is_string(std::string type){
    return type == STRING;
}

bool is_none(std::string type){
    return type == NONE;
}

bool is_primitive(std::string type){
    return is_unit(type)
        && is_boolean(type)
        && is_integer(type)
        && is_string(type)
        && is_none(type);
}