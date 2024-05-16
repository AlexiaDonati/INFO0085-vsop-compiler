#include "ast.hpp"
#include "code_generation_visitor.hpp"
#include <iostream>

using namespace AST;
using namespace std;
  

void* Code_generation_visitor::visit(Program* program){
    llvm_instance = LLVM::get_instance(program, program->get_file_name());
    
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
