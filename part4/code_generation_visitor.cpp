#include "ast.hpp"
#include "code_generation_visitor.hpp"
#include <iostream>

using namespace AST;
using namespace std;
  

void* Code_generation_visitor::visit(Program* program){
    llvm_instance = LLVM::get_instance(program, program->get_file_name());
    ACCEPT_LIST(program->get_class_list());
    return NULL;
}

void* Code_generation_visitor::visit(Class* class_){
    current_class = class_;

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

    ACCEPT_LIST(class_->get_method_list());

    return NULL;
}

void* Code_generation_visitor::visit(Field* field){
    field->get_column();
    return NULL;
}

void* Code_generation_visitor::visit(Method* method){
    /******** Implement the method ********/
    // First retrieve the function:
    Function *function = llvm_instance->module->getFunction(method->get_name() + "_" + current_class->get_name());

    // Create the entry block:
    BasicBlock *function_entry = BasicBlock::Create(
        *(llvm_instance->context),  // The LLVM context
        "entry",                    // The label of the block
        function);                  // The function in which should be inserted the block

    llvm_instance->builder->SetInsertPoint(function_entry);
    
    //auto args = function->arg_begin() + 1;
    //ACCEPT_LIST(method->get_formal_list());

    //auto ret = ACCEPT(method->get_body_block());
    //ret = llvm_instance->builder->CreatePointerCast(ret, llvm_instance->get_type(method->get_return_type()));
    //llvm_instance->builder->CreateRet(ret);
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
    return TO_VOID(llvm_instance->builder->CreateGlobalStringPtr(string_->get_value()));
}

void* Code_generation_visitor::visit(Integer* integer){
    return TO_VOID(ConstantInt::get(llvm_instance->get_type("int32"), integer->get_value())); // The value of the integer
}

void* Code_generation_visitor::visit(Boolean* boolean){
    if(boolean->get_value()){
        return TO_VOID(ConstantInt::get(llvm_instance->get_type("bool"), 1));
    }
    else{
        return TO_VOID(ConstantInt::get(llvm_instance->get_type("bool"), 0));
    }
}

void* Code_generation_visitor::visit(Unit* unit){
    return TO_VOID(ConstantPointerNull::get(llvm_instance->get_type("unit")));
}

void* Code_generation_visitor::visit(Object* object){
    object->get_column();
    return NULL;
}
