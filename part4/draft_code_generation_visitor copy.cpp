#include "ast.hpp"
#include "code_generation_visitor.hpp"
#include <iostream>

using namespace AST;
using namespace std;

#define BUILDER llvm_instance->builder

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
    vector<Value *> lines_values = this->accept_list(block->get);
    return lines_values.back();
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
    string op = unop->get_op();

    Value *result = NULL;

    Value *expr = (Value*) unop->get_expr()->accept(this);

    switch (unop_to_enum.at(op)){
    case NOT:
        result = BUILDER->CreateNot(expr, "");
        break;
    case UNARY:
        result = BUILDER->CreateNeg(expr, "");
        break;
    case ISNULL:
        result = BUILDER->CreateIsNull(expr, "");
        break;
    default:
        break;
    }

    return result;
}

void* Code_generation_visitor::visit(Binop* binop){
    string op = binop->get_op();

    Value *result = NULL;

    Value *left = (Value*) binop->get_left_expr()->accept(this);
    Value *right = (Value*) binop->get_right_expr()->accept(this);

    switch (binop_to_enum.at(op)){
    case EQ:
        result = BUILDER->CreateICmpEQ(left, right, "");
        break;
    case LT:
        result = BUILDER->CreateICmpSLT(left, right, "");
        break;
    case LEQ:
        result = BUILDER->CreateICmpSLE(left, right, "");
        break;
    case ADD:
        result = BUILDER->CreateAdd(left, right, "");
        break;
    case SUB:
        result = BUILDER->CreateSub(left, right, "");
        break;
    case MUL:
        result = BUILDER->CreateMul(left, right, "");
        break;
    case DIV:
        result = BUILDER->CreateSDiv(left, right, "");
        break;
    case POW:
        // TODO: implement the pow function the call it here
        break;
    case AND:
        result = BUILDER->CreateAnd(left, right, "");
        break;
    default:
        break;
    }

    return result;
}

void* Code_generation_visitor::visit(Call* call){
    Value *object = (Value*) call->get_object()->accept(this);
    string object_type = get_type_string(call->get_object());
    // Position of the method in the m table
    uint position = method_indexes[object_type + "." + call->get_method()];
    string return_type = get_type_string(call);
    vector<Value *> args = this->accept_list(call->get_arg_expr_list());

    // Load the value of mtable of object
    Value* m_table_value = load(object, 0); // m table always at index 0
    
    // Load object.method() in object m_tables
    Value* length_method_value = load(m_table_value, position);

    FunctionType *false_signature = FunctionType::get(
        get_type(return_type), // Return type
        {},               // do not need to specify because will be con$mpleted automaticaly by call (to be verified)
        false);           // No variable number of arguments

    args.insert(args.begin(), object);

    return BUILDER->CreateCall(false_signature, length_method_value, args, "");
}

void* Code_generation_visitor::visit(New* new_){
    Function *new_function = module->getFunction(new_->type + "..new");

    Value *new_object = BUILDER->CreateCall(new_function);

    Function *init_function = module->getFunction(new_->type + "..init");

    return BUILDER->CreateCall(init_function, {new_object});
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
    return BUILDER->CreateLoad(ptr, "");
}

Value* Code_generation_visitor::get_pointer(Value* object, uint position){
    return BUILDER->CreateGEP(
            object, 
            {BUILDER->getInt32(0), 
            BUILDER->getInt32(position)},
            "");
}

string Code_generation_visitor::get_type_string(Expr *expr){
    return table->find_expr_table(expr)->type_to_string();
}

Function* Code_generation_visitor::get_function(){
    return BUILDER->GetInsertBlock()->getParent();
}

BasicBlock * Code_generation_visitor::make_new_block(Function *function){
    // ==== ==== ==== Define block
    BasicBlock *function_block = BasicBlock::Create(
        *context,         // The LLVM context
        "",             // The label of the block
        function);        // The function in which should be inserted the block
    
    // ==== ==== ==== Define Builder
    BUILDER->SetInsertPoint(function_block);

    return function_block;
}

BasicBlock * Code_generation_visitor::make_next_block(Function *function){
    // ==== ==== ==== Define block
    BasicBlock *function_block = BasicBlock::Create(
        *context,         // The LLVM context
        "",             // The label of the block
        function);        // The function in which should be inserted the block
    
    // ==== ==== ==== Define Builder
    BUILDER->CreateBr(function_block);
    BUILDER->SetInsertPoint(function_block);

    return function_block;
}
