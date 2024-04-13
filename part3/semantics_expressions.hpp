#ifndef _SEMANTICS_EXPRESSIONS_HPP
#define _SEMANTICS_EXPRESSIONS_HPP

#include <vector>
#include <map>
#include <string>
#include "ast.hpp"

#define UNIT "unit"
#define BOOLEAN "boolean"
#define INTEGER "integer"
#define STRING "string"
#define NONE ""

namespace AST{

    namespace type{
        class Error {
            public:
                Error(int line, int column, std::string file_name, std::string message) : 
                    line(line), column(column), file_name(file_name), message(message) {};
                ~Error() = default;

                int get_line() { return line; };
                int get_column() { return column; };
                std::string get_file_name() { return file_name; };
                std::string get_message() { return message; };
            private:
                int line;
                int column;
                std::string file_name;
                std::string message;
        };

        class Variable {
            public:
                Variable(std::string name, std::string type) :
                    name(name), type(type) {};
                ~Variable() = default;

                bool operator==(Variable variable){
                    return (variable.get_name() == name);
                }

                std::string get_name() { return name; };
                std::string get_type() { return type; };
            private:
                std::string name;
                std::string type;
        };

        class Dispatch {
            public:
                Dispatch(std::string method_name, std::string object_name, std::string return_type) :
                    method_name(method_name), object_name(object_name), return_type(return_type) {};
                ~Dispatch() = default;

                bool operator==(Dispatch dispatch){
                    return (dispatch.get_method_name() == method_name && 
                            dispatch.get_object_name() == object_name);
                }

                std::string get_method_name() { return method_name; };
                std::string get_object_name() { return object_name; };
                std::string get_return_type() { return return_type; };
            private:
                std::string method_name;
                std::string object_name;
                std::string return_type;
        };

        class Table {
            public:
                Table(std::string return_type) : 
                    return_type(return_type) {};
                ~Table() = default;

                void add_error(Error error) { error_list.push_back(error); }
                size_t number_of_error() { return error_list.size(); }

                std::string get_variable_type(std::string variable_name){
                    auto it = std::find(v_table.begin(), v_table.end(), new Variable(variable_name, NONE));
                    if(it != v_table.end())
                        return it->get_type();
                    return NONE;
                }

                void set_variable_type(std::string variable_name, std::string variable_type){
                    v_table.push_back(new Variable(variable_name, variable_type));
                }

                std::string get_method_type(std::string object_name, std::string method_name){
                    auto it = std::find(d_table.begin(), d_table.end(), new Dispatch(object_name, variable_name, NONE));
                    if(it != d_table.end())
                        return it->get_return_type();
                    return NONE;
                }

                void set_method_type(std::string object_name, std::string method_name, std::string return_type){
                    d_table.push_back(new Dispatch(object_name, method_name, return_type));
                }

                std::string get_return_type() { return return_type; }
            private:
                std::vector<Error> error_list;
                std::string return_type;
                std::vector<Variable> v_table;
                std::vector<Dispatch> d_table;
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

            void* get_void_from_value(type::Table* value){
                return value;
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
