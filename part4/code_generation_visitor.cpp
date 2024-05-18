#include "ast.hpp"
#include "code_generation_visitor.hpp"
#include <iostream>

using namespace AST;
using namespace std;
using namespace llvm;

#define BUILDER llvm_instance->builder
#define MODULE llvm_instance->module
#define CONTEXT llvm_instance->context

#define LOG(value) llvm_instance->print(value)

enum BINOP {EQ, LT, LEQ, ADD, SUB, MUL, DIV, POW, AND};
enum UNOP {NOT, UNARY, ISNULL};

const map<string, BINOP> binop_to_enum = {
    {"=", EQ},
    {"<", LT},
    {"<=", LEQ},
    {"+", ADD},
    {"-", SUB},
    {"*", MUL},
    {"/", DIV},
    {"^", POW},
    {"and", AND},
};

const map<string, UNOP> unop_to_enum = {
    {"not", NOT},
    {"-", UNARY},
    {"isnull", ISNULL},
};

void* Code_generation_visitor::visit(Program* program){
    llvm_instance = LLVM::get_instance(program, program->get_file_name());
    
    this->accept_list(program->get_class_list());

    return NULL;
}

void* Code_generation_visitor::visit(Class* class_){
    current_class = class_;

    // Clear v_table (because do not need variable of other classes)
    current_vtable.clear();
    // We don't put field in the v_table 
    // because we will get the value from the self arg

    // We know that if a variable has no value in the v table
    // It is a field of the class

    this->accept_list(class_->get_field_list());

    // Make methods
    this->accept_list(class_->get_method_list());

    return NULL;
}

void* Code_generation_visitor::visit(Field* field){
    if(!field->has_init_expr())
        return NULL;

    Function *init_function = MODULE->getFunction(current_class->get_name() + "..init");

    BasicBlock *last_block = &init_function->getBasicBlockList().back();

    BasicBlock *previous_block = get_current_block();

    BUILDER->SetInsertPoint(last_block);

    Value* init_value = (Value *) field->get_init_expr()->accept(this);

    Value *field_ptr = get_variable_ptr(field->get_name());

    BUILDER->CreateStore(init_value, field_ptr);

    BUILDER->SetInsertPoint(previous_block);

    return init_value;
}

void* Code_generation_visitor::visit(Method* method){
    // Clear v_table (because do not need variable of other methods)
    current_vtable.clear();

    // Put args in v_table
    Function *method_function = get_function(current_class->get_name(), method->get_name());

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

    Value *return_value = (Value *) method->get_body_block()->accept(this);
    
    set_return_value(return_value);

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
    formal->get_column();
    return NULL;
}

void* Code_generation_visitor::visit(Block* block){
    vector<Value *> lines_values = this->accept_list(block->get_expr_list());
    return lines_values.back();
}

void* Code_generation_visitor::visit(If* if_){
    // Make the condition block
    BasicBlock *cond_block = make_next_block();

    Value *cond_value = (Value*) if_->get_cond_expr()->accept(this);

    // Make the after block
    BasicBlock *after_block = make_new_block();

    // Make the then block
    BasicBlock *then_block = make_new_block();

    Value *then_value = (Value*)if_->get_then_expr()->accept(this);

    BUILDER->CreateBr(after_block);

    // Make the else block
    BasicBlock *else_block = NULL;
    Value *else_value = NULL;
    if(if_->has_else_expr()){
        else_block = make_new_block();

        else_value = (Value*)if_->get_else_expr()->accept(this);

        BUILDER->CreateBr(after_block);
    }

    // Make the builder targetting the current block
    BUILDER->SetInsertPoint(cond_block);

    // if cond is true -> jmp to then_block, else -> jmp to else_block
    // if no else block -> jmp to after_block
    if(else_block == NULL)
        BUILDER->CreateCondBr(cond_value, then_block, after_block);
    else
        BUILDER->CreateCondBr(cond_value, then_block, else_block);

    // Make the builder targetting the current block
    BUILDER->SetInsertPoint(after_block);

    // If one is unit -> return unit
    if(else_value == NULL || then_value == NULL)
        return NULL;

    // use phi function to get the return value

    PHINode* phi_node = BUILDER->CreatePHI(then_value->getType(), 2, "");
    phi_node->addIncoming(then_value, then_block);
    phi_node->addIncoming(else_value, else_block);

    return dyn_cast<Value>(phi_node);
}

void* Code_generation_visitor::visit(While* while_){
    // Make the condition block
    BasicBlock *cond_block = make_next_block();

    Value *cond_value = (Value*) while_->get_cond_expr()->accept(this);

    // Make the body block
    BasicBlock *body_block = make_new_block();

    while_->get_body_expr()->accept(this);
    BUILDER->CreateBr(cond_block);

    // Make the after block
    BasicBlock *after_block = make_new_block();

    // Make the builder targetting the current block
    BUILDER->SetInsertPoint(cond_block);

    // if cond is true -> jmp to body_block, else -> jmp to after_block
    BUILDER->CreateCondBr(cond_value, body_block, after_block);

    // Must finish on the after block
    BUILDER->SetInsertPoint(after_block);
    
    return NULL;
}

void* Code_generation_visitor::visit(Let* let){
    // add new variable to vtable and keep the old if there is a collition on names
    string name = let->get_name();
    Value *old_variable = NULL;
    if(current_vtable.count(name))
        old_variable = current_vtable[name];

    // initialize if needed
    if(let->has_init_expr()){
        make_next_block();
        Value* variable_value = (Value*) let->get_init_expr()->accept(this);
        current_vtable[name] = variable_value;
    } else {
        current_vtable[name] = BUILDER->CreateAlloca(llvm_instance->get_type(let->get_type()));;
    }

    // execute block
    make_next_block();
    Value* return_value = (Value*) let->get_scope_expr()->accept(this);

    // Remove new and reload old variable if collision
    current_vtable.erase(name);

    if(old_variable != NULL)
        current_vtable[name] = old_variable;  

    return return_value;
}

void* Code_generation_visitor::visit(Assign* assign){
    Value* expr_value = (Value *) assign->get_expr()->accept(this);

    Value *new_value = get_variable_ptr(assign->get_name());

    BUILDER->CreateStore(expr_value, new_value);

    // update v table
    if(current_vtable.count(assign->get_name()))
        current_vtable[assign->get_name()] = new_value;

    return new_value;
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
    uint position = llvm_instance->method_indexes[object_type + "." + call->get_method()];
    string return_type = get_type_string(call);
    vector<Value *> args = this->accept_list(call->get_arg_expr_list());

    // Load the value of mtable of object
    Value* m_table_value = load(object, 0); // m table always at index 0
    
    // Load object.method() in object m_tables
    Value* length_method_value = load(m_table_value, position);

    FunctionType *false_signature = FunctionType::get(
        llvm_instance->get_type(return_type), // Return type
        {},               // do not need to specify because will be con$mpleted automaticaly by call (to be verified)
        false);           // No variable number of arguments

    args.insert(args.begin(), object);

    return BUILDER->CreateCall(false_signature, length_method_value, args, "");
}

void* Code_generation_visitor::visit(New* new_){
    Function *new_function = MODULE->getFunction(new_->get_type() + "..new");

    return BUILDER->CreateCall(new_function);
}

void* Code_generation_visitor::visit(String* string_){
    return BUILDER->CreateGlobalStringPtr(string_->get_value());
}

void* Code_generation_visitor::visit(Integer* integer){
    return BUILDER->getInt32(integer->get_value());
}

void* Code_generation_visitor::visit(Boolean* boolean){
    return BUILDER->getInt1(boolean->get_value());
}

void* Code_generation_visitor::visit(Unit* unit){
    unit->get_column();
    return NULL;
}

void* Code_generation_visitor::visit(Object* object){
    return get_variable(object->get_name());
}

/*********************************************************************************/

Value* Code_generation_visitor::load(Value* object, string name){
    Value* ptr = get_pointer(object, name);
    return BUILDER->CreateLoad(ptr, "");
}

Value* Code_generation_visitor::get_pointer(Value* object, string name){
    return BUILDER->CreateGEP(
            object, 
            {BUILDER->getInt32(0), 
            BUILDER->getInt32(current_class->field_indexes[name])},
            "");
}

Value* Code_generation_visitor::load(Value* object, uint position){
    Value* ptr = get_pointer(object, position);
    return BUILDER->CreateLoad(ptr, "");
}

Value* Code_generation_visitor::get_pointer(Value* object){
    return BUILDER->CreateGEP(
            object, 
            {BUILDER->getInt32(0)},
            "");
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

Function* Code_generation_visitor::get_function(string class_name, string method){
    return current_class->functions[method + "_" + class_name];
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
    return value->getName().str();
}

Value *Code_generation_visitor::get_variable(string name){
    if(current_vtable.count(name))
        return current_vtable[name];

    // if not in v_table, it is in self
    return load(get_function_args()[0], name);
}

Value *Code_generation_visitor::get_variable_ptr(string name){
    if(current_vtable.count(name))
        return get_pointer(current_vtable[name]);

    // if not in v_table, it is in self
    return get_pointer(get_function_args()[0], name);
}

Value *Code_generation_visitor::get_field(string name){
    vector<Value *> args_values = get_function_args();

    Value *self_value = args_values[0];

    return load(self_value, name);
}

BasicBlock * Code_generation_visitor::get_current_block(){
    return BUILDER->GetInsertBlock();
}

BasicBlock * Code_generation_visitor::make_new_block(){
    return make_new_block(get_function());
}

BasicBlock * Code_generation_visitor::make_new_block(Function *function){
    // ==== ==== ==== Define block
    BasicBlock *function_block = BasicBlock::Create(
        *CONTEXT,         // The LLVM context
        "",             // The label of the block
        function);        // The function in which should be inserted the block
    
    // ==== ==== ==== Define Builder
    BUILDER->SetInsertPoint(function_block);

    return function_block;
}

BasicBlock * Code_generation_visitor::make_next_block(){
    return make_next_block(get_function());
}

BasicBlock * Code_generation_visitor::make_next_block(Function *function){
    // ==== ==== ==== Define block
    BasicBlock *function_block = BasicBlock::Create(
        *CONTEXT,         // The LLVM context
        "",             // The label of the block
        function);        // The function in which should be inserted the block
    
    // ==== ==== ==== Define Builder
    BUILDER->CreateBr(function_block);
    BUILDER->SetInsertPoint(function_block);

    return function_block;
}

void Code_generation_visitor::set_return_value(Value *return_value){

    if(return_value == NULL)
        BUILDER->CreateRetVoid();
    else
        BUILDER->CreateRet(
            return_value
        );
}