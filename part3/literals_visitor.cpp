#include "semantics_expressions.hpp"

using namespace AST;
using namespace std;

#define T_ACCEPT(expr) (type::Table*) expr->accept(this)
#define T_ACCEPT_LIST(list) accept_list(list)
#define LOC(expr) expr->get_line(), expr->get_column(), expr->get_file_name(), expr

bool iS_TYPE_UNIT(std::string type);
bool iS_TYPE_BOOLEAN(std::string type);
bool iS_TYPE_INTEGER(std::string type);
bool iS_TYPE_STRING(std::string type);
bool iS_TYPE_NONE(std::string type);
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
    vector<type::Table*> class_list_tables = T_ACCEPT_LIST(program->get_class_list());

    type::Table *returned_table = new type::Table(LOC(program), S_TYPE_NONE);

    for(size_t i = 0; i < class_list_tables.size(); i++){
        returned_table->concatenate(class_list_tables[i]);
        returned_table->add_child(class_list_tables[i]);
    }

    returned_table->v_table_must_be_empty();

    returned_table->update_children();

    return returned_table;
}

void* Literals_visitor::visit(Class* class_) {
    string name = class_->get_name();
    string parent_type = class_->get_parent();
    vector<type::Table*> field_list_tables = T_ACCEPT_LIST(class_->get_field_list());
    vector<type::Table*> method_list_tables = T_ACCEPT_LIST(class_->get_method_list());

    type::Table *returned_table = new type::Table(LOC(class_), name, S_TYPE_SELF);

    for(size_t i = 0; i < method_list_tables.size(); i++){
        returned_table->concatenate(method_list_tables[i]);
        returned_table->add_child(method_list_tables[i]);
    }

    for(size_t i = 0; i < field_list_tables.size(); i++){
        returned_table->concatenate(field_list_tables[i]);
        returned_table->add_child(field_list_tables[i]);
    }

    returned_table->update_children();

    // Class variables must be removed from the tab
    for(size_t i = 0; i < field_list_tables.size(); i++)
        returned_table->remove_type(field_list_tables[i]->get_return_variable_name());
    returned_table->remove_type(S_TYPE_SELF);
    
    return returned_table;
}

void* Literals_visitor::visit(Field* field) {
    string name = field->get_name();
    string type = field->get_type();
    type::Table *init_expr_table = new type::Table(LOC(field), S_TYPE_NONE);
    
    if(field->has_init_expr()){
        delete init_expr_table;
        init_expr_table = T_ACCEPT(field->get_init_expr());
    }

    type::Table *returned_table = new type::Table(LOC(field), type, name);

    returned_table->set_type(init_expr_table->get_type());
    
    returned_table->concatenate(init_expr_table);

    returned_table->add_child(init_expr_table);

    return returned_table;
}

void* Literals_visitor::visit(Method* method) {
    string return_type = method->get_return_type();
    string name = method->get_name();
    type::Table *body_block_table = T_ACCEPT(method->get_body_block());
    vector<type::Table*> formal_list_tables = T_ACCEPT_LIST(method->get_formal_list());

    type::Table *returned_table = new type::Table(LOC(method), return_type, name, S_TYPE_SELF);

    body_block_table->set_type(return_type);
    returned_table->concatenate(body_block_table);

    for(size_t i = 0; i < formal_list_tables.size(); i++){
        returned_table->concatenate(formal_list_tables[i]);
        returned_table->add_child(formal_list_tables[i]);
    }

    returned_table->add_child(body_block_table);

    returned_table->update_children();

    // Method arguments must be removed from the table
    for(size_t i = 0; i < formal_list_tables.size(); i++)
        returned_table->remove_type(formal_list_tables[i]->get_return_variable_name());

    return returned_table;
}

void* Literals_visitor::visit(Formal* formal) {
    string type = formal->get_type();
    string name = formal->get_name();

    type::Table *returned_table = new type::Table(LOC(formal), type, name);

    return returned_table;
}

void* Literals_visitor::visit(Block* block) {
    vector<type::Table*> expr_list_tables = T_ACCEPT_LIST(block->get_expr_list());

    type::Table* last_table = expr_list_tables[expr_list_tables.size()-1]; 

    type::Table *returned_table = new type::Table(LOC(block), last_table->get_type());

    if(last_table->is_return_a_dispatch()){
        delete returned_table;
        returned_table = new type::Table(LOC(block)
                                        , last_table->get_type()
                                        , last_table->get_return_dispatch_method_name()
                                        , last_table->get_return_dispatch_object_name());
    } else if(last_table->is_return_a_variable()){
        delete returned_table;
        returned_table = new type::Table(LOC(block)
                                        , last_table->get_type()
                                        , last_table->get_return_variable_name());
    }


    for(size_t i = 0; i < expr_list_tables.size(); i++){
        returned_table->concatenate(expr_list_tables[i]);
        returned_table->add_child(expr_list_tables[i]);
    }

    return returned_table;
}

void* Literals_visitor::visit(If* if_) {
    type::Table *cond_expr_table = T_ACCEPT(if_->get_cond_expr());
    type::Table *then_expr_table = T_ACCEPT(if_->get_then_expr());
    type::Table *else_expr_table = new type::Table(LOC(if_), S_TYPE_NONE);

    if(if_->has_else_expr()){
        delete else_expr_table;
        else_expr_table = T_ACCEPT(if_->get_else_expr());
    }

    cond_expr_table->set_type(S_TYPE_BOOLEAN);

    string then_type = then_expr_table->get_type();
    string else_type = else_expr_table->get_type();

    type::Table *returned_table;

    if(iS_TYPE_UNIT(then_type) || iS_TYPE_UNIT(else_type)){
        returned_table = new type::Table(LOC(if_), S_TYPE_UNIT);
    } else if(is_primitive(then_type) || is_primitive(else_type)) {
        then_expr_table->set_type(else_type);
        else_expr_table->set_type(then_type);
        returned_table = new type::Table(LOC(if_), else_type);
    } else if(iS_TYPE_NONE(then_type) && iS_TYPE_NONE(else_type)){
        returned_table = new type::Table(LOC(if_), S_TYPE_NONE);
    } else {
        // TODO create common_class_type table
        returned_table = new type::Table(LOC(if_), S_TYPE_NONE); // mocked
    }

    returned_table->concatenate(cond_expr_table);
    returned_table->concatenate(then_expr_table);
    returned_table->concatenate(else_expr_table);

    returned_table->add_child(cond_expr_table);
    returned_table->add_child(then_expr_table);
    returned_table->add_child(else_expr_table);

    return returned_table;
}

void* Literals_visitor::visit(While* while_) {
    type::Table *cond_expr_table = T_ACCEPT(while_->get_cond_expr());
    type::Table *body_expr_table = T_ACCEPT(while_->get_body_expr());

    cond_expr_table->set_type(S_TYPE_BOOLEAN);

    type::Table *returned_table = new type::Table(LOC(while_), S_TYPE_UNIT);

    returned_table->concatenate(cond_expr_table);
    returned_table->concatenate(body_expr_table);

    returned_table->add_child(cond_expr_table);
    returned_table->add_child(body_expr_table);

    return returned_table;
}

void* Literals_visitor::visit(Let* let) {
    string variable_name = let->get_name();
    string variable_type = let->get_type();

    type::Table *scope_expr_table = T_ACCEPT(let->get_scope_expr());
    type::Table *init_expr_table = new type::Table(LOC(let), S_TYPE_NONE);

    if(let->has_init_expr()){
        delete init_expr_table;
        init_expr_table = T_ACCEPT(let->get_init_expr());
    }

    scope_expr_table->set_type(variable_name, variable_type);

    type::Table *returned_table;
    if(scope_expr_table->is_return_a_variable())
        returned_table = new type::Table(LOC(let), (scope_expr_table->get_return_variable_name() == variable_name) ? variable_type : scope_expr_table->get_type()
                                                , scope_expr_table->get_return_variable_name());
    else if(scope_expr_table->is_return_a_dispatch())
        returned_table = new type::Table(LOC(let), scope_expr_table->get_type()
                                                , scope_expr_table->get_return_dispatch_method_name()
                                                , scope_expr_table->get_return_dispatch_object_name());
    else
        returned_table = new type::Table(LOC(let), scope_expr_table->get_type());
        

    returned_table->concatenate(init_expr_table);
    returned_table->concatenate(scope_expr_table);

    returned_table->add_child(init_expr_table);
    returned_table->add_child(scope_expr_table);

    returned_table->update_children();
    returned_table->remove_type(variable_name);

    return returned_table;
}

void* Literals_visitor::visit(Assign* assign) {
    string variable_name = assign->get_name();
    type::Table *expr_table = T_ACCEPT(assign->get_expr());

    expr_table->set_type(variable_name, expr_table->get_type());

    type::Table *returned_table = new type::Table(LOC(assign), expr_table->get_type(), variable_name);

    returned_table->concatenate(expr_table);

    returned_table->add_child(expr_table);

    return returned_table;
}

void* Literals_visitor::visit(Self* self) {
    // to remove the warning
    self->get_line();
    return new type::Table(LOC(self), S_TYPE_NONE, S_TYPE_SELF);
}

void* Literals_visitor::visit(Unop* unop) {
    string op = unop->get_op();
    type::Table *expr_table = T_ACCEPT(unop->get_expr());

    type::Table *returned_table;

    switch (unop_to_enum.at(op))
    {
    case NOT:
        returned_table = new type::Table(LOC(unop), S_TYPE_BOOLEAN);

        expr_table->set_type(S_TYPE_BOOLEAN);

        break;
    case UNARY:
        returned_table = new type::Table(LOC(unop), S_TYPE_INTEGER);

        expr_table->set_type(S_TYPE_INTEGER);

        break;
    case ISNULL:
        returned_table = new type::Table(LOC(unop), S_TYPE_NONE);
        break;
    default:
        returned_table = new type::Table(LOC(unop), S_TYPE_NONE);

        returned_table->throw_error(unop, 
                "Unop not recognised: " 
                + op
            );

        break;
    }

    returned_table->concatenate(expr_table);

    returned_table->add_child(expr_table);

    return returned_table;
}

void* Literals_visitor::visit(Binop* binop) {
    string op = binop->get_op();
    type::Table *left_expr_table = T_ACCEPT(binop->get_left_expr());
    type::Table *right_expr_table = T_ACCEPT(binop->get_right_expr());

    string left_type = left_expr_table->get_type();
    string right_type = right_expr_table->get_type();

    type::Table *returned_table;

    switch (binop_to_enum.at(op)){
    case EQ:   
        returned_table = new type::Table(LOC(binop), S_TYPE_BOOLEAN);

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
            returned_table = new type::Table(LOC(binop), S_TYPE_BOOLEAN);
        else
            returned_table = new type::Table(LOC(binop), S_TYPE_INTEGER);

        left_expr_table->set_type(S_TYPE_INTEGER);
        right_expr_table->set_type(S_TYPE_INTEGER);

        break;
    case AND:
        returned_table = new type::Table(LOC(binop), S_TYPE_BOOLEAN);

        left_expr_table->set_type(S_TYPE_BOOLEAN);
        right_expr_table->set_type(S_TYPE_BOOLEAN);
        
        break;
    default:
        returned_table = new type::Table(LOC(binop), S_TYPE_NONE);

        returned_table->throw_error(binop, 
                "Binop not recognised: " 
                + op
            );

        break;
    }

    returned_table->concatenate(left_expr_table);
    returned_table->concatenate(right_expr_table);

    returned_table->add_child(left_expr_table);
    returned_table->add_child(right_expr_table);

    return returned_table;
}

void* Literals_visitor::visit(Call* call) {
    type::Table *object_table = T_ACCEPT(call->get_object());
    string object = object_table->get_type();
    if(object == S_TYPE_NONE)
        object = object_table->get_return_variable_name();
    string method = call->get_method();
    vector<type::Table*> arg_expr_list_tables = T_ACCEPT_LIST(call->get_arg_expr_list());
    
    type::Table *returned_table = new type::Table(LOC(call), S_TYPE_NONE, method, object);
    returned_table->concatenate(object_table);
    
    for(size_t i = 0; i < arg_expr_list_tables.size(); i++){
        returned_table->concatenate(arg_expr_list_tables[i]);
        returned_table->add_child(arg_expr_list_tables[i]);
    }

    returned_table->add_child(object_table);

    return returned_table;
}

void* Literals_visitor::visit(New* new_) {
    return new type::Table(LOC(new_), new_->get_type());
}

void* Literals_visitor::visit(String* string_) {
    // to remove the warning
    string_->get_line();
    return new type::Table(LOC(string_), S_TYPE_STRING);
}

void* Literals_visitor::visit(Integer* integer) {
    // to remove the warning
    integer->get_line();
    return new type::Table(LOC(integer), S_TYPE_INTEGER);
}

void* Literals_visitor::visit(Boolean* boolean) {
    // to remove the warning
    boolean->get_line();
    return new type::Table(LOC(boolean), S_TYPE_BOOLEAN);
}

void* Literals_visitor::visit(Unit* unit) {
    // to remove the warning
    unit->get_line();
    return new type::Table(LOC(unit), S_TYPE_UNIT);
}

void* Literals_visitor::visit(Object* object) {
    return new type::Table(LOC(object), S_TYPE_NONE, object->get_name());
}

/* Local Functions */

bool iS_TYPE_UNIT(string type){
    return type == S_TYPE_UNIT;
}

bool iS_TYPE_BOOLEAN(string type){
    return type == S_TYPE_BOOLEAN;
}

bool iS_TYPE_INTEGER(string type){
    return type == S_TYPE_INTEGER;
}

bool iS_TYPE_STRING(string type){
    return type == S_TYPE_STRING;
}

bool iS_TYPE_NONE(string type){
    return type == S_TYPE_NONE;
}

bool is_primitive(string type){
    return iS_TYPE_UNIT(type)
        || iS_TYPE_BOOLEAN(type)
        || iS_TYPE_INTEGER(type)
        || iS_TYPE_STRING(type);
}