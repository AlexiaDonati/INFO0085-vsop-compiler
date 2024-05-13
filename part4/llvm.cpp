#include "llvm.hpp"

using namespace llvm;

LLVM *LLVM::instance = nullptr;

LLVM::LLVM(AST::Program* program, const std::string &fileName): fileName(fileName){
    if(program == nullptr){
        errs() << "Error: Program is null\n";
        return;
    }

    context = new LLVMContext();
    module = new Module(fileName, *context);
    builder = new IRBuilder<>(*context);

{
/******* Class List *******/

// Structure--------------------------
    string class_name = "List";
    StructType* class_type = StructType::create(*context, class_name);
    StructType *m_table_type = create_mtable(class_name);

// Fields--------------------------
// ==== Define the list of fields
    vector<Type *> fields_types;

// ==== Define fields
    // No fields

// ==== Push the field in the class structure
    class_type->setBody(fields_types);

// Methods--------------------------
    vector<Type *> methods_types;
    vector<Constant *> methods;
// ==== isNil()
    vector<string> args_name;
    vector<string> args_type;
    Function* method_function = make_method(    
        args_name, 
        args_type, 
        "bool", 
        class_name, 
        class_type,
        "isNil");

    // put in m_table
    methods_types.push_back(method_function->getFunctionType()->getPointerTo());
    methods.push_back(method_function);

    IRBuilder<> builder(*context);

    make_function_block(builder, "entry", method_function);

    vector<Value *> arguments_values = get_function_args(method_function);

    set_return_value(builder, true);

// ==== length()
    method_function = make_method(    
        args_name, 
        args_type, 
        "int32", 
        class_name, 
        class_type,
        "length");

    // put in m_table
    methods_types.push_back(method_function->getFunctionType()->getPointerTo());
    methods.push_back(method_function);

    make_function_block(builder, "entry", method_function);

    arguments_values = get_function_args(method_function);

    set_return_value(builder, 0);

// M table save---------------------------
    save_m_table(m_table_type, class_name, methods_types, methods);
    
/**************************/
}
{
/******* Class Nil extends List *******/

// Structure--------------------------
    string class_name = "Nil";
    string parent_class_name = "List";
    StructType* class_type = StructType::create(*context, class_name);

// Fields--------------------------
// ==== Define the list of fields
    vector<Type *> fields_types;

// ==== Define fields
    // No fields

// ==== Push the field in the class structure
    class_type->setBody(fields_types);

// Methods--------------------------
// ==== Inherits parents methods
// ==== ==== isNil()
    string method_name = "isNil";
    string true_method_name = parent_class_name + "." + method_name;
    Function* parent_function = module->getFunction(true_method_name);

    vector<string> args_name;
    vector<string> args_type;
    Function* method_function = make_method(    
        args_name, 
        args_type, 
        "bool", 
        class_name, 
        class_type,
        method_name);

    IRBuilder<> builder(*context);

    make_function_block(builder, "entry", method_function);

    vector<Value *> arguments_values = get_function_args(method_function);

    Value *result = builder.CreateCall(parent_function);

    set_return_value(builder, result);

// ==== ==== length()
    method_name = "length";
    true_method_name = parent_class_name + "." + method_name;
    parent_function = module->getFunction(true_method_name);

    method_function = make_method(    
        args_name, 
        args_type, 
        "int32", 
        class_name, 
        class_type,
        method_name);

    make_function_block(builder, "entry", method_function);

    arguments_values = get_function_args(method_function);

    result = builder.CreateCall(parent_function);

    set_return_value(builder, result);
    
/**************************/
}
}

void LLVM::optimize(){

}

void LLVM::print(){
    string output;

    raw_string_ostream os(output);
    module->print(os, nullptr);

    std::cout << output << std::endl;
}

void LLVM::executable(const string &fileName){

}

Function* LLVM::make_method(    
    vector<string> args_name, 
    vector<string> args_type, 
    string return_type, 
    string class_name,
    StructType* class_type,
    string method_name){

    if(args_name.size() != args_type.size()){
        fprintf(stderr, "Error: make_method -> args_name and args_type must be the same size\n");
        return NULL;
    }

// ==== ==== Define args
    vector<Type *> method_arguments;

    // First arg is always Self
    Type *self = class_type->getPointerTo();
    method_arguments.push_back(self);

    for (auto type : args_type)
        method_arguments.push_back(get_type(type));

// ==== ==== Create signature
    string true_method_name = class_name + "." + method_name;

    FunctionType *method_signature = FunctionType::get(
        get_type(return_type),        // Return type
        method_arguments,             // List of arguments types
        false);                       // No variable number of arguments

// ==== ==== Define function
    Function *method_function = Function::Create(
        method_signature,                  // The signature
        GlobalValue::ExternalLinkage,      // The linkage (not important here)
        true_method_name,                       // The name
        module);                           // The LLVM module

// ==== ==== Name args
    int i = 0;
    for (auto &arg : method_function->args()){
        switch(i){
            case 0:
                arg.setName("self");
                break;
            default:
                arg.setName(args_name[i-1]);
                break;
        }
        i++;
    }

    return method_function;
}

void LLVM::make_function_block(IRBuilder<>& builder, string name, Function *function){
    // ==== ==== ==== Define block
    BasicBlock *function_block = BasicBlock::Create(
        *context,         // The LLVM context
        name,             // The label of the block
        function);        // The function in which should be inserted the block
    
    // ==== ==== ==== Define Builder
    builder.SetInsertPoint(function_block);
}

vector<Value *> LLVM::get_function_args(Function *function){
    vector<Value *> arguments_values;
    for (auto &arg : function->args())
        arguments_values.push_back(&arg);
    return arguments_values;
}

StructType * LLVM::create_mtable(string class_name){
    string mtable_name = "struct." + class_name + "_MTable";
    return StructType::create(*context, mtable_name);
}

void LLVM::save_m_table(StructType *mtable_type, string class_name, vector<Type *> methods_types, vector<Constant *> methods){
    mtable_type->setBody(methods_types);

    // Create a constant
    Constant *mtable_const = ConstantStruct::get(
        mtable_type, // Type of the constant structure
        methods);    // Values to give to the different fields

    // Assign the constant to a global variable
    GlobalVariable *vtable = new GlobalVariable(
        *module,                      // The LLVM module
        mtable_type,                  // The type of the constant
        true,                        // It is constant
        GlobalValue::InternalLinkage, // The linkage
        mtable_const,                 // The constant value
        class_name + "_mtable");      // The name of the variable
}

void LLVM::set_return_value(IRBuilder<>& builder, bool return_value){
    builder.CreateRet(
        ConstantInt::get(
            Type::getInt1Ty(*context), 
            (return_value) ? 1 : 0
        )
    );
}

void LLVM::set_return_value(IRBuilder<>& builder, int return_value){
    builder.CreateRet(
        ConstantInt::get(
            Type::getInt32Ty(*context), 
            return_value
        )
    );
}

void LLVM::set_return_value(IRBuilder<>& builder, Value *return_value){
    builder.CreateRet(
        return_value
    );
}