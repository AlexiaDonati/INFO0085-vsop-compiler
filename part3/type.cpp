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

void Table::set_type(std::string name, std::string type){
    std::string previous_type = get_type(name);

    if(previous_type != S_TYPE_NONE && type == S_TYPE_NONE)
        return;
    if(previous_type != S_TYPE_NONE && previous_type != type)
        throw_error("variable " + name + " have different types " + previous_type + " and " + type);

    Variable* new_variable = new Variable(name);

    // Delete previous stored variable if existing
    remove_type(name);

    v_table.insert({new_variable, type});
}

void Table::set_type(std::string method_name, std::string object_name, std::string type){
    std::string previous_type = get_type(method_name, object_name);

    if(previous_type != S_TYPE_NONE && type == S_TYPE_NONE)
        return;
    if(previous_type != S_TYPE_NONE && previous_type != type)
        throw_error("dispatch " + object_name + "." + method_name + " have different types " + previous_type + " and " + type);

    Dispatch* new_dispatch = new Dispatch(method_name, object_name);

    // Delete previous stored dispatch if existing
    remove_type(method_name, object_name);

    d_table.insert({new_dispatch, type});
}

void Table::set_type(std::string type){
    std::string previous_type = return_type;

    if(previous_type != S_TYPE_NONE && type == S_TYPE_NONE)
        return;
    if(previous_type != S_TYPE_NONE && previous_type != type)
        throw_error("Return_type have different types " + previous_type + " and " + type);

    if(return_variable != NULL){
        return_type = type;
        set_type(return_variable->name, type);
    }
    if(return_dispatch != NULL){
        return_type = type;
        set_type(return_dispatch->method_name, return_dispatch->object_name, type);
    }
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

bool Table::is_return(std::string name) {
    if(return_variable != NULL 
    && return_variable->name == name)
        return true;
    return false;
}

bool Table::is_return(std::string method_name, std::string object_name) {
    if(return_dispatch != NULL 
    && return_dispatch->method_name == method_name 
    && return_dispatch->object_name == object_name)
        return true;
    return false;
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

void Table::replace_object_by_name(std::string old_name, std::string new_name){
    std::map<Dispatch*, std::string> to_remove;

    for(auto it = d_table.begin(); it != d_table.end(); it++){
        std::string method = it->first->method_name;
        std::string object = it->first->object_name;
        std::string return_type = it->second;

        if(object == old_name)
            to_remove.insert({it->first, it->second});
    }

    for(auto it = to_remove.begin(); it != to_remove.end(); it++){
        std::string method = it->first->method_name;
        std::string object = it->first->object_name;
        std::string return_type = it->second;

        if(is_return(method, object)){
            delete return_dispatch;
            return_dispatch = new Dispatch(method, new_name);
        }

        remove_type(method, object);
        set_type(method, new_name, return_type);
    }

    replace_object_by_name_in_children(old_name, new_name);
}

void Table::replace_object_by_name_in_children(std::string old_name, std::string new_name){
    for (auto table : children)
        table->replace_object_by_name(old_name, new_name);
}

void Table::update_children(std::string name, std::string type){
    std::string previous_type = get_type(name);

    if(type == S_TYPE_NONE)
        return;

    if(is_return(name) && get_type() == S_TYPE_NONE)
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

    if(is_return(method_name, object_name) && get_type() == S_TYPE_NONE)
        return_type = type;

    Dispatch* new_dispatch = new Dispatch(method_name, object_name);

    // Delete previous stored dispatch if existing
    remove_type(method_name, object_name);

    d_table.insert({new_dispatch, type});

    for (auto table : children)
        table->update_children(method_name, object_name, type);
}