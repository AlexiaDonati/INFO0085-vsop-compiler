#include "semantics_expressions.hpp"

using namespace AST;
using namespace std;

void* Literals_visitor::visit(Program* program) {
    // to remove the warning
    program->get_line();
    return NULL;
}

void* Literals_visitor::visit(Class* class_) {
    // to remove the warning
    class_->get_line();
    return NULL;
}

void* Literals_visitor::visit(Field* field) {
    // to remove the warning
    field->get_line();
    return NULL;
}

void* Literals_visitor::visit(Method* method) {
    // to remove the warning
    method->get_line();
    return NULL;
}

void* Literals_visitor::visit(Formal* formal) {
    // to remove the warning
    formal->get_line();
    return NULL;
}

void* Literals_visitor::visit(Block* block) {
    // to remove the warning
    block->get_line();
    return NULL;
}

void* Literals_visitor::visit(If* if_) {
    // to remove the warning
    if_->get_line();
    return NULL;
}

void* Literals_visitor::visit(While* while_) {
    // to remove the warning
    while_->get_line();
    return NULL;
}

void* Literals_visitor::visit(Let* let) {
    // to remove the warning
    let->get_line();
    return NULL;
}

void* Literals_visitor::visit(Assign* assign) {
    // to remove the warning
    assign->get_line();
    return NULL;
}

void* Literals_visitor::visit(Self* self) {
    // to remove the warning
    self->get_line();
    return NULL;
}

void* Literals_visitor::visit(Unop* unop) {
    // to remove the warning
    unop->get_line();
    return NULL;
}

void* Literals_visitor::visit(Binop* binop) {
    // to remove the warning
    binop->get_line();
    return NULL;
}

void* Literals_visitor::visit(Call* call) {
    // to remove the warning
    call->get_line();
    return NULL;
}

void* Literals_visitor::visit(New* new_) {
    // to remove the warning
    new_->get_line();
    return NULL;
}

void* Literals_visitor::visit(String* string_) {
    // to remove the warning
    string_->get_line();
    return NULL;
}

void* Literals_visitor::visit(Integer* integer) {
    // to remove the warning
    integer->get_line();
    return NULL;
}

void* Literals_visitor::visit(Boolean* boolean) {
    // to remove the warning
    boolean->get_line();
    return NULL;
}

void* Literals_visitor::visit(Unit* unit) {
    // to remove the warning
    unit->get_line();
    return NULL;
}

void* Literals_visitor::visit(Object* object) {
    // to remove the warning
    object->get_line();
    return NULL;
}

