#ifndef _SEMANTICS_EXPRESSIONS_HPP
#define _SEMANTICS_EXPRESSIONS_HPP

#include <vector>
#include <map>
#include <string>
#include "ast.hpp"

#define UNIT "unit"
#define BOOLEAN "boolean"
#define INTEGER "int32"
#define STRING "string"
#define NONE ""
#define SELF "self"

namespace AST{

    namespace type{

        class Variable {
            public:
                std::string name;

                Variable(std::string name) : name(name) {};

                bool operator ==(const Variable &variable){
                    return (name == variable.name);
                }
        } ;

        class Dispatch {
            public:
                std::string method_name;
                std::string object_name;
                // TODO handle bad args
                //std::map<Variable*, std::string> args_variables;

                Dispatch(std::string method_name, std::string object_name) :
                    method_name(method_name), object_name(object_name){};
                ~Dispatch() = default;

                bool operator ==(const Dispatch &dispatch){
                    return (method_name == dispatch.method_name) && 
                           (object_name == dispatch.object_name) /*&&
                           (compare_args(dispatch))*/;
                }
            /*
            bool compare_args(Dispatch dispatch){
                size_t size_1 = args_variables.size();
                size_t size_2 = dispatch.args_variables.size();

                if(size_1 != size_2)
                    return false;

                for(size_t i = 0; i < size_1; i++){
                    std::string type_1 = args_variables[i].second;
                    std::string type_2 = dispatch.args_variables[i].second;
                    if(args_variables[i].type != NONE
                    && dispatch.args_variables[i].type != NONE
                    && args_variables[i].type != dispatch.args_variables[i].type)
                        return false;
                }

                return true;
            }
            */
        };

        class Error {
            public:
                Error(size_t line, size_t column, std::string file_name, std::string message) :
                    line(line), column(column), file_name(file_name), message(message) {};

                std::string to_string() {
                    // dum message to stop warning
                    line = line +1 -1;
                    column = column +1 -1;
                    return "mocked";
                }
            private:
                size_t line;
                size_t column;
                std::string file_name;
                std::string message;
        };

        class Table {
            public:
                Table(std::string return_type) : 
                    return_type(return_type) {
                        return_variable = NULL;
                        return_dispatch = NULL;
                    };
                Table(std::string return_type, std::string name) : 
                    return_type(return_type) {
                        return_variable = new Variable(name);
                        set_type(return_type, name);
                        return_dispatch = NULL;
                    };
                Table(std::string return_type, std::string method_name, std::string object_name) : 
                    return_type(return_type) {
                        return_variable = NULL;
                        return_dispatch = new Dispatch(method_name, object_name);
                        set_type(return_type, method_name, object_name);
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
                        0,
                        0,
                        "mocked",
                        message
                    );
                    error_list.push_back(new_error); 
                }

                void throw_error(Expr *expr, std::string message) { 
                    Error *new_error = new Error(
                        expr->get_line(),
                        expr->get_column(),
                        expr->get_file_name(),
                        message
                    );
                    error_list.push_back(new_error); 
                }

                size_t number_of_error() { return error_list.size(); }

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
                    Variable* new_variable = new Variable(name);

                    v_table.erase(new_variable);

                    delete new_variable;
                }

                void set_type(std::string name, std::string type){
                    std::string previous_type = get_type(name);

                    if(previous_type != NONE && previous_type != type)
                        throw_error("variable " + name + " have different types " + previous_type + " and " + type);
                    if(previous_type != NONE && type == NONE)
                        return;

                    Variable* new_variable = new Variable(name);

                    // Delete previous stored variable if existing
                    v_table.erase(new_variable);

                    v_table[new_variable] = type;
                }

                void set_type(std::string method_name, std::string object_name, std::string type){
                    std::string previous_type = get_type(method_name, object_name);

                    if(previous_type != NONE && previous_type != type)
                        throw_error("dispatch " + object_name + "." + method_name + " have different types " + previous_type + " and " + type);
                    if(previous_type != NONE && type == NONE)
                        return;

                    Dispatch* new_dispatch = new Dispatch(method_name, object_name);

                    // Delete previous stored dispatch if existing
                    d_table.erase(new_dispatch);

                    d_table[new_dispatch] = type;
                }

                void set_type(std::string type){
                    std::string previous_type = return_type;

                    if(previous_type != NONE && previous_type != type)
                        throw_error("Return_type have different types " + previous_type + " and " + type);
                    if(previous_type != NONE && type == NONE)
                        return;

                    if(return_variable != NULL){
                        return_type = type;
                        set_type(type, return_variable->name);
                    }
                    if(return_dispatch != NULL){
                        return_type = type;
                        set_type(type, return_dispatch->method_name, return_dispatch->object_name);
                    }
                }

                std::string get_type(std::string name) {
                    Variable* new_variable = new Variable(name);

                    std::string type = NONE;
                    auto it = v_table.find(new_variable);

                    if(it != v_table.end())
                        type = it->second;

                    delete new_variable;

                    return type;
                }

                std::string get_type(std::string method_name, std::string object_name) {
                    Dispatch* new_dispatch = new Dispatch(method_name, object_name);

                    std::string type = NONE;
                    auto it = d_table.find(new_dispatch);

                    if(it != d_table.end())
                        type = it->second;

                    delete new_dispatch;

                    return type;
                }

                std::string get_type() { return return_type; }

            private:
                std::vector<Error*> error_list;
                std::string return_type;
                Variable* return_variable;
                Dispatch* return_dispatch;
                std::map<Variable*, std::string, std::equal_to<Variable*>> v_table;
                std::map<Dispatch*, std::string, std::equal_to<Dispatch*>> d_table;
        };
    }

    class Literals_visitor : public Visitor {
        public:
            void* visit(Program* program);
            void* visit(Class* class_);
            void* visit(Field* field);
            void* visit(Method* method);
            void* visit(Formal* formal);
            void* visit(Block* block);
            void* visit(If* if_);
            void* visit(While* while_);
            void* visit(Let* let);
            void* visit(Assign* assign);
            void* visit(Self* self);
            void* visit(Unop* unop);
            void* visit(Binop* binop);
            void* visit(Call* call);
            void* visit(New* new_);
            void* visit(String* string_);
            void* visit(Integer* integer);
            void* visit(Boolean* boolean);
            void* visit(Unit* unit);
            void* visit(Object* object);

            type::Table get_value_from_void(void* void_value){
                // Specifiate the type of the value
                type::Table* value = (type::Table*) void_value;

                // Give this value to a non pointer variable
                type::Table return_value = *value;

                // Free the pointer
                delete value;

                // Return the value as non pointer
                return return_value;
            }   

            template <typename T>
            std::vector<type::Table> accept_list(List<T>* list){
                size_t size = list->get_size();
                std::vector<type::Table> result_vector;

                for (size_t i = 0; i < size; i++){
                    type::Table expr_result = this->get_value_from_void(list->accept_one(this, i));
                    result_vector.push_back(expr_result);
                }

                return result_vector;
            }    
    };
}

#endif
