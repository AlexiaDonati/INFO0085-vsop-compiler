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
enum UNOP {NOT, UNARY, ISNULL};

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
};

const map<string, UNOP> unop_to_enum = {
    {"not", NOT},
    {"-", UNARY},
    {"isnull", ISNULL},
};

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
    type::Table cond_expr_table = ACCEPT(if_->get_cond_expr());
    type::Table then_expr_table = ACCEPT(if_->get_then_expr());
    type::Table else_expr_table = ACCEPT(if_->get_else_expr());

    cond_expr_table.set_type(BOOLEAN);

    string then_type = then_expr_table.get_type();
    string else_type = then_expr_table.get_type();

    type::Table *returned_table;

    if(is_unit(then_type) || is_unit(else_type)){
        returned_table = new type::Table(UNIT);
    } else if(is_primitive(then_type) || is_primitive(else_type)) {
        then_expr_table.set_type(else_type);
        else_expr_table.set_type(then_type);
        returned_table = new type::Table(else_type);
    } else if(is_none(then_type) && is_none(else_type)){
        returned_table = new type::Table(NONE);
    } else {
        // TODO create common_class_type table
        returned_table = new type::Table(NONE); // mocked
    }

    returned_table->concatenate(&cond_expr_table);
    returned_table->concatenate(&then_expr_table);
    returned_table->concatenate(&else_expr_table);

    return returned_table;
}

void* Literals_visitor::visit(While* while_) {
    type::Table cond_expr_table = ACCEPT(while_->get_cond_expr());
    type::Table body_expr_table = ACCEPT(while_->get_body_expr());

    cond_expr_table.set_type(BOOLEAN);

    type::Table *returned_table = new type::Table(UNIT);

    returned_table->concatenate(&cond_expr_table);
    returned_table->concatenate(&body_expr_table);

    return returned_table;
}

void* Literals_visitor::visit(Let* let) {
    string variable_name = let->get_name();
    string variable_type = let->get_type();

    type::Table init_expr_table = ACCEPT(let->get_init_expr());
    type::Table scope_expr_table = ACCEPT(let->get_scope_expr());

    scope_expr_table.set_type(variable_name, variable_type);

    type::Table *returned_table = new type::Table(variable_type, variable_name);

    returned_table->concatenate(&init_expr_table);
    returned_table->concatenate(&scope_expr_table);

    returned_table->remove_type(variable_type);

    return returned_table;
}

void* Literals_visitor::visit(Assign* assign) {
    string variable_name = assign->get_name();
    type::Table expr_table = ACCEPT(assign->get_expr());

    expr_table.set_type(variable_name, expr_table.get_type());

    type::Table *returned_table = new type::Table(expr_table.get_type(), variable_name);

    returned_table->concatenate(&expr_table);

    return returned_table;
}

void* Literals_visitor::visit(Self* self) {
    // to remove the warning
    self->get_line();
    return new type::Table(SELF);
}

void* Literals_visitor::visit(Unop* unop) {
    string op = unop->get_op();
    type::Table expr_table = ACCEPT(unop->get_expr());

    type::Table *returned_table;

    switch (unop_to_enum.at(op))
    {
    case NOT:
        returned_table = new type::Table(BOOLEAN);

        expr_table.set_type(BOOLEAN);

        break;
    case UNARY:
        returned_table = new type::Table(INTEGER);

        expr_table.set_type(INTEGER);

        break;
    case ISNULL:
        returned_table = new type::Table(NONE);
        break;
    default:
        returned_table = new type::Table(NONE);

        returned_table->throw_error(unop, 
                "Unop not recognised: " 
                + op
            );

        break;
    }

    returned_table->concatenate(&expr_table);

    return returned_table;
}

void* Literals_visitor::visit(Binop* binop) {
    string op = binop->get_op();
    type::Table left_expr_table = ACCEPT(binop->get_left_expr());
    type::Table right_expr_table = ACCEPT(binop->get_right_expr());

    std::string left_type = left_expr_table.get_type();
    std::string right_type = right_expr_table.get_type();

    type::Table *returned_table;

    switch (binop_to_enum.at(op)){
    case EQ:   
        returned_table = new type::Table(BOOLEAN);

        // Both type must be the same if primitive
        if(is_primitive(left_type) || is_primitive(right_type)){
            right_expr_table.set_type(left_type);
            left_expr_table.set_type(right_type);
        }

        break;
    case LT:
    case LEQ:
    case ADD:
    case SUB:
    case MUL:
    case DIV:
    case POW:
        if(binop_to_enum.at(op) == LT && binop_to_enum.at(op) == LEQ)
            returned_table = new type::Table(BOOLEAN);
        else
            returned_table = new type::Table(INTEGER);

        left_expr_table.set_type(INTEGER);
        right_expr_table.set_type(INTEGER);

        break;
    case AND:
        returned_table = new type::Table(BOOLEAN);

        left_expr_table.set_type(BOOLEAN);
        right_expr_table.set_type(BOOLEAN);
        
        break;
    default:
        returned_table = new type::Table(NONE);

        returned_table->throw_error(binop, 
                "Binop not recognised: " 
                + op
            );

        break;
    }

    returned_table->concatenate(&left_expr_table);
    returned_table->concatenate(&right_expr_table);

    return returned_table;
}

void* Literals_visitor::visit(Call* call) {
    string object = ACCEPT(call->get_object()).get_type();
    string method = call->get_method();
    
    type::Table *returned_table = new type::Table(NONE, object, method);

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
        && is_string(type);
}