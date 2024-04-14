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
                    return_type(return_type) {};
                ~Table() = default;

                void add_error(Error error) { error_list.push_back(error); }
                size_t number_of_error() { return error_list.size(); }

                Table* concatenate(Table *table, std::string return_type){
                    Table new_table = new Table(return_type);

                    // concatenate v_table
                    for(auto it = v_table.begin(); it != v_table.end(); it++)
                        new_table->set_type(it->first->name, it->second);

                    for(auto it = table->v_table.begin(); it != table->v_table.end(); it++){
                        std::string name = it->first->name;
                        if(are_different_types(get_type(name), table->get_type(name)))
                            int a = 2; // TODO throw error

                        new_table->set_type(name, it->second);
                    }

                    // concatenate d_table
                    for(auto it = d_table.begin(); it != d_table.end(); it++)
                        new_table->set_type(method, object, it->second);

                    for(auto it = table->d_table.begin(); it != table->d_table.end(); it++){
                        std::string method = it->first->method_name;
                        std::string object = it->first->objetc_name;
                        if(are_different_types(get_type(method, object), table->get_type(method, object)))
                            int a = 2; // TODO throw error

                        new_table->set_type(method, object, it->second);
                    }

                    return new_table;
                }

                bool are_different_types(std::string type_1, std::string type_2){
                    return (type_1 != NONE) &&
                           (type_2 != NONE) &&
                           (type_1 != type_2);
                }

                void set_type(std::string name, std::string type){
                    Variable* new_variable = new Variable(name);

                    // Delete previous stored variable if existing
                    v_table.erase(new_variable);

                    v_table[new_variable] = type;
                }

                void set_type(std::string method_name, std::string object_name, std::string type){
                    Dispatch* new_dispatch = new Dispatch(method_name, object_name);

                    // Delete previous stored dispatch if existing
                    d_table.erase(new_dispatch);

                    d_table[new_dispatch] = type;
                }

                std::string get_type(std::string name) {
                    Variable* new_variable = new Variable(name);

                    std::string type = NONE;
                    auto it = v_table.find(new_variable);

                    if(it != d_table.end())
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

                bool exist(std::string name){
                    Variable* new_variable = new Variable(name);

                    bool exist = (v_table.count(new_variable)) ? true : false;

                    delete new_variable;

                    return exist;
                }

                bool exist(std::string method_name, std::string object_name){
                    Dispatch* new_dispatch = new Dispatch(method_name, object_name);

                    bool exist = (d_table.count(new_dispatch)) ? true : false;

                    delete new_dispatch;

                    return exist;
                }

                std::string get_return_type() { return return_type; }
            private:
                std::vector<Error> error_list;
                std::string return_type;
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
            std::string accept_list(List<T>* list){
                size_t size = list->get_size();
                std::string result = "[";

                for (size_t i = 0; i < size; i++){
                    std::string expr_result = this->get_value_from_void(list->accept_one(this, i));
                    result += expr_result;
                    result += (i+1 == size) ? "]" : ", ";
                }

                if(size == 0){
                    result += "]";
                }

                return result;
            }    
    };
}

#endif
