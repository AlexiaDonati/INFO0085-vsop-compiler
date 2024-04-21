#ifndef _SEMANTICS_EXPRESSIONS_HPP
#define _SEMANTICS_EXPRESSIONS_HPP

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include "ast.hpp"
#include "type.hpp"

namespace AST{

    class Literals_visitor : public Visitor {
        public:
            static std::map<std::string, std::string> c_table;
            static std::map<type::Variable*, std::string> v_table;
            static std::map<type::Dispatch*, std::string> d_table;

            static void set_parent(std::string child, std::string parent);
            static void set_variable(std::string object, std::string name, std::string type);
            static void set_dispatch(std::string object, std::string name, std::string type);

            static bool is_child_of(std::string child, std::string parent);
            static std::string get_variable_type(std::string object, std::string name);
            static std::string get_dispatch_type(std::string object, std::string name);

            static std::vector<std::string> get_children(std::string parent);
            static std::vector<std::string> get_parents(std::string child);

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

            std::string to_string(AST::Program *ast){
                type::Table table = ACCEPT(ast);

                return table.to_string();
            }

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
            std::vector<type::Table*> accept_list(List<T>* list){
                size_t size = list->get_size();
                std::vector<type::Table*> result_vector;

                for (size_t i = 0; i < size; i++){
                    type::Table *expr_result = (type::Table*) list->accept_one(this, i);
                    result_vector.push_back(expr_result);
                }
                return result_vector;
            }    
    };
}

#endif
