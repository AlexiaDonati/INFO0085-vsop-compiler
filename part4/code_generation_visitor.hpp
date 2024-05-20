#ifndef _CODE_GENERATION_VISITOR_HPP
#define _CODE_GENERATION_VISITOR_HPP

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include "ast.hpp"
#include "type.hpp"
#include "llvm.hpp"

using namespace llvm;
using namespace std;

namespace AST{

    class Code_generation_visitor : public Visitor {
        private:
            LLVM *llvm_instance = nullptr;

        public:
            type::Table *table;
            Program* current_program;
            Class* current_class;
            map<string, Value*> current_vtable;

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
            vector<Value *> accept_list(List<T>* list){
                vector<Value *> result_value_list;
                size_t size = list->get_size();
                for (size_t i = 0; i < size; i++){
                    result_value_list.push_back(get_value_from_void(list->accept_one(this, i)));
                }  
                return result_value_list;
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

            void generate_executable(const std::string &fileName){
                if(llvm_instance != NULL){
                    llvm_instance->executable(fileName);
                }
            }

            /*********************************************************************************/

            Value *assign_value(Value* new_value, Value *destination_ptr);

            Value* load(Value* object, string name);

            Value* get_pointer(Value* object, string name);

            Value* load(Value* object, uint position);

            Value* load(Value* ptr);

            Value* get_pointer(Value* object);

            Value* get_pointer(Value* object, uint position);

            string get_type_string(Expr *expr);

            Function* get_function(string class_name, string method);

            Function* get_function();

            vector<Value *> get_function_args(Function *function);

            vector<Value *> get_function_args();

            string get_name(Value *value);

            Value *get_variable(string name);

            Value *get_variable_ptr(string name);

            Value *get_field(string name);

            BasicBlock * get_current_block();

            BasicBlock * make_new_block();

            BasicBlock * make_new_block(Function *function);

            BasicBlock * make_next_block();

            BasicBlock * make_next_block(Function *function);

            void set_return_value(Value *return_value);

            bool is_parent(string child, string parent);

            Class *get_class(string class_name);

            bool is_field(string name);

            Value *get_unit_value();

            Value *make_pointer(Value *value);
    };

}

# endif