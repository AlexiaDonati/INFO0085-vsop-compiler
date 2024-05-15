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
    Function *malloc_function = Function::Create(
        malloc_sign,                  // The signature
        GlobalValue::ExternalLinkage, // The linkage: external, malloc is implemented elsewhere
        "malloc",                     // The name
        module);                      // The LLVM module
    
    AST::List<AST::Class> *classes = program->get_class_list();
    for (size_t i = 0; i < classes->get_size(); ++i){
        AST::Class *current_class = classes->get_element(i);

        /****************** Define the Class structure ******************/
        // Declare structure
        StructType *class_type = StructType::create(*context, current_class->get_name()); // Declare structure for class
        
        // Declare vtable
        StructType *vtable_type = StructType::create(*context, "struct." + current_class->get_name() + "Vtable"); // Declare VTable structure for class

        /************** Fields **************/
        std::vector<Type *> class_fields = std::vector<Type *>();
        
        // First field is the pointer towards the vtable
        class_fields.push_back(vtable_type->getPointerTo());

        AST::List<AST::Field> *field_list = current_class->get_field_list();
        for (size_t j = 0; j < field_list->get_size(); ++j){
            AST::Field *field = field_list->get_element(j);

            // Insert the other fields
            class_fields.push_back(get_type(field->get_type()));
        }
        class_type->setBody(class_fields);

        /************** Methods **************/
        std::vector<Type *> methods_types;
        std::vector<Constant *> methods;
        std::map<string, FunctionType *> methods_signatures;
        std::map<string, Function *> methods_functions;

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

            methods_types.push_back(method_type->getPointerTo());
            methods_signatures[method->get_name()] = method_type;

            // Create the actual function that will implement the method
            Function *method_function = Function::Create(
                method_type,                  // The signature
                GlobalValue::ExternalLinkage, // The linkage (not important here)
                method->get_name(),           // The name
                module);                      // The LLVM module
            
            // Setting the name of the arguments
            auto arg_it = method_function->arg_begin();
            arg_it->setName("self");
            ++arg_it;
            for (size_t k = 0; k < formal_list->get_size(); ++k){
                arg_it->setName(formal_list->get_element(k)->get_name());
                ++arg_it;
            }

            methods_functions[method->get_name()] = method_function;
            methods.push_back(method_function);
        }
        vtable_type->setBody(methods_types);

        /************** Defining the vtable **************/
        // Create a constant
        Constant *vtable_const = ConstantStruct::get(
            vtable_type, // Type of the constant structure
            methods);    // Values to give to the different fields

        // Assign the constant to a global variable
        GlobalVariable *vtable = new GlobalVariable(
            *module,                      // The LLVM module
            vtable_type,                  // The type of the constant
            true,                         // It is constant
            GlobalValue::InternalLinkage, // The linkage
            vtable_const,                 // The constant value
            current_class->get_name() + "_vtable");      // The name of the variable
            
        /************** New and Init functions **************/
        // Declare function 'new': allocate memory for a new object and return it.
        FunctionType *method_type = FunctionType::get(
            class_type->getPointerTo(), // The return type
            {},                         // The arguments
            false);                     // No variable number of arguments

        Function *new_function = Function::Create(
            method_type,                  // The signature
            GlobalValue::ExternalLinkage, // The linkage
            "new_" + current_class->get_name(),          // The name
            module);                      // The LLVM module

        // Declare function 'init': initialize an object.
        method_type = FunctionType::get(
            class_type->getPointerTo(),   // The return type
            {class_type->getPointerTo()}, // The arguments
            false);                       // No variable number of arguments

        Function *init_function = Function::Create(
            method_type,                  // The signature
            GlobalValue::ExternalLinkage, // The linkage
            "init_" + current_class->get_name(),         // The name
            module);                      // The LLVM module

        init_function->arg_begin()->setName("self");
        
        /* Inheritance -----------------

        AST::Class *parent = current_class;

        // inherited methods
        std::map<std::string, bool> defined;
        std::map<std::string, int> method_indexes;

        int current_index = 0;
        
        do{
            if (parent->get_parent() == "Object"){
                break;
            }
            for(size_t c = 0; c < classes->get_size(); ++c){
                if(classes->get_element(c)->get_name() == parent->get_parent()){
                    parent = classes->get_element(c);
                    break;
                }
            }

            // inherited fields 
            AST::List<AST::Field> *parent_fields = parent->get_field_list();
            for(size_t y = 0; y < parent_fields->get_size(); ++y){
                AST::Field *field = parent_fields->get_element(y);
                
                class_fields.push_back(get_type(field->get_type()));
            }

            // inherited methods
            AST::List<AST::Method> *parent_methods = parent->get_method_list();
            for(size_t y = 0; y < parent_methods->get_size(); ++y){

                AST::Method *method = parent_methods->get_element(y); 

                if(methods_signatures.find(method->get_name()) == methods_signatures.end()){ // if it is not overriding
                    methods.push_back(module->getFunction(parent->get_name() + "__" + method->get_name()));

                    auto type = ((Function*) methods[i])->getFunctionType();
                    methods_types.push_back(PointerType::get(type, 0)); // add the type
                }
                else{ // if it is overriding
                    int index = method_indexes[method->get_name()]; // get the index
                    methods[index] = module->getFunction(parent->get_name() + "__" + method->get_name()); // update the method

                    auto type = ((Function*) methods[index])->getFunctionType(); 
                    methods_types[index] = PointerType::get(type, 0); // update the type
                }
            }
        } while(true);*/
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