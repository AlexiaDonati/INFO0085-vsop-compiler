#ifndef _TYPE_HPP
#define _TYPE_HPP

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include "ast.hpp"

#define S_TYPE_UNIT "unit"
#define S_TYPE_BOOLEAN "bool"
#define S_TYPE_INTEGER "int32"
#define S_TYPE_STRING "string"
#define S_TYPE_NONE ""
#define S_TYPE_SELF "Self"
#define S_TYPE_OBJECT "Object"

namespace AST{
    class Expr;
    
    namespace type{

        class Variable {
            public:
                std::string name;

                Variable(std::string name) : name(name) {};

                bool operator ==(const Variable &variable){
                    return (name == variable.name);
                }

                bool operator <(const Variable *variable) const{
                    return name < variable->name;
                }
        } ;

        class Dispatch {
            public:
                std::string method_name;
                std::string object_name;

                Dispatch(std::string method_name, std::string object_name) :
                    method_name(method_name), object_name(object_name){};
                ~Dispatch() = default;
        };

        class Error {
            public:
                Error(size_t line, size_t column, std::string file_name, std::string message) :
                    line(line), column(column), file_name(file_name), message(message) {};

                std::string to_string() {
                    return file_name + ":" + std::to_string(line) + ":" + std::to_string(column) + ": semantic error: " + message;
                }
            private:
                size_t line;
                size_t column;
                std::string file_name;
                std::string message;
        };

        class Table {
            public:
                Table(size_t line, size_t column, std::string file_name, Expr *owner, std::string return_type) : 
                    line(line), column(column), file_name(file_name), return_type(return_type), owner(owner) {
                        return_variable = NULL;
                        return_dispatch = NULL;
                    };
                Table(size_t line, size_t column, std::string file_name, Expr *owner, std::string return_type, std::string name) : 
                    line(line), column(column), file_name(file_name), return_type(return_type), owner(owner) {
                        return_variable = new Variable(name);
                        set_type(name, return_type);
                        return_dispatch = NULL;
                    };
                Table(size_t line, size_t column, std::string file_name, Expr *owner, std::string return_type, std::string method_name, std::string object_name) : 
                    line(line), column(column), file_name(file_name), return_type(return_type), owner(owner) {
                        return_variable = NULL;
                        return_dispatch = new Dispatch(method_name, object_name);
                        set_type(method_name, object_name, return_type);
                    };
                ~Table() {
                    if(return_variable != NULL)
                        delete return_variable;
                    if(return_dispatch != NULL)
                        delete return_dispatch;
                };

                void throw_error(Error *error) { 
                    error_list.push_back(error); 
                }

                void throw_error(std::string message) { 
                    Error *new_error = new Error(
                        line,
                        column,
                        file_name,
                        message
                    );
                    error_list.push_back(new_error); 
                }

                void throw_error(Expr *expr, std::string message);

                std::string errors_to_string(){
                    std::string result = "";

                    result += "---- Error List ----\n";

                    for(size_t i = 0; i < error_list.size(); i++)
                        result += error_list[i]->to_string() + "\n";

                    result += "--------------------\n";

                    return result;
                }

                std::string to_string(){
                    std::string result = "";

                    result += "---- Return type ---\n";

                    result += return_type + "\n";

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

                void concatenate(const Table *table){
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

                void remove_type(std::string name){
                    for(auto it = v_table.begin(); it != v_table.end(); it++){
                        std::string name_ = it->first->name;
                        std::string type = it->second;

                        if(name == name_){
                            replace_object_by_name(name_, (type != S_TYPE_NONE) ? type : name_);
                            delete it->first;
                            v_table.erase(it);
                            break;
                        }
                    }
                }

                void remove_type(std::string method_name, std::string object_name){
                    for(auto it = d_table.begin(); it != d_table.end(); it++){
                        std::string method = it->first->method_name;
                        std::string object = it->first->object_name;
                        std::string return_type = it->second;

                        if(method_name == method && object_name == object){
                            delete it->first;
                            d_table.erase(it);
                            break;
                        }
                    }
                }

                void set_type(std::string name, std::string type){
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

                void set_type(std::string method_name, std::string object_name, std::string type){
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

                void set_type(std::string type){
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

                std::string get_type(std::string name) {
                    for(auto it = v_table.begin(); it != v_table.end(); it++){
                        std::string name_ = it->first->name;
                        std::string type = it->second;

                        if(name == name_)
                            return type;
                    }
                    return S_TYPE_NONE;
                }

                std::string get_type(std::string method_name, std::string object_name) {
                    for(auto it = d_table.begin(); it != d_table.end(); it++){
                        std::string method = it->first->method_name;
                        std::string object = it->first->object_name;
                        std::string return_type = it->second;

                        if(method_name == method && object_name == object)
                            return return_type;
                    }
                    return S_TYPE_NONE;
                }

                std::string get_type() { return return_type; }

                std::string get_return_variable_name() { 
                    if(return_variable == NULL)
                        return S_TYPE_NONE;
                    return return_variable->name;
                }

                void replace_object_by_name(std::string old_name, std::string new_name){
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

                        remove_type(method, object);
                        set_type(method, new_name, return_type);
                    }
                }

                void v_table_must_be_empty(){
                    if(v_table.empty())
                        return;
                    throw_error("Variable non defined");
                }

                // Table tree

                void add_child(Table *child){
                    children.push_back(child);
                }

                void update_children(){
                    // update v_table
                    for(auto it = v_table.begin(); it != v_table.end(); it++){
                        std::string name = it->first->name;
                        std::string type = it->second;

                        for (auto table : children)
                            table->update_children(name, type);
                    }

                    // update d_table
                    for(auto it = d_table.begin(); it != d_table.end(); it++){
                        std::string method = it->first->method_name;
                        std::string object = it->first->object_name;
                        std::string return_type = it->second;

                        for (auto table : children)
                            table->update_children(method, object, return_type);
                    }
                }

                void update_children(std::string name, std::string type){
                    std::string previous_type = get_type(name);

                    if(type == S_TYPE_NONE)
                        return;

                    Variable* new_variable = new Variable(name);

                    // Delete previous stored variable if existing
                    remove_type(name);

                    v_table.insert({new_variable, type});

                    for (auto table : children)
                        table->update_children(name, type);
                }

                void update_children(std::string method_name, std::string object_name, std::string type){
                    std::string previous_type = get_type(method_name, object_name);

                    if(type == S_TYPE_NONE)
                        return;

                    Dispatch* new_dispatch = new Dispatch(method_name, object_name);

                    // Delete previous stored dispatch if existing
                    remove_type(method_name, object_name);

                    d_table.insert({new_dispatch, type});

                    for (auto table : children)
                        table->update_children(method_name, object_name, type);
                }

                std::string print_children(){
                    size_t i = 0;
                    std::string result = "";
                    for (auto table : children){
                        result += "************" + std::to_string(i) + "/" + std::to_string(children.size()) + "************\n";
                        result += table->to_string();
                        i++;
                    }
                    
                    return result;
                }

                Table* find_expr_table(Expr *expr){
                    if(owner == expr)
                        return this;
                        
                    for (auto table : children){
                        Table *aux = table->find_expr_table(expr);
                        if(aux != NULL)
                            return aux;
                    }
                    return NULL;
                }

            private:
                size_t line;
                size_t column;
                std::string file_name;
                std::vector<Error*> error_list;
                std::string return_type;
                Variable* return_variable;
                Dispatch* return_dispatch;
                std::map<Variable*, std::string> v_table;
                std::map<Dispatch*, std::string> d_table;

                // Table tree
                std::vector<Table*> children;
                Expr *owner;
        };
    }
}

#endif
