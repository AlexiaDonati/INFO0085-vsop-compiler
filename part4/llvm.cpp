#include "llvm.hpp"

using namespace llvm;

LLVM *LLVM::instance = nullptr;

LLVM::LLVM(AST::Program* program, const std::string &fileName): fileName(fileName){
    if(program == nullptr){
        errs() << "Error: Program is null\n";
        return;
    }

    context = new LLVMContext(); // Used to generate the LLVM module and the types

    module = new Module(fileName, *context); // Module containing the LLVM IR (compilation unit)
    builder = new IRBuilder<>(*context); // Used to generate the LLVM IR
    
    /****************** Declare 'malloc' function ******************/
    // void *malloc(size_t size);

    // Signature
    FunctionType *malloc_sign = FunctionType::get(
        Type::getInt8Ty(*context)->getPointerTo(), // The return type
        {Type::getInt64Ty(*context)},              // The arguments
        false);                                    // No variable number of arguments

    // Declare the function
    Function::Create(
        malloc_sign,                  // The signature
        GlobalValue::ExternalLinkage, // The linkage: external, malloc is implemented elsewhere
        "malloc",                     // The name
        module);                      // The LLVM module
    
    /****************** Declare 'pow' function ******************/
    // int pow(int x, int y);

    // Signature
    FunctionType *pow_sign = FunctionType::get(
        Type::getInt32Ty(*context),                                       // The return type
        {Type::getInt32Ty(*context), Type::getInt32Ty(*context)},   // The arguments
        false);                                                                 // No variable number of arguments

    // Declare the function                         
    Function::Create(
        pow_sign,                       // The signature
        GlobalValue::ExternalLinkage,   // The linkage: external, pow is implemented elsewhere
        "pow",                          // The name
        module);                        // The LLVM module

    /****************** Define the Classes ******************/
    StructType *class_type = nullptr; // Declare structure for class
    StructType *vtable_type = nullptr; // Declare VTable structure for class
    
    for (auto &class_: program->class_map){

        AST::Class *current_class = class_.second;

        /****************** Define the Class structure ******************/
        // Initialize the class structure
        class_type = StructType::create(*context, current_class->get_name()); // Declare structure for class
        
        // Initialize the VTable structure
        vtable_type = StructType::create(*context, "struct." + current_class->get_name() + "Vtable"); // Declare VTable structure for class

        /************** Fields **************/
        std::vector<Type *> class_fields = std::vector<Type *>();
        
        // First field is the pointer towards the vtable
        class_fields.push_back(vtable_type->getPointerTo());

        current_class->field_indexes["self"] = (uint32_t) 0;
        uint32_t field_index = 1;

        AST::Class *parent = current_class;
        while (true){ // Iterate over the class and its parents starting from the current class to the root class 'Object'

            AST::List<AST::Field> *field_list = parent->get_field_list();
            for (size_t j = 0; j < field_list->get_size(); ++j){ // Iterate over the fields of the class
                AST::Field *field = field_list->get_element(j);

                // Insert the fields
                class_fields.push_back(get_type(field->get_type()));
                current_class->field_indexes[field->get_name()] = field_index;
                field_index++;
            }

            if(parent->get_name() == "Object"){
                break;
            }
            parent = program->class_map[parent->get_parent()];
        }
    
        class_type->setBody(class_fields);

        /************** Create the Methods **************/
        AST::List<AST::Method> *method_list = current_class->get_method_list();
        for (size_t j = 0; j < method_list->get_size(); ++j){
            AST::Method *method = method_list->get_element(j);
            
            std::vector<Type *> method_arguments = std::vector<Type *>();
            
            // First argument is always 'self'
            method_arguments.push_back(class_type->getPointerTo());
            
            AST::List<AST::Formal> *formal_list = method->get_formal_list();
            for (size_t k = 0; k < formal_list->get_size(); ++k){
                AST::Formal *formal = formal_list->get_element(k);
                
                // Insert the other arguments
                method_arguments.push_back(get_type(formal->get_type()));
            }
            
            // Create signature
            FunctionType *method_type = FunctionType::get(get_type(
                method->get_return_type()), // Return type
                method_arguments,           // List of arguments types
                false);                     // No variable number of arguments

            // Create the actual function that will implement the method
            Function *method_function = Function::Create(
                method_type,                                            // The signature
                GlobalValue::ExternalLinkage,                           // The linkage (not important here)
                method->get_name() + "_" + current_class->get_name(),   // The name
                module);                                                // The LLVM module
            
            // Setting the name of the arguments
            auto arg_it = method_function->arg_begin();
            arg_it->setName("self");
            ++arg_it;
            for (size_t k = 0; k < formal_list->get_size(); ++k){
                arg_it->setName(formal_list->get_element(k)->get_name());
                ++arg_it;
            }

            current_class->functions[method->get_name() + "_" + current_class->get_name()] = method_function;
        }
    }

    /************** Declare the methods as part of the class **************/
    for (auto &class_: program->class_map){
        AST::Class *current_class = class_.second;

        vtable_type = module->getTypeByName("struct." + current_class->get_name() + "Vtable");
        class_type = module->getTypeByName(current_class->get_name());

        std::vector<Type *> methods_types;
        vector<Constant *> methods;
        uint32_t method_index = 0;

        AST::Class *parent = current_class;
        while(true){ // Iterate over the class and its parents starting from the current class to the root class 'Object'

            AST::List<AST::Method> *parent_methods = parent->get_method_list();
            for(size_t y = 0; y < parent_methods->get_size(); ++y){
                AST::Method *method = parent_methods->get_element(y); 

                bool got_overriden = current_class->method_signatures.find(method->get_name()) != current_class->method_signatures.end();
                if(!got_overriden){
                    Function *function = module->getFunction(method->get_name() + "_" + parent->get_name());
                    methods.push_back(function);

                    FunctionType *type = function->getFunctionType();
                    methods_types.push_back(type->getPointerTo());

                    current_class->method_signatures[method->get_name()] = type;
                    method_indexes[current_class->get_name() + "." + method->get_name()] = method_index;
                    method_index++;
                }
            }

            if (parent->get_name() == "Object"){
                break;
            }
            parent = program->class_map[parent->get_parent()];
        } 
        vtable_type->setBody(methods_types);

        /************** New and Init functions **************/
        // Declare function 'new': allocate memory for a new object and return it.
        FunctionType *method_type = FunctionType::get(
            class_type->getPointerTo(), // The return type
            {},                         // The arguments (no self for new)
            false);                     // No variable number of arguments

        Function *new_function = Function::Create(
            method_type,                            // The signature
            GlobalValue::ExternalLinkage,           // The linkage
            current_class->get_name() + "..new",     // The name
            module);                                // The LLVM module

        // Declare function 'init': initialize an object.
        method_type = FunctionType::get(
            class_type->getPointerTo(),   // The return type
            {class_type->getPointerTo()}, // The arguments
            false);                       // No variable number of arguments

        Function *init_function = Function::Create(
            method_type,                            // The signature
            GlobalValue::ExternalLinkage,           // The linkage
            current_class->get_name() + "..init" ,    // The name
            module);                                // The LLVM module

        init_function->arg_begin()->setName("self");

        /******** Implement the 'new' function ********/
        // First create an entry point.
        BasicBlock *new_entry = BasicBlock::Create(
            *context,      // The LLVM context
            "entry",       // The label of the block
            new_function); // The function in which should be inserted the block
        
        builder->SetInsertPoint(new_entry);

        // Get the size of the object
        Value* ptr_size = builder->CreateGEP(
            class_type,                                             // The pointed type
            ConstantPointerNull::get(class_type->getPointerTo()),   // The address
            {builder->getInt32(1)},                                 // The index of the element we want
            "");                                                    // Name of the LLVM variable (not fixed here)

        Value* bytes_size = builder->CreatePointerCast(
            ptr_size,                       // The value to cast
            Type::getInt64Ty(*context),     // The casting type
            "");                            // Name of the LLVM variable (not fixed here)

        // Now that we have the size, we can call malloc:
        Value *struct_ptr = builder->CreateCall(
            module->getFunction("malloc"),  // Function to call
            {bytes_size});                  // Arguments

        // Malloc returns a i8 pointer, we have to cast it as a pointer towards our structure
        Value *cast_struct_ptr = builder->CreatePointerCast(
            struct_ptr,                     // The value to cast
            class_type->getPointerTo(),     // The casting type
            "");                            // Name of the LLVM variable (not fixed here)

        // The last thing to do is to return the initialized object
        builder->CreateRet(cast_struct_ptr);

        /******** Implement the 'init' function ********/

        BasicBlock *init_entry = BasicBlock::Create(
            *context,       // The LLVM context
            "entry",        // The label of the block
            init_function); // The function in which should be inserted the block

        builder->SetInsertPoint(init_entry);


        /************** Defining the mtable **************/
        // Create a constant
        Constant *mtable_const = ConstantStruct::get(
            vtable_type, // Type of the constant structure
            methods);    // Values to give to the different fields

        // Assign the constant to a global variable
        GlobalVariable *mtable = new GlobalVariable(
            *module,                      // The LLVM module
            vtable_type,                  // The type of the constant
            true,                         // It is constant
            GlobalValue::InternalLinkage, // The linkage
            mtable_const,                 // The constant value
            current_class->get_name() + "_mtable");      // The name of the variable

        /******** Initialize the mtable ********/

        Value* mtable_ptr = builder->CreateGEP(
            class_type,                 // The pointed type
            init_function->arg_begin(), // The address (self)
            {builder->getInt32(0),      // First element "of the array" (no array here, only one element, but it is required)
            builder->getInt32(0)}, 
            "");                        // Name of the LLVM variable (not fixed here)

        builder->CreateStore(mtable, mtable_ptr);    
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

void LLVM::print(Value *value){
    string output;

    raw_string_ostream os(output);
    value->print(os);

    std::cerr << output << std::endl;
}

void LLVM::executable(const string &fileName){

}