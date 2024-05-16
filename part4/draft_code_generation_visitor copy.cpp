#include "ast.hpp"
#include "code_generation_visitor.hpp"
#include <iostream>

using namespace AST;
using namespace std;

#define BUILDER llvm_instance->builder
#define MODULE module

void* Code_generation_visitor::visit(Program* program){
    this->accept_list(program->get_class_list());

    return NULL;
}

void* Code_generation_visitor::visit(Class* class_){
    current_class = class_->get_name();

    // Clear v_table (because do not need variable of other classes)
    current_vtable.clear();
    // We don't put field in the v_table 
    // because we will get the value from the self arg

    // We know that if a variable has no value in the v table
    // It is a field of the class

    // Make methods
    this->accept_list(class_->get_method_list());

    return NULL;
    
}

void* Code_generation_visitor::visit(Field* field){
    return NULL;
}

void* Code_generation_visitor::visit(Method* method){
    // Put args in v_table
    Function *method_function = get_function(current_class, method->get_name());

    vector<Value *> args_values = get_function_args(method_function);
    map<string, Value*> old_vtable;

    for (auto arg_value : args_values){
        // Save all already existing variables to retreive later
        if(current_vtable.count(get_name(arg_value)))
            old_vtable[get_name(arg_value)] = current_vtable[get_name(arg_value)];
            
        current_vtable[get_name(arg_value)] = arg_value;
    }

    // Make block then implement it
    make_new_block(method_function);

    method->get_body_block()->accept(this);

    // Remove args from v_table
    for (auto arg_value : args_values)
        current_vtable.erase(get_name(arg_value));

    // retreve old variable
    for(auto it = old_vtable.begin(); it != old_vtable.end(); it++){
        string arg_name = it->first;
        Value *arg_value = it->second;

        current_vtable[arg_name] = arg_value;
    }

    return NULL;
}

void* Code_generation_visitor::visit(Formal* formal){
    return NULL;
}

void* Code_generation_visitor::visit(Block* block){
    vector<Value *> lines_values = this->accept_list(block->get);
    return lines_values.back();
}

void* Code_generation_visitor::visit(If* if_){
    
}

void* Code_generation_visitor::visit(While* while_){
    
}

void* Code_generation_visitor::visit(Let* let){
    // add new variable to vtable and keep the old if there is a collition on names

    // initialize if needed

    // execute block

    // reload old variable if collision
    
}

void* Code_generation_visitor::visit(Assign* assign){
    Value* expr_value = (Value *) assign->get_expr()->accept(this);

    BUILDER->CreateStore(expr_value, get_variable(assign->get_name()));
}

void* Code_generation_visitor::visit(Self* self){
    return get_function_args().front();
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
    
}

void* Code_generation_visitor::visit(Integer* integer){
    
}

void* Code_generation_visitor::visit(Boolean* boolean){
    
}

void* Code_generation_visitor::visit(Unit* unit){
    
}

void* Code_generation_visitor::visit(Object* object){
   return get_variable(object->get_name());
}

Value* Code_generation_visitor::load(Value* object, string name){
    Value* ptr = get_pointer(object, position);
    return BUILDER->CreateLoad(ptr, "");
}

Value* Code_generation_visitor::get_pointer(Value* object, string name){
    return BUILDER->CreateGEP(
            object, 
            {BUILDER->getInt32(0), 
            BUILDER->getInt32(field_indexes[name])},
            "");
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

Function* Code_generation_visitor::get_function(string class, string method){
    return MODULE->getFunction(class + "." + method);
}

Function* Code_generation_visitor::get_function(){
    return BUILDER->GetInsertBlock()->getParent();
}

vector<Value *> Code_generation_visitor::get_function_args(Function *function){
    vector<Value *> arguments_values;
    for (auto &arg : function->args())
        arguments_values.push_back(&arg);
    return arguments_values;
}

vector<Value *> Code_generation_visitor::get_function_args(){
    Function *current_function = get_function();
    vector<Value *> arguments_values;
    for (auto &arg : current_function->args())
        arguments_values.push_back(&arg);
    return arguments_values;
}

string Code_generation_visitor::get_name(Value *value){
    return value->getName()->str();
}

Value *Code_generation_visitor::get_variable(string name){
    if(current_vtable.count(name))
        return current_vtable[name];

    return load(get_function_args.first(), name);
}

Value *Code_generation_visitor::get_field(string name){
    vector<Value *> args_values = get_function_args();

    Value *self_value = args_values.first();

    return load(self_value, name);
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
