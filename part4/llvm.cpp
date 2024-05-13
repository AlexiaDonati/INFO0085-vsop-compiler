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

/******* Class List *******/

// Structure--------------------------
    string class_name = "List";
    StructType* class_type = StructType::create(*context, class_name);

// Fields--------------------------
// ==== Define the list of fields
    vector<Type *> fields_types;

// ==== Define fields
    // No fields

// ==== Push the field in the class structure
    class_type->setBody(fields_types);

// Methods--------------------------
// ==== isNil()
    vector<string> args_name;
    vector<string> args_type;
    Function* method_function = make_method(    
        args_name, 
        args_type, 
        "bool", 
        "List", 
        class_type,
        "isNil");

    IRBuilder<> builder(*context);

    make_function_block(builder, "entry", method_function);

    vector<Value *> arguments_values = get_function_args(method_function);

    set_return_value(builder, true);

// ==== length()
    method_function = make_method(    
        args_name, 
        args_type, 
        "int32", 
        "List", 
        class_type,
        "length");

    make_function_block(builder, "entry", method_function);

    arguments_values = get_function_args(method_function);

    set_return_value(builder, 0);
    
/**************************/
    
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