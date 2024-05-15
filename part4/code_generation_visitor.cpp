#include "ast.hpp"
#include "code_generation_visitor.hpp"
#include <iostream>

using namespace AST;
using namespace std;

void* Code_generation_visitor::print(){
    if(llvm_instance != NULL){
        llvm_instance->print();
    }
    return NULL;
}

void* Code_generation_visitor::visit(Program* program){
    llvm_instance = LLVM::get_instance(program, file_name);
    ACCEPT_LIST(program->get_class_list());
    return NULL;
}

void* Code_generation_visitor::visit(Class* class_){
    /******** Implement the 'new' function ********/
    Function *new_function = llvm_instance->module->getFunction("new_" + class_->get_name());

    // First create an entry point.
    BasicBlock *new_entry = BasicBlock::Create(
        *(llvm_instance->context),      // The LLVM context
        "entry",                        // The label of the block
        new_function);                  // The function in which should be inserted the block

    llvm_instance->builder->SetInsertPoint(new_entry);

    /******** Implement the 'init' function ********/
    Function *init_function = llvm_instance->module->getFunction("init_" + class_->get_name());

    // First create an entry point.
    BasicBlock *init_entry = BasicBlock::Create(
        *(llvm_instance->context),      // The LLVM context
        "entry",                        // The label of the block
        init_function);                 // The function in which should be inserted the block

    llvm_instance->builder->SetInsertPoint(init_entry);

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
    call->get_column();
    return NULL;
}

void* Code_generation_visitor::visit(New* new_){
    new_->get_column();
    return NULL;
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
