#include "type.hpp"
#include "ast.hpp"
#include "semantics_expressions.hpp"

using namespace AST;
using namespace AST::type;

bool Table::is_type_exist(std::string type){
    if(is_primitive(type) 
    || type == S_TYPE_NONE 
    || type == S_TYPE_SELF 
    || type == S_TYPE_OBJECT 
    || Literals_visitor::is_child_of(type, S_TYPE_OBJECT))
    return true;
    return false;
}

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

std::string Table::to_string(){
    std::string result = "";

    result += "---- Return type ---\n";

    result += return_type + "\n";

    if(is_return_a_variable())
        result += "> " + get_return_variable_name() + "\n";
    else if(is_return_a_dispatch())
        result += "> " + get_return_dispatch_object_name() + "." + get_return_dispatch_method_name() + "\n";

    result += "---- Error List ----\n";

    for(size_t i = 0; i < error_list.size(); i++)
        result += error_list[i]->to_string() + "\n";

    result += "------ v table -----\n";

    for(auto it = v_table.begin(); it != v_table.end(); it++){
        std::string name = it->first->name;
        std::string type = it->second;

        result += name + " : " + type + "\n";
    }

    result += "------ d table -----\n";

    for(auto it = d_table.begin(); it != d_table.end(); it++){
        std::string method = it->first->method_name;
        std::string object = it->first->object_name;
        std::string return_type = it->second;

        result += object + "." + method + " : " + return_type + "\n";
    }

    //result += print_children();

    return result;
}

void Table::concatenate(const Table *table){
    // concatenate error_list
    for(auto it = table->error_list.begin(); it != table->error_list.end(); it++){
        throw_error(*it);
    }

    // concatenate v_table
    for(auto it = table->v_table.begin(); it != table->v_table.end(); it++){
        std::string name = it->first->name;
        std::string type = it->second;

        set_type(name, type);
    }

    // concatenate d_table
    for(auto it = table->d_table.begin(); it != table->d_table.end(); it++){
        std::string method = it->first->method_name;
        std::string object = it->first->object_name;
        std::string return_type = it->second;

        set_type(method, object, return_type);
    }
}

void Table::set_type(std::string name, std::string type){

    if(!is_type_exist(type))
        throw_error("type of variable " + name + " do not exist : " + type);
    

    std::string previous_type = get_type(name);

    if(previous_type != S_TYPE_NONE && type == S_TYPE_NONE)
        return;
    if(previous_type != S_TYPE_NONE && previous_type != type)
        throw_error("variable " + name + " have different types " + previous_type + " and " + type);

    Variable* new_variable = new Variable(name);

    // Delete previous stored variable if existing
    remove_type(name);

    v_table.insert({new_variable, type});

    if(return_type == S_TYPE_NONE && return_variable != NULL && return_variable->name == name)
        return_type = type;
}

void Table::set_type(std::string method_name, std::string object_name, std::string type){
    if(!is_type_exist(type))
        throw_error("type of dispatch " + object_name + "." + method_name + " do not exist : " + type);
    if(object_name != S_TYPE_SELF && Literals_visitor::get_dispatch_type(object_name, method_name) == S_TYPE_NONE)
        throw_error("method of dispatch " + object_name + "." + method_name + " do not exist : " + method_name);

    std::string previous_type = get_type(method_name, object_name);

    if(previous_type != S_TYPE_NONE && type == S_TYPE_NONE)
        return;
    if(previous_type != S_TYPE_NONE && previous_type != type && (is_primitive(previous_type) || is_primitive(type)))
        throw_error("dispatch " + object_name + "." + method_name + " have different types " + previous_type + " and " + type);
    else if(previous_type != S_TYPE_NONE && previous_type != type && !Literals_visitor::is_child_of(type, previous_type))
        throw_error("" + type + " is not a child of " + previous_type);

    // Delete previous stored dispatch if existing
    remove_type(method_name, object_name);

    Dispatch* new_dispatch = new Dispatch(method_name, object_name);

    d_table.insert({new_dispatch, type});

    if(previous_type == S_TYPE_NONE && type == S_TYPE_NONE && Literals_visitor::get_dispatch_type(object_name, method_name) != S_TYPE_NONE)
        set_type(method_name, object_name, Literals_visitor::get_dispatch_type(object_name, method_name));
    
    if(return_type == S_TYPE_NONE && return_dispatch != NULL && return_dispatch->method_name == method_name && return_dispatch->object_name == object_name)
        return_type = type;

    // set the type to all children classes
    std::vector<std::string> children = Literals_visitor::get_children(object_name);

    for (auto children_name : children){
        if(Literals_visitor::get_dispatch_type(children_name, method_name) != S_TYPE_NONE)
            set_type(method_name, children_name, type);
    }

}

void Table::set_type(std::string type){
    if(!is_type_exist(type))
        throw_error("type of return_variable do not exist : " + type);

    std::string previous_type = return_type;

    if(previous_type != S_TYPE_NONE && type == S_TYPE_NONE)
        return;
    if(previous_type != S_TYPE_NONE && previous_type != type && (is_primitive(previous_type) || is_primitive(type)))
        throw_error("Return_type have different types " + previous_type + " and " + type);
    else if (previous_type != S_TYPE_NONE && previous_type != type){
        set_type(return_type);
    }

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

std::string Table::get_return_variable_object_name() { 
    if(return_variable == NULL)
        return S_TYPE_NONE;
    return return_variable->object_name;
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

void Table::update_class_variable(std::string class_name){
    for(auto it = v_table.begin(); it != v_table.end(); it++){
        std::string name = it->first->name;
        std::string type = it->second;

        std::string class_variable_type = Literals_visitor::get_variable_type(class_name, name);

        if(class_variable_type != S_TYPE_NONE && type == S_TYPE_NONE){
            set_type(name, class_variable_type);
            break;
        } else if(class_variable_type != S_TYPE_NONE && class_variable_type != type){
            throw_error("Variable " + class_name + "." + name + " must have type " + class_variable_type + " but have " + type);
        }
    }
}

void Table::remove_class_variable(std::string class_name){
    for(auto it = v_table.begin(); it != v_table.end(); it++){
        std::string name = it->first->name;
        std::string type = it->second;

        std::string class_variable_type = Literals_visitor::get_variable_type(class_name, name);

        if(class_variable_type != S_TYPE_NONE && class_variable_type == type){
            replace_object_by_name(name, (type != S_TYPE_NONE) ? type : name);
            delete it->first;
            v_table.erase(it);
            remove_class_variable(class_name);
            return;
        } else if(class_variable_type != S_TYPE_NONE){
            throw_error("Variable " + class_name + "." + name + " must have type " + class_variable_type + " but have " + type);
        }
    }
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

void Table::v_table_must_be_empty(){
    if(v_table.empty())
        return;
    throw_error("There is non initiated variable");
}

void Table::update_children(std::string name, std::string type){
    std::string previous_type = get_type(name);

    if(type == S_TYPE_NONE)
        return;

    if(is_return(name) && get_type() == S_TYPE_NONE)
        return_type = type;

    // Delete previous stored variable if existing
    remove_type(name);


    Variable* new_variable = new Variable(name);

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

    // Delete previous stored dispatch if existing
    remove_type(method_name, object_name);

    Dispatch* new_dispatch = new Dispatch(method_name, object_name);

    d_table.insert({new_dispatch, type});

    for (auto table : children)
        table->update_children(method_name, object_name, type);
}