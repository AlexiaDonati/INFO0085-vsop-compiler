#include "semantics_expressions.hpp"

using namespace AST;
using namespace std;

#define TO_VALUE(void_pointer) get_value_from_void(void_pointer)
#define TO_VOID(value) get_void_from_value(value)
#define ACCEPT(expr) (type::Table*) expr->accept(this)
#define ACCEPT_LIST(list) accept_list(list)

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
    vector<type::Table*> class_list_tables = ACCEPT_LIST(program->get_class_list());

    type::Table *returned_table = new type::Table(S_NONE);

    for(size_t i = 0; i < class_list_tables.size(); i++)
        returned_table->concatenate(class_list_tables[i]);

    return returned_table;
}

void* Literals_visitor::visit(Class* class_) {
    string name = class_->get_name();
    string parent_type = class_->get_parent();
    vector<type::Table*> field_list_tables = ACCEPT_LIST(class_->get_field_list());
    vector<type::Table*> method_list_tables = ACCEPT_LIST(class_->get_method_list());

    type::Table *returned_table = new type::Table(name);

    for(size_t i = 0; i < method_list_tables.size(); i++)
        returned_table->concatenate(method_list_tables[i]);

    for(size_t i = 0; i < field_list_tables.size(); i++){
        returned_table->concatenate(field_list_tables[i]);
        // Class variables must be removed from the table
        returned_table->remove_type(field_list_tables[i]->get_return_variable_name());
    }

    returned_table->replace_self_by_name(name);
    return returned_table;
}

void* Literals_visitor::visit(Field* field) {
    string name = field->get_name();
    string type = field->get_type();
    type::Table *init_expr_table = new type::Table(S_NONE);
    
    if(field->has_init_expr())
        init_expr_table = ACCEPT(field->get_init_expr());

    type::Table *returned_table = new type::Table(type, name);

    returned_table->set_type(init_expr_table->get_type());
    
    returned_table->concatenate(init_expr_table);

    delete init_expr_table;

    return returned_table;
}

void* Literals_visitor::visit(Method* method) {
    string return_type = method->get_return_type();
    string name = method->get_name();
    type::Table *body_block_table = ACCEPT(method->get_body_block());
    vector<type::Table*> formal_list_tables = ACCEPT_LIST(method->get_formal_list());

    type::Table *returned_table = new type::Table(return_type, name, S_SELF);

    body_block_table->set_type(return_type);
    returned_table->concatenate(body_block_table);

    for(size_t i = 0; i < formal_list_tables.size(); i++){
        returned_table->concatenate(formal_list_tables[i]);
        // Method arguments must be removed from the table
        returned_table->remove_type(formal_list_tables[i]->get_return_variable_name());
    }

    delete body_block_table;

    return returned_table;
}

void* Literals_visitor::visit(Formal* formal) {
    string type = formal->get_type();
    string name = formal->get_name();

    type::Table *returned_table = new type::Table(type, name);

    return returned_table;
}

void* Literals_visitor::visit(Block* block) {
    vector<type::Table*> expr_list_tables = ACCEPT_LIST(block->get_expr_list());

    string last_type = expr_list_tables[expr_list_tables.size()-1]->get_type(); 

    type::Table *returned_table = new type::Table(last_type);

    for(size_t i = 0; i < expr_list_tables.size(); i++)
        returned_table->concatenate(expr_list_tables[i]);

    return returned_table;
}

void* Literals_visitor::visit(If* if_) {
    type::Table *cond_expr_table = ACCEPT(if_->get_cond_expr());
    type::Table *then_expr_table = ACCEPT(if_->get_then_expr());
    type::Table *else_expr_table = new type::Table(S_NONE);

    if(if_->has_else_expr())
        else_expr_table = ACCEPT(if_->get_else_expr());

    cond_expr_table->set_type(S_BOOLEAN);

    string then_type = then_expr_table->get_type();
    string else_type = else_expr_table->get_type();

    type::Table *returned_table;

    if(is_unit(then_type) || is_unit(else_type)){
        returned_table = new type::Table(S_UNIT);
    } else if(is_primitive(then_type) || is_primitive(else_type)) {
        then_expr_table->set_type(else_type);
        else_expr_table->set_type(then_type);
        returned_table = new type::Table(else_type);
    } else if(is_none(then_type) && is_none(else_type)){
        returned_table = new type::Table(S_NONE);
    } else {
        // TODO create common_class_type table
        returned_table = new type::Table(S_NONE); // mocked
    }

    returned_table->concatenate(cond_expr_table);
    returned_table->concatenate(then_expr_table);
    returned_table->concatenate(else_expr_table);

    delete cond_expr_table;
    delete then_expr_table;
    delete else_expr_table;

    return returned_table;
}

void* Literals_visitor::visit(While* while_) {
    type::Table *cond_expr_table = ACCEPT(while_->get_cond_expr());
    type::Table *body_expr_table = ACCEPT(while_->get_body_expr());

    cond_expr_table->set_type(S_BOOLEAN);

    type::Table *returned_table = new type::Table(S_UNIT);

    returned_table->concatenate(cond_expr_table);
    returned_table->concatenate(body_expr_table);

    delete cond_expr_table;
    delete body_expr_table;

    return returned_table;
}

void* Literals_visitor::visit(Let* let) {
    string variable_name = let->get_name();
    string variable_type = let->get_type();

    type::Table *scope_expr_table = ACCEPT(let->get_scope_expr());
    type::Table *init_expr_table = new type::Table(S_NONE);
    if(let->has_init_expr())
        init_expr_table = ACCEPT(let->get_init_expr());

    scope_expr_table->set_type(variable_name, variable_type);

    type::Table *returned_table = new type::Table(variable_type);

    returned_table->concatenate(init_expr_table);
    returned_table->concatenate(scope_expr_table);

    returned_table->remove_type(variable_type);

    delete init_expr_table;
    delete scope_expr_table;

    return returned_table;
}

void* Literals_visitor::visit(Assign* assign) {
    string variable_name = assign->get_name();
    type::Table *expr_table = ACCEPT(assign->get_expr());

    expr_table->set_type(variable_name, expr_table->get_type());

    type::Table *returned_table = new type::Table(expr_table->get_type(), variable_name);

    returned_table->concatenate(expr_table);

    delete expr_table;

    return returned_table;
}

void* Literals_visitor::visit(Self* self) {
    // to remove the warning
    self->get_line();
    return new type::Table(S_SELF);
}

void* Literals_visitor::visit(Unop* unop) {
    string op = unop->get_op();
    type::Table *expr_table = ACCEPT(unop->get_expr());

    type::Table *returned_table;

    switch (unop_to_enum.at(op))
    {
    case NOT:
        returned_table = new type::Table(S_BOOLEAN);

        expr_table->set_type(S_BOOLEAN);

        break;
    case UNARY:
        returned_table = new type::Table(S_INTEGER);

        expr_table->set_type(S_INTEGER);

        break;
    case ISNULL:
        returned_table = new type::Table(S_NONE);
        break;
    default:
        returned_table = new type::Table(S_NONE);

        returned_table->throw_error(unop, 
                "Unop not recognised: " 
                + op
            );

        break;
    }

    returned_table->concatenate(expr_table);
    delete expr_table;
    return returned_table;
}

void* Literals_visitor::visit(Binop* binop) {
    string op = binop->get_op();
    type::Table *left_expr_table = ACCEPT(binop->get_left_expr());
    type::Table *right_expr_table = ACCEPT(binop->get_right_expr());

    string left_type = left_expr_table->get_type();
    string right_type = right_expr_table->get_type();

    type::Table *returned_table;

    switch (binop_to_enum.at(op)){
    case EQ:   
        returned_table = new type::Table(S_BOOLEAN);

        // Both type must be the same if primitive
        if(is_primitive(left_type) || is_primitive(right_type)){
            right_expr_table->set_type(left_type);
            left_expr_table->set_type(right_type);
        }

        break;
    case LT:
    case LEQ:
    case ADD:
    case SUB:
    case MUL:
    case DIV:
    case POW:
        if(binop_to_enum.at(op) == LT || binop_to_enum.at(op) == LEQ)
            returned_table = new type::Table(S_BOOLEAN);
        else
            returned_table = new type::Table(S_INTEGER);

        left_expr_table->set_type(S_INTEGER);
        right_expr_table->set_type(S_INTEGER);

        break;
    case AND:
        returned_table = new type::Table(S_BOOLEAN);

        left_expr_table->set_type(S_BOOLEAN);
        right_expr_table->set_type(S_BOOLEAN);
        
        break;
    default:
        returned_table = new type::Table(S_NONE);

        returned_table->throw_error(binop, 
                "Binop not recognised: " 
                + op
            );

        break;
    }

    returned_table->concatenate(left_expr_table);
    returned_table->concatenate(right_expr_table);

    delete left_expr_table;
    delete right_expr_table;

    return returned_table;
}

void* Literals_visitor::visit(Call* call) {
    type::Table *object_table = ACCEPT(call->get_object());
    string object = object_table->get_type();
    string method = call->get_method();
    vector<type::Table*> arg_expr_list_tables = ACCEPT_LIST(call->get_arg_expr_list());
    
    type::Table *returned_table = new type::Table(S_NONE, method, object);
    
    for(size_t i = 0; i < arg_expr_list_tables.size(); i++)
        returned_table->concatenate(arg_expr_list_tables[i]);

    delete object_table;

    return returned_table;
}

void* Literals_visitor::visit(New* new_) {
    return new type::Table(new_->get_type());
}

void* Literals_visitor::visit(String* string_) {
    // to remove the warning
    string_->get_line();
    return new type::Table(S_STRING);
}

void* Literals_visitor::visit(Integer* integer) {
    // to remove the warning
    integer->get_line();
    return new type::Table(S_INTEGER);
}

void* Literals_visitor::visit(Boolean* boolean) {
    // to remove the warning
    boolean->get_line();
    return new type::Table(S_BOOLEAN);
}

void* Literals_visitor::visit(Unit* unit) {
    // to remove the warning
    unit->get_line();
    return new type::Table(S_UNIT);
}

void* Literals_visitor::visit(Object* object) {
    return new type::Table(S_NONE, object->get_name());
}

/* Local Functions */

bool is_unit(string type){
    return type == S_UNIT;
}

bool is_boolean(string type){
    return type == S_BOOLEAN;
}

bool is_integer(string type){
    return type == S_INTEGER;
}

bool is_string(string type){
    return type == S_STRING;
}

bool is_none(string type){
    return type == S_NONE;
}

bool is_primitive(string type){
    return is_unit(type)
        || is_boolean(type)
        || is_integer(type)
        || is_string(type);
}