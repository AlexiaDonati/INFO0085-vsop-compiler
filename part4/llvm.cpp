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
// ==== Define the list of methods
    /*vector<Type *> methods_types;
    map<string, Function *> methods_functions;
    map<string, FunctionType *> methods_signatures;*/

// ==== isNil()
vector<string> args_name;
vector<string> args_type;
make_method(    
    args_name, 
    args_type, 
    "bool", 
    "List", 
    class_type,
    "isNil");

args_name.push_back("test");
make_method(    
    args_name, 
    args_type, 
    "bool", 
    "List", 
    class_type,
    "length");

/*
// ==== ==== Define args
    vector<Type *> method_arguments;

    // First arg is always Self
    Type *self = class_type->getPointerTo();
    method_arguments.push_back(self);

// ==== ==== Create signature
    string method_return_type = "bool";
    string method_name = class_name + "." + "isNil";

    FunctionType *method_signature = FunctionType::get(
        get_type(method_return_type), // Return type
        method_arguments,             // List of arguments types
        false);                       // No variable number of arguments

    methods_types.push_back(method_signature->getPointerTo());
    methods_signatures[method_name] = method_signature;

// ==== ==== Define function
    Function *method_function = Function::Create(
        method_signature,                  // The signature
        GlobalValue::ExternalLinkage,      // The linkage (not important here)
        method_name,                       // The name
        module);                           // The LLVM module

    methods_functions[method_name] = method_function;

// ==== ==== Name args
    int i = 0;
    for (auto &arg : method_function->args()){
        switch(i){
            case 0:
                arg.setName("self");
                break;
            default:
                break;
        }
        i++;
    }

// ==== ==== Implementation (block creation)
// ==== ==== ==== Define block
    string block_name = method_name + ".block";
    BasicBlock *function_block = BasicBlock::Create(
        *context,         // The LLVM context
        block_name,       // The label of the block
        method_function); // The function in which should be inserted the block

// ==== ==== ==== Define Builder
    IRBuilder<> builder(*context);
    builder.SetInsertPoint(function_block);

// ==== ==== ==== Get args
    vector<Value *> arguments_values;
    for (auto &arg : method_function->args())
        arguments_values.push_back(&arg);

// ==== ==== ==== Set return value
    bool return_value = true;
    builder.CreateRet(
        ConstantInt::get(
            Type::getInt1Ty(*context), 
            (return_value) ? 1 : 0
        )
    );*/
    
/**************************/

    /*

    context = new LLVMContext(); // Used to generate the LLVM module and the types

    module = new Module(fileName, *context); // Module containing the LLVM IR (compilation unit)
    builder = new IRBuilder<>(*context); // Used to generate the LLVM IR
    
    AST::List<AST::Class> *classes = program->get_class_list();
    for (size_t i = 0; i < classes->get_size(); ++i){
        AST::Class *current_class = classes->get_element(i);

        StructType *structure_class = StructType::create(*context, current_class->get_name()); // Declare structure for class
        StructType *structure_vtable = StructType::create(*context, current_class->get_name() + "_vtable"); // Declare VTable structure for class
        
        // Create the types -----------------
        
        // Create the types for the fields
        std::vector<Type *> types_fields = std::vector<Type *>();

        AST::List<AST::Field> *field_list = current_class->get_field_list();
        for (size_t j = 0; j < field_list->get_size(); ++j){
            AST::Field *field = field_list->get_element(j);

            types_fields.push_back(get_type(field->get_type()));
        }
        StructType::create(*context, types_fields, current_class->get_name());
        
        // Create the types for the methods
        AST::List<AST::Method> *method_list = current_class->get_method_list();
        for (size_t j = 0; j < method_list->get_size(); ++j){
            AST::Method *method = method_list->get_element(j);
            
            std::vector<Type *> types_methods = std::vector<Type *>();
            types_methods.push_back(get_type(current_class->get_name()));
            
            AST::List<AST::Formal> *formal_list = method->get_formal_list();
            for (size_t k = 0; k < formal_list->get_size(); ++k){
                AST::Formal *formal = formal_list->get_element(k);
                types_methods.push_back(get_type(formal->get_type()));
            }
            
            FunctionType *functionType = FunctionType::get(get_type(method->get_return_type()), types_methods, false);
            module->getOrInsertFunction(current_class->get_name() + "__" + method->get_name(), functionType);
        }
        
        // Inheritance -----------------

        AST::Class *parent = current_class;

        // inherited fields 
        std::vector<Type *> parent_types_fields = std::vector<Type *>();
        parent_types_fields.push_back(PointerType::get(structure_vtable, 0));

        // inherited methods
        std::vector<Type *> parent_types_methods = std::vector<Type *>();
        std::vector<Constant*> methods = std::vector<Constant*>();
        
        std::map<std::string, bool> defined;
        std::map<std::string, int> method_indexes;

        int current_index = 0;
        
        while(true){
            
            // inherited fields 
            AST::List<AST::Field> *parent_fields = parent->get_field_list();
            for(size_t y = 0; y < parent_fields->get_size(); ++y){
                AST::Field *field = parent_fields->get_element(y);
                parent_types_fields.push_back(get_type(field->get_type()));
            }

            // inherited methods
            AST::List<AST::Method> *parent_methods = parent->get_method_list();
            for(size_t y = 0; y < parent_methods->get_size(); ++y){

                AST::Method *method = parent_methods->get_element(y); 

                if(!defined[method->get_name()]){ // if it is not overriding
                    defined[method->get_name()] = true;

                    method_indexes[method->get_name()] = current_index; // save the index
                    current_index++; // increment the index for the next method

                    methods.push_back(module->getFunction(parent->get_name() + "__" + method->get_name()));

                    auto type = ((Function*) methods[i])->getFunctionType();
                    parent_types_methods.push_back(PointerType::get(type, 0)); // add the type
                }
                else{ // if it is overriding
                    int index = method_indexes[method->get_name()]; // get the index
                    methods[index] = module->getFunction(parent->get_name() + "__" + method->get_name()); // update the method

                    auto type = ((Function*) methods[index])->getFunctionType(); 
                    parent_types_methods[index] = PointerType::get(type, 0); // update the type
                }
            }
            
            if (parent->get_parent() == "Object"){
                break;
            }
            for(size_t c = 0; c < classes->get_size(); ++c){
                if(classes->get_element(c)->get_name() == parent->get_parent()){
                    parent = classes->get_element(c);
                    break;
                }
            }
        }
        structure_vtable->setBody(parent_types_methods);
        structure_class->setBody(parent_types_fields);
    }*/
    
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

void LLVM::make_method(    
    vector<string> args_name, 
    vector<string> args_type, 
    string return_type, 
    string class_name,
    StructType* class_type,
    string method_name){

    if(args_name.size() != args_type.size()){
        fprintf(stderr, "Error: make_method -> args_name and args_type must be the same size");
        return;
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

// ==== ==== Implementation (block creation)
// ==== ==== ==== Define block
    string block_name = true_method_name + ".block";
    BasicBlock *function_block = BasicBlock::Create(
        *context,         // The LLVM context
        block_name,       // The label of the block
        method_function); // The function in which should be inserted the block

// ==== ==== ==== Define Builder
    IRBuilder<> builder(*context);
    builder.SetInsertPoint(function_block);

// ==== ==== ==== Get args
    vector<Value *> arguments_values;
    for (auto &arg : method_function->args())
        arguments_values.push_back(&arg);

// ==== ==== ==== Set return value
    bool return_value = true;
    builder.CreateRet(
        ConstantInt::get(
            Type::getInt1Ty(*context), 
            (return_value) ? 1 : 0
        )
    );
}