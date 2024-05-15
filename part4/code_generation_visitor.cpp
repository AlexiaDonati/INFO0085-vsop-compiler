#include "ast.hpp"
#include "code_generation_visitor.hpp"
#include <iostream>

using namespace AST;
using namespace std;

void* Code_generation_visitor::visit(Program* program){
    program->get_column();
    return NULL;
}

void* Code_generation_visitor::visit(Class* class_){
    class_->get_column();
    return NULL;
}

void* Code_generation_visitor::visit(Field* field){
    field->get_column();
    return NULL;
}

void* Code_generation_visitor::visit(Method* method){
    method->get_column();
    return NULL;
}

void* Code_generation_visitor::visit(Formal* formal){
    formal->get_column();
    return NULL;
}

void* Code_generation_visitor::visit(Block* block){
    block->get_column();
    return NULL;
}

void* Code_generation_visitor::visit(If* if_){
    if_->get_column();
    return NULL;
}

void* Code_generation_visitor::visit(While* while_){
    while_->get_column();
    return NULL;
}

void* Code_generation_visitor::visit(Let* let){
    let->get_column();
    return NULL;
}

void* Code_generation_visitor::visit(Assign* assign){
    assign->get_column();
    return NULL;
}

void* Code_generation_visitor::visit(Self* self){
    self->get_column();
    return NULL;
}

void* Code_generation_visitor::visit(Unop* unop){
    unop->get_column();
    return NULL;
}

void* Code_generation_visitor::visit(Binop* binop){
    binop->get_column();
    return NULL;
}

void* Code_generation_visitor::visit(Call* call){
    Value *object = (Value*) call->get_object()->accept(this);
    // Position of the method in the m table
    uint position = // TODO: must know the position of the method in m table
    std::string return_type = table->find_expr_table(call)->->type_to_string();
    std::vector<Value *> args = this->accept_list(call->get_arg_expr_list());

    // Load the value of mtable of object
    Value* m_table_value = load(object, 0); // m table always at index 0
    
    // Load object.method() in object m_tables
    Value* length_method_value = load(m_table_value, position);

    FunctionType *false_signature = FunctionType::get(
        get_type(return_type), // Return type
        {},               // do not need to specify because will be con$mpleted automaticaly by call (to be verified)
        false);           // No variable number of arguments

    args.insert(args.begin(), object);

    return builder->CreateCall(false_signature, length_method_value, args, "");
}

void* Code_generation_visitor::visit(New* new_){
    Function *new_function = module->getFunction(new_->type + "..new");

    Value *new_object = llvm_instance->builder->CreateCall(new_function);

    Function *init_function = module->getFunction(new_->type + "..init");

    return llvm_instance->builder->CreateCall(init_function, {new_object});
}

void* Code_generation_visitor::visit(String* string_){
    string_->get_column();
    return NULL;
}

void* Code_generation_visitor::visit(Integer* integer){
    integer->get_column();
    return NULL;
}

void* Code_generation_visitor::visit(Boolean* boolean){
    boolean->get_column();
    return NULL;
}

void* Code_generation_visitor::visit(Unit* unit){
    unit->get_column();
    return NULL;
}

void* Code_generation_visitor::visit(Object* object){
    object->get_column();
    return NULL;
}

Value* Code_generation_visitor::load(Value* object, uint position){
    Value* ptr = get_pointer(object, position);
    return llvm_instance->builder->CreateLoad(ptr, "");
}

Value* Code_generation_visitor::get_pointer(Value* object, uint position){
    return llvm_instance->builder->CreateGEP(
            object, 
            {llvm_instance->builder->getInt32(0), 
            llvm_instance->builder->getInt32(position)},
            "");
}
