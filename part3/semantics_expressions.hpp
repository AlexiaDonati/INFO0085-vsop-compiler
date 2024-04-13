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

    class Error {
        public:
            Error(int line, int column, std::string file_name, std::string message) : 
                line(line), column(column), file_name(file_name), message(message) {};
            virtual ~Error() = default;

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

    class Table {
        public:
            Table(std::string return_type) : 
                return_type(return_type) {};
            ~Table() = default;

            void add_error(Error error) { error_list.push_back(error); }
            size_t number_of_error() { return error_list.size(); }

            std::string get_variable_type(std::string variable_name){
                auto it = v_table.find(variable_name);
                if (it != v_table.end())
                    return it->second;
                return NONE;
            }

            void set_variable_type(std::string variable_name, std::string variable_type){
                v_table[variable_name] = variable_type;
            }

            std::string get_return_type() { return return_type; }
        private:
            std::vector<Error> error_list;
            std::string return_type;
            // name -> type
            std::map<std::string, std::string> v_table;
    };

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

            Table get_value_from_void(void* void_value){
                // Specifiate the type of the value
                Table* value = (Table*) void_value;

                // Give this value to a non pointer variable
                Table return_value = *value;

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
