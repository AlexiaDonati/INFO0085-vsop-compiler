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