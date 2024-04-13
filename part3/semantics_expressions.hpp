#ifndef _SEMANTICS_EXPRESSIONS_HPP
#define _SEMANTICS_EXPRESSIONS_HPP

#include "ast.hpp"

namespace AST{
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

            std::string get_value_from_void(void* void_value){
                // Specifiate the type of the value
                std::string* value = (std::string*) void_value;

                // Give this value to a non pointer variable
                std::string return_value = *value;

                // Free the pointer
                delete value;

                // Return the value as non pointer
                return return_value;
            }   

            void* get_void_from_value(std::string value){
                return new std::string(value);
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
