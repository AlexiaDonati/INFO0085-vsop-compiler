#ifndef _CODE_GENERATION_VISITOR_HPP
#define _CODE_GENERATION_VISITOR_HPP

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include "ast.hpp"
#include "type.hpp"
#include "llvm.hpp"

namespace AST{

    class Code_generation_visitor : public Visitor {
        private:
            LLVM *llvm_instance = nullptr;
            Class *current_class = nullptr;

        public:
            type::Table *table;
            

            Code_generation_visitor(type::Table *table): table(table){}

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
                size_t size = list->get_size();
                for (size_t i = 0; i < size; i++){
                    list->accept_one(this, i);
                }  
            } 

            Value *get_value_from_void(void* void_value){
                return (Value *) void_value;
            }   

            void *get_void_from_value(Value *value){
                return (void *) value;
            }

            void print(){
                if(llvm_instance != NULL){
                    llvm_instance->print();
                }
            }
    };

}

# endif