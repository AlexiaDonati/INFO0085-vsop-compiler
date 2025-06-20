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


    /****************** Define the Class structure ******************/
    for (auto &class_: program->class_map){
        StructType *class_type = StructType::create(*context, class_.first); // Declare structure for class
        StructType *vtable_type = StructType::create(*context, "struct." + class_.first + "Vtable"); // Declare VTable structure for class
    }
    
    for (auto &class_: program->class_map){
        StructType *class_type = module->getTypeByName(class_.first);
        StructType *vtable_type = module->getTypeByName("struct." + class_.first + "Vtable");

        AST::Class *current_class = class_.second;

        /************** Fields **************/
        std::vector<Type *> class_fields = std::vector<Type *>();
        
        // First field is the pointer towards the vtable
        class_fields.push_back(vtable_type->getPointerTo());

        current_class->field_indexes["self"] = (uint32_t) 0;
        uint32_t field_index = 1;

        AST::Class *parent = current_class;
        std::vector<AST::Class *> parents;
        parents.insert(parents.begin(), parent);

        // Get all parents
        while(true){
            if (parent->get_name() == "Object")
                break;
            parent = program->class_map[parent->get_parent()];
            parents.insert(parents.begin(), parent);
        }
        for (auto &parent : parents){ // Iterate over the class and its parents starting from the root class 'Object' to the current class

            AST::List<AST::Field> *field_list = parent->get_field_list();
            for (size_t j = 0; j < field_list->get_size(); ++j){ // Iterate over the fields of the class
                AST::Field *field = field_list->get_element(j);

                // Insert the fields
                class_fields.push_back(get_type(field->get_type()));
                current_class->field_indexes[field->get_name()] = field_index;
                field_index++;
            }
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
                current_class->get_name() + "__" + method->get_name(),   // The name
                module);                                                // The LLVM module
            
            // Setting the name of the arguments
            auto arg_it = method_function->arg_begin();
            arg_it->setName("self");
            ++arg_it;
            for (size_t k = 0; k < formal_list->get_size(); ++k){
                arg_it->setName(formal_list->get_element(k)->get_name());
                ++arg_it;
            }

            current_class->functions[current_class->get_name() + "__" + method->get_name()] = method_function;
        }
    }
    
    /************** Declare the methods as part of the class **************/
    for (auto &class_: program->class_map){
        AST::Class *current_class = class_.second;

        StructType *vtable_type = module->getTypeByName("struct." + current_class->get_name() + "Vtable");
        StructType *class_type = module->getTypeByName(current_class->get_name());

        std::vector<Type *> methods_types;
        vector<Constant *> methods;
        uint32_t method_index = 0;
        std::map<std::string, bool> override_another_method;

        AST::Class *parent = current_class;
        std::vector<AST::Class *> parents;
        parents.insert(parents.begin(), parent);

        // Get all parents
        while(true){
            if (parent->get_name() == "Object")
                break;
            parent = program->class_map[parent->get_parent()];
            parents.insert(parents.begin(), parent);
        }
        for (auto &parent : parents){ // Iterate over the class and its parents starting from the root class 'Object' to the current class

            AST::List<AST::Method> *parent_methods = parent->get_method_list();
            for(size_t y = 0; y < parent_methods->get_size(); ++y){
                AST::Method *method = parent_methods->get_element(y); 

                if(!override_another_method[method->get_name()]){
                    override_another_method[method->get_name()] = true;

                    Function *function = module->getFunction(parent->get_name() + "__" + method->get_name());
                    Type *function_type = function->getFunctionType();
                    
                    methods.push_back(function);
                    methods_types.push_back(PointerType::get(function_type, 0));

                    current_class->method_signatures[method->get_name()] = (FunctionType *) function_type;
                    current_class->method_indexes[method->get_name()] = method_index;
                    method_index++;
                } else {
                    int index = current_class->method_indexes[method->get_name()];

                    Function *function = module->getFunction(parent->get_name() + "__" + method->get_name());
                    methods[index] = function;

                    Type *function_type = function->getFunctionType();
                    methods_types[index] = PointerType::get(function_type, 0);
                    current_class->method_signatures[method->get_name()] = (FunctionType *) function_type;
                }
            }
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
            current_class->get_name() + "___new",     // The name
            module);                                // The LLVM module

        // Declare function 'init': initialize an object.
        method_type = FunctionType::get(
            class_type->getPointerTo(),   // The return type
            {class_type->getPointerTo()}, // The arguments
            false);                       // No variable number of arguments

        Function *init_function = Function::Create(
            method_type,                            // The signature
            GlobalValue::ExternalLinkage,           // The linkage
            current_class->get_name() + "___init" ,    // The name
            module);                                // The LLVM module

        init_function->arg_begin()->setName("self");

        /******** Implement the 'new' function ********/
        
        if(current_class->get_name() == "Object"){
            // The 'new' function of the Object class are already implemented in the runtime
            continue;
        }

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
            builder->getInt32(1),                                   // The index of the element we want
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

        // The new object needs to be initialized, thus we call the 'init' function
        Value *initialized_struct = CallInst::Create(
            init_function,     // Name of the function to call
            {cast_struct_ptr}, // Arguments
            "",                // Name of the LLVM variable (not fixed here)
            new_entry);        // The block in which the instruction will be inserted

        // The last thing to do is to return the initialized object
        builder->CreateRet(initialized_struct);

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

    /************** Define the main function **************/
    
    FunctionType *method_type = FunctionType::get(
        get_type("int32"),  // The return type
        {},                 // The arguments (no self for new)
        false);             // No variable number of arguments

    Function *main_function = Function::Create(
        method_type,                    // The signature
        GlobalValue::ExternalLinkage,   // The linkage
        "main",                         // The name
        module);                        // The LLVM module

    /************** Implement the 'main' function **************/

    BasicBlock *main_entry = BasicBlock::Create(
        *context,       // The LLVM context
        "entry",        // The label of the block
        main_function); // The function in which should be inserted the block

    builder->SetInsertPoint(main_entry);

    Function *Main_new_function = module->getFunction("Main___new");
    Value *main_value = builder->CreateCall(Main_new_function);

    Function *Main_main_function = module->getFunction("Main__main");
    Value *main_return = builder->CreateCall(Main_main_function, {main_value});
    
    builder->CreateRet(main_return);
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
    string output;
    raw_string_ostream os(output);
    module->print(os, nullptr);

    string exe_filename = fileName.substr(0, fileName.find_last_of("."));
    ofstream exe(exe_filename + ".ll");
	exe << output;
	exe.close();

    string cmd = "clang -Wno-override-module " + exe_filename + ".ll" + " /usr/local/lib/vsop/*.ll" + " -o " + exe_filename;
    char char_cmd[cmd.size() + 1];
    strcpy(char_cmd, cmd.c_str());

    system(char_cmd);
}