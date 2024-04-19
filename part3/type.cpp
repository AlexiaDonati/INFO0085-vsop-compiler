#include "type.hpp"
#include "ast.hpp"

using namespace AST;
using namespace AST::type;

void Table::throw_error(Expr *expr, std::string message) { 
    Error *new_error = new Error(
        expr->get_line(),
        expr->get_column(),
        expr->get_file_name(),
        message
    );
    error_list.push_back(new_error); 
}

std::string Table::type_to_string(std::string name) {
    std::string result = get_type(name);

    if(result == S_TYPE_NONE)
        return S_TYPE_OBJECT;
    
    return result;
}

std::string Table::type_to_string(std::string method_name, std::string object_name) {
    std::string result = get_type(method_name, object_name);

    if(result == S_TYPE_NONE)
        return S_TYPE_OBJECT;
    
    return result;
}

std::string Table::type_to_string() { 
    std::string result = get_type();

    if(result == S_TYPE_NONE)
        return S_TYPE_OBJECT;
    
    return result;
}

bool Table::is_return_a_variable() {
    if(return_variable == NULL)
        return false;
    return true;
}

bool Table::is_return_a_dispatch() {
    if(return_dispatch == NULL)
        return false;
    return true;
}

std::string Table::get_return_variable_name() { 
    if(return_variable == NULL)
        return S_TYPE_NONE;
    return return_variable->name;
}

std::string Table::get_return_dispatch_object_name() { 
    if(return_dispatch == NULL)
        return S_TYPE_NONE;
    return return_dispatch->object_name;
}

std::string Table::get_return_dispatch_method_name() { 
    if(return_dispatch == NULL)
        return S_TYPE_NONE;
    return return_dispatch->method_name;
}

void Table::update_children(std::string name, std::string type){
    std::string previous_type = get_type(name);

    if(type == S_TYPE_NONE)
        return;

    if(return_variable != NULL && get_type() == S_TYPE_NONE 
    && return_variable->name == name)
        return_type = type;

    Variable* new_variable = new Variable(name);

    // Delete previous stored variable if existing
    remove_type(name);

    v_table.insert({new_variable, type});

    for (auto table : children)
        table->update_children(name, type);
}

void Table::update_children(std::string method_name, std::string object_name, std::string type){
    std::string previous_type = get_type(method_name, object_name);

    if(type == S_TYPE_NONE)
        return;

    if(return_dispatch != NULL && get_type() == S_TYPE_NONE 
    && return_dispatch->method_name == method_name && return_dispatch->object_name == object_name)
        return_type = type;

    Dispatch* new_dispatch = new Dispatch(method_name, object_name);

    // Delete previous stored dispatch if existing
    remove_type(method_name, object_name);

    d_table.insert({new_dispatch, type});

    for (auto table : children)
        table->update_children(method_name, object_name, type);
}