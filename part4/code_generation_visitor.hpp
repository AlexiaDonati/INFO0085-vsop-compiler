#ifndef _CODE_GENERATION_VISITOR_HPP
#define _CODE_GENERATION_VISITOR_HPP

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include "ast.hpp"

namespace AST{

    class Code_generation_visitor : public Visitor {
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

            template <typename T>
            void accept_list(List<T>* list){
                // TODO
                // to stop warnings
                list->get_size();
            } 

            void get_value_from_void(void* void_value){
                // TODO
                // to stop warnings
                void* a = void_value;
                void_value = a;
            }   

            void* get_void_from_value(std::string value){
                // TODO
                // to stop warnings
                value = "";
                return NULL;
            }
    };

}

# endif