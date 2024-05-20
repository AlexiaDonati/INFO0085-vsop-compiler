#include "ast.hpp"
#include "code_generation_visitor.hpp"
#include <iostream>
#include <sstream>

using namespace AST;
using namespace std;
using namespace llvm;

#define BUILDER llvm_instance->builder
#define MODULE llvm_instance->module
#define CONTEXT llvm_instance->context

#define LOG(value) llvm_instance->print(value)

string replace_in_string(string raw_string, string replace_target, string replacement);
bool is_object_type(string type);

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

    current_program = program;
    
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

    // add the return value to the init function
    Function *init_function = MODULE->getFunction(current_class->get_name() + "___init");
    BasicBlock *last_block = &init_function->getBasicBlockList().back();
    BasicBlock *previous_block = get_current_block();
    BUILDER->SetInsertPoint(last_block);
    set_return_value(get_function_args(init_function)[0]);
    BUILDER->SetInsertPoint(previous_block);

    // Make methods
    this->accept_list(class_->get_method_list());

    return NULL;
}

void* Code_generation_visitor::visit(Field* field){
    if(!field->has_init_expr())
        return NULL;

    Function *init_function = MODULE->getFunction(current_class->get_name() + "___init");

    BasicBlock *last_block = &init_function->getBasicBlockList().back();

    BasicBlock *previous_block = get_current_block();

    BUILDER->SetInsertPoint(last_block);

    Value* init_value = (Value *) field->get_init_expr()->accept(this);

    Value *field_ptr = get_variable_ptr(field->get_name());

    BUILDER->CreateStore(init_value, field_ptr);

    BUILDER->SetInsertPoint(previous_block);

    return field_ptr;
}

void* Code_generation_visitor::visit(Method* method){
    // Clear v_table (because do not need variable of other methods)
    current_vtable.clear();

    // Put args in v_table
    Function *method_function = get_function(current_class->get_name(), method->get_name());

    vector<Value *> args_values = get_function_args(method_function);

    make_new_block(method_function);
    
    for (auto arg_value : args_values){       
        current_vtable[get_name(arg_value)] = BUILDER->CreateAlloca(arg_value->getType());
        BUILDER->CreateStore(arg_value, current_vtable[get_name(arg_value)]);
    }

    // Make block then implement it

    Value *return_value = (Value *) method->get_body_block()->accept(this);
    
    set_return_value(return_value);

    // Remove args from v_table
    for (auto arg_value : args_values)
        current_vtable.erase(get_name(arg_value));

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
    // Prepare casting if needed
    string else_return_type = "";
    string then_return_type = "";
    bool must_cast_else = false;
    bool must_cast_then = false;
    string cast_type = "";
    if(if_->has_else_expr()){
        then_return_type = get_type_string(if_->get_then_expr());
        else_return_type = get_type_string(if_->get_else_expr());
        if(is_parent(then_return_type, else_return_type)){
            then_return_type = true;
            cast_type = else_return_type;
        }
        else if(is_parent(else_return_type, then_return_type)){
            must_cast_else = true;
            cast_type = then_return_type;
        }
    }


    // Make the condition block
    BasicBlock *cond_block = get_current_block();

    Value *cond_value = (Value*) if_->get_cond_expr()->accept(this);

    // Make the after block
    BasicBlock *after_block = make_new_block();

    // Make the then block
    BasicBlock *then_block = make_new_block();

    Value *then_value = (Value*)if_->get_then_expr()->accept(this);

    if(must_cast_then){
        Type *cast_destination_type = llvm_instance->get_type(else_return_type);
        then_value = BUILDER->CreateBitCast(then_value, cast_destination_type, "cast");
    }

    BUILDER->CreateBr(after_block);

    // Make the else block
    BasicBlock *else_block = NULL;
    Value *else_value = NULL;
    if(if_->has_else_expr()){
        else_block = make_new_block();

        else_value = (Value*)if_->get_else_expr()->accept(this);

        if(must_cast_else){
            Type *cast_destination_type = llvm_instance->get_type(then_return_type);
            else_value = BUILDER->CreateBitCast(else_value, cast_destination_type, "cast");
        }

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
    if(get_type_string(if_) == "unit")
        return get_unit_value();

    // use phi function to get the return value
    PHINode* phi_node = BUILDER->CreatePHI(then_value->getType(), 2, "phi");
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
    
    return get_unit_value();
}

void* Code_generation_visitor::visit(Let* let){
    // add new variable to vtable and keep the old if there is a collition on names
    string name = let->get_name();
    Value *old_variable = NULL;
    if(current_vtable.count(name))
        old_variable = current_vtable[name];

    // initialize if needed
    Value *new_ptr = BUILDER->CreateAlloca(llvm_instance->get_type(let->get_type()));
    if(let->has_init_expr()){
        Value* variable_new_value = load((Value*) let->get_init_expr()->accept(this));
        new_ptr = assign_value(variable_new_value, new_ptr);
    }

    current_vtable[name] = new_ptr;

    // execute block
    Value* return_value = (Value*) let->get_scope_expr()->accept(this);

    // Remove new and reload old variable if collision
    current_vtable.erase(name);

    if(old_variable != NULL)
        current_vtable[name] = old_variable;  

    return return_value;
}

void* Code_generation_visitor::visit(Assign* assign){
    Value* expr_value = load((Value *) assign->get_expr()->accept(this));

    Value *variable_ptr = get_variable_ptr(assign->get_name());

    Value *new_value = assign_value(expr_value, variable_ptr);

    if(current_vtable.count(assign->get_name()))
        current_vtable[assign->get_name()] = new_value;

    return new_value;
}

Value *Code_generation_visitor::assign_value(Value* new_value, Value *destination_ptr){
    // Cast new_value
    Type *cast_destination_type = load(destination_ptr)->getType();
    if(cast_destination_type->isPointerTy())
        new_value = BUILDER->CreateBitCast(new_value, cast_destination_type, "cast");

    BUILDER->CreateStore(new_value, destination_ptr);

    return destination_ptr;
}

void* Code_generation_visitor::visit(Self* self){
    return current_vtable["self"];
}

void* Code_generation_visitor::visit(Unop* unop){
    string op = unop->get_op();

    Value *result = NULL;

    Value *expr = load((Value*) unop->get_expr()->accept(this));

    switch (unop_to_enum.at(op)){
    case NOT:
        result = BUILDER->CreateNot(expr, "not");
        break;
    case UNARY:
        result = BUILDER->CreateNeg(expr, "neg");
        break;
    case ISNULL:
        result = BUILDER->CreateIsNull(expr, "isnull");
        break;
    default:
        break;
    }

    return result;
}

void* Code_generation_visitor::visit(Binop* binop){
    string op = binop->get_op();

    Value *result = NULL;

    Value *left = load((Value*) binop->get_left_expr()->accept(this));
    Value *right = load((Value*) binop->get_right_expr()->accept(this));

    switch (binop_to_enum.at(op)){
    case EQ:
        result = BUILDER->CreateICmpEQ(left, right, "eq");
        break;
    case LT:
        result = BUILDER->CreateICmpSLT(left, right, "slt");
        break;
    case LEQ:
        result = BUILDER->CreateICmpSLE(left, right, "sle");
        break;
    case ADD:
        result = BUILDER->CreateAdd(left, right, "add");
        break;
    case SUB:
        result = BUILDER->CreateSub(left, right, "sub");
        break;
    case MUL:
        result = BUILDER->CreateMul(left, right, "mul");
        break;
    case DIV:
        result = BUILDER->CreateSDiv(left, right, "div");
        break;
    case POW:
        {
            vector<Value *> args;
            args.push_back(left);
            args.push_back(right);
            Function *pow_function = MODULE->getFunction("pow");
            result = BUILDER->CreateCall(pow_function, args, "pow");
        }
        break;
    case AND:
        result = BUILDER->CreateAnd(left, right, "and");
        break;
    default:
        break;
    }

    return result;
}

void* Code_generation_visitor::visit(Call* call){
    Value *object = (Value*) call->get_object()->accept(this);
    // Position of the method in the m table
    string object_type = get_type_string(call->get_object());
    uint position = get_class(object_type)->method_indexes[call->get_method()];
    vector<Value *> args = this->accept_list(call->get_arg_expr_list());
    // Load the value of mtable of object
    Value* m_table_value = load(load(object), 0); // m table always at index 0
    // Load object.method() in object m_tables
    Value* method_value = load(m_table_value, position);

    FunctionType *signature = get_class(object_type)->method_signatures[call->get_method()];


    // Cast object to the right type
    vector<Value *> casted_args;
    {
        Type *cast_destination_type = signature->getParamType(0);
        casted_args.push_back(BUILDER->CreateBitCast(load(object), cast_destination_type, "cast"));
    }
    int i = 1;
    for(auto arg : args){
        List<Expr> *args_expr = call->get_arg_expr_list();
        if(get_type_string(args_expr->get_element(i-1)) == "string"){
            Type *cast_destination_type = signature->getParamType(i);
            casted_args.push_back(BUILDER->CreateBitCast(arg, cast_destination_type, "cast"));
        }
        else if(is_object_type(get_type_string(args_expr->get_element(i-1)))){
            Type *cast_destination_type = signature->getParamType(i);
            casted_args.push_back(BUILDER->CreateBitCast(load(arg), cast_destination_type, "cast"));
        } else {
            casted_args.push_back(load(arg));
        }
        i++;
    }

    Value *result_value = BUILDER->CreateCall(signature, method_value, casted_args, "call");

    Value *result_ptr = BUILDER->CreateAlloca(result_value->getType());
    BUILDER->CreateStore(result_value, result_ptr);
    
    return result_ptr;
}

void* Code_generation_visitor::visit(New* new_){
    Function *new_function = MODULE->getFunction(new_->get_type() + "___new");

    return BUILDER->CreateCall(new_function);
}

void* Code_generation_visitor::visit(String* string_){
    string raw_string = string_->get_value();

    // replace all hex by the value
    string new_string = replace_in_string(raw_string, "\\x08", "\x08"); // b
    new_string = replace_in_string(new_string, "\\x09", "\x09"); // t
    new_string = replace_in_string(new_string, "\\x0a", "\x0a"); // n
    new_string = replace_in_string(new_string, "\\x0d", "\x0d"); // r
    new_string = replace_in_string(new_string, "\\x22", "\x22"); // "
    new_string = replace_in_string(new_string, "\\x5c", "\x5c"); // \\
    new_string = replace_in_string(new_string, "\\x7F", "\\x7F");

    for(int i = 0; i <= 0x1F; i++){
        stringstream ss;
        ss << "\\x" << hex << i;
        string real_value;
        real_value.push_back((char) i);
        new_string = replace_in_string(new_string, ss.str(), real_value);
    }
    
    return BUILDER->CreateGlobalStringPtr(new_string);
}

void* Code_generation_visitor::visit(Integer* integer){
    return BUILDER->getInt32(integer->get_value());
}

void* Code_generation_visitor::visit(Boolean* boolean){
    return BUILDER->getInt1(boolean->get_value());
}

void* Code_generation_visitor::visit(Unit* unit){
    unit->get_column();
    return get_unit_value();
}

void* Code_generation_visitor::visit(Object* object){
    string name = object->get_name();
    if(current_vtable.count(name))
        return current_vtable[name];

    // if not in v_table, it is in self
    return load(get_function_args()[0], name);
}

/*********************************************************************************/

Value* Code_generation_visitor::load(Value* object, string name){
    Value* ptr = get_pointer(object, name);
    return BUILDER->CreateLoad(ptr, "value");
}

Value* Code_generation_visitor::get_pointer(Value* object, string name){
    return BUILDER->CreateGEP(
            object, 
            {BUILDER->getInt32(0), 
            BUILDER->getInt32(current_class->field_indexes[name])},
            "ptr");
}

Value* Code_generation_visitor::load(Value* object, uint position){
    Value* ptr = get_pointer(object, position);
    return BUILDER->CreateLoad(ptr, "value");
}

Value* Code_generation_visitor::load(Value* ptr){
    if (!ptr->getType()->isPointerTy()) {
        return ptr;
    } 
    return BUILDER->CreateLoad(ptr, "value");
}

Value* Code_generation_visitor::get_pointer(Value* object, uint position){
    return BUILDER->CreateGEP(
            object, 
            {BUILDER->getInt32(0), 
            BUILDER->getInt32(position)},
            "ptr");
}

string Code_generation_visitor::get_type_string(Expr *expr){
    return table->find_expr_table(expr)->type_to_string();
}

Function* Code_generation_visitor::get_function(string class_name, string method){
    return current_class->functions[class_name + "__" + method];
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
        return load(current_vtable[name]);

    // if not in v_table, it is in self
    return load(get_function_args()[0], name);
}

Value *Code_generation_visitor::get_variable_ptr(string name){
    if(current_vtable.count(name))
        return current_vtable[name];

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
        "block",             // The label of the block
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
        "block",             // The label of the block
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

bool Code_generation_visitor::is_parent(string child, string parent){
    if(parent == "Object")
        return true;
    AST::Class *child_class = current_program->class_map[child];
    AST::Class *parent_class = current_program->class_map[parent];

    if(child_class == NULL || parent_class == NULL)
        return false;

    AST::Class *true_parent = current_program->class_map[child_class->get_parent()];
    while(true){
        if(true_parent->get_name() == parent_class->get_name())
            return true;
        if (true_parent->get_name() == "Object")
            break;
        true_parent = current_program->class_map[true_parent->get_parent()];
    }
    return false;
}

Class *Code_generation_visitor::get_class(string class_name){
    return current_program->class_map[class_name];
}

bool Code_generation_visitor::is_field(string name){
    if(current_vtable.count(name))
        return false;

    // if not in v_table, it is in self
    return true;
}

Value *Code_generation_visitor::get_unit_value(){
    return ConstantPointerNull::get(dyn_cast<PointerType>(llvm_instance->get_type("unit")));
}

/**************************/

string replace_in_string(string raw_string, string replace_target, string replacement){
    string new_string = raw_string;
    size_t position = new_string.find(replace_target);
    while(position != string::npos){
        string left = new_string.substr(0, position);
        string right = new_string.substr(position + replace_target.length());
        new_string = left + replacement + right;
        position = new_string.find(replace_target);
    }
    return new_string;
}

bool is_object_type(string type){
    if(type == "int32"
        || type == "bool"
        || type == "unit"
        || type == "")
        return false;
    return true;
}
