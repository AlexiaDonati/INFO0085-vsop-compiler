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

/******* Malloc Function *******/
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

/**************************/

/******* Class List *******/

// Structure--------------------------
    string class_name = "List";
    StructType* class_type = StructType::create(*context, class_name);
    StructType *m_table_type = create_mtable(class_name);

// Fields--------------------------
// ==== Define the list of fields
    vector<Type *> fields_types;

// ==== Define fields
    // First field is the pointer towards the mtable
    fields_types.push_back(m_table_type->getPointerTo());

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

    make_function_block("entry", method_function);

    vector<Value *> arguments_values = get_function_args(method_function);

    set_return_value(true);

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

    make_function_block("entry", method_function);

    arguments_values = get_function_args(method_function);

    set_return_value(0);

// M table save---------------------------
    save_m_table(m_table_type, class_name, methods_types, methods);

// Mandatory "method"

// ==== .init()
    make_init(class_name, class_type);

// ==== .new()
    method_function = make_new(class_name, class_type);
    
/**************************/
}
{
/******* Class Nil extends List *******/

// Structure--------------------------
    string class_name = "Nil";
    string parent_class_name = "List";
    StructType* class_type = StructType::create(*context, class_name);
    StructType *m_table_type = create_mtable(class_name);

// Fields--------------------------
// ==== Define the list of fields
    vector<Type *> fields_types;

// ==== Define fields
    // First field is the pointer towards the mtable
    fields_types.push_back(m_table_type->getPointerTo());

// ==== Push the field in the class structure
    class_type->setBody(fields_types);

// Methods--------------------------
    vector<Type *> methods_types;
    vector<Constant *> methods;
// ==== Inherits parents methods
/* Try to get the m table of the parrent, but seg fault
    GlobalVariable* parent_m_table = module->getNamedGlobal(parent_class_name + "_Mtable");
    Constant* value = parent_m_table->getInitializer();
*/
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

    // put in m_table
    methods_types.push_back(method_function->getFunctionType()->getPointerTo());
    methods.push_back(method_function);

    make_function_block("entry", method_function);

    vector<Value *> arguments_values = get_function_args(method_function);

    Value *result = builder->CreateCall(parent_function, {arguments_values[0]});

    set_return_value(result);

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
    
    // put in m_table
    methods_types.push_back(method_function->getFunctionType()->getPointerTo());
    methods.push_back(method_function);

    make_function_block("entry", method_function);

    arguments_values = get_function_args(method_function);

    result = builder->CreateCall(parent_function, {arguments_values[0]});

    set_return_value(result);

// M table save---------------------------
    save_m_table(m_table_type, class_name, methods_types, methods);

// Mandatory "method"

// ==== .init()
    make_init(class_name, class_type);
// ==== .new()
    method_function = make_new(class_name, class_type);
    
/**************************/
}

{
/******* Class Cons extends List *******/

// Structure--------------------------
    string class_name = "Cons";
    string parent_class_name = "List";
    StructType* class_type = StructType::create(*context, class_name);
    StructType *m_table_type = create_mtable(class_name);

// Fields--------------------------
// ==== Define the list of fields
    vector<Type *> fields_types;

// ==== Define fields
    // First field is the pointer towards the mtable
    fields_types.push_back(m_table_type->getPointerTo());
    fields_types.push_back(get_type("int32")); // head
    fields_types.push_back(get_type("List")); // tail

// ==== Push the field in the class structure
    class_type->setBody(fields_types);

// Methods--------------------------
    vector<Type *> methods_types;
    vector<Constant *> methods;
// ==== head()
    string method_name = "head";

    vector<string> args_name;
    vector<string> args_type;
    Function* method_function = make_method(    
        args_name, 
        args_type, 
        "int32", 
        class_name, 
        class_type,
        method_name);

    // put in m_table
    methods_types.push_back(method_function->getFunctionType()->getPointerTo());
    methods.push_back(method_function);

    make_function_block("entry", method_function);

    vector<Value *> arguments_values = get_function_args(method_function);

    // Load the value of head
    Value* head_value = load(arguments_values[0], 1);

    set_return_value(head_value);

// ==== init()
    method_name = "init";
    
    method_function = make_method(    
        {"hd", "tl"}, 
        {"int32", "List"}, 
        class_name, 
        class_name, 
        class_type,
        method_name);

    // put in m_table
    methods_types.push_back(method_function->getFunctionType()->getPointerTo());
    methods.push_back(method_function);

    make_function_block("entry", method_function);

    arguments_values = get_function_args(method_function);

    //head <- hd;
    set_value(get_pointer(arguments_values[0], 1), arguments_values[1]);

    //tail <- tl;
    set_value(get_pointer(arguments_values[0], 2), arguments_values[2]);

    set_return_value(arguments_values[0]);

// ==== Inherits parents methods
/* Try to get the m table of the parrent, but seg fault
    GlobalVariable* parent_m_table = module->getNamedGlobal(parent_class_name + "_Mtable");
    Constant* value = parent_m_table->getInitializer();
*/
// ==== ==== isNil()
    method_name = "isNil";
    string true_method_name = parent_class_name + "." + method_name;
    Function* parent_function = module->getFunction(true_method_name);

    method_function = make_method(    
        args_name, 
        args_type, 
        "bool", 
        class_name, 
        class_type,
        method_name);

    // put in m_table
    methods_types.push_back(method_function->getFunctionType()->getPointerTo());
    methods.push_back(method_function);

    make_function_block("entry", method_function);

    arguments_values = get_function_args(method_function);

    // We don"t call the parent method because this is an override
    Value *result; //= builder->CreateCall(parent_function);

    set_return_value(false);

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
    
    // put in m_table
    methods_types.push_back(method_function->getFunctionType()->getPointerTo());
    methods.push_back(method_function);

    make_function_block("entry", method_function);

    arguments_values = get_function_args(method_function);

    // We don"t call the parent method because this is an override
    // result = builder->CreateCall(parent_function);

    // Load the value of tail
    Value* tail_value = load(arguments_values[0], 2);

    result = call_method(tail_value, 1, "int32", {});

    // 1 + tail.length()
    result = builder->CreateAdd(builder->getInt32(1), result, "");

    set_return_value(result);

// M table save---------------------------
    save_m_table(m_table_type, class_name, methods_types, methods);

// Mandatory "method"

// ==== .init()
    make_init(class_name, class_type);

// ==== .new()
    method_function = make_new(class_name, class_type);
    
/**************************/
}
{
/******* Class main *******/

// Structure--------------------------
    string class_name = "main";
    StructType* class_type = StructType::create(*context, class_name);
    StructType *m_table_type = create_mtable(class_name);

// Fields--------------------------
// ==== Define the list of fields
    vector<Type *> fields_types;

// ==== Define fields
    // First field is the pointer towards the mtable
    fields_types.push_back(m_table_type->getPointerTo());

// ==== Push the field in the class structure
    class_type->setBody(fields_types);

// Methods--------------------------
    vector<Type *> methods_types;
    vector<Constant *> methods;

// ==== main()

    string method_name = "main";

    vector<string> args_name;
    vector<string> args_type;
    Function* method_function = make_method(    
        args_name, 
        args_type, 
        "int32", 
        class_name, 
        class_type,
        method_name);

    // put in m_table
    methods_types.push_back(method_function->getFunctionType()->getPointerTo());
    methods.push_back(method_function);

    make_function_block("entry", method_function);

    vector<Value *> arguments_values = get_function_args(method_function);

    // (new Cons).init(0, (new Cons).init(1, (new Cons).init(2, new Nil)))
    // ==== (new Cons)
    Value *first_new_object = call_new_then_init("Cons");

    // ==== .init(0, (new Cons).init(1, (new Cons).init(2, new Nil)))
    // ==== ==== (new Cons).init(1, (new Cons).init(2, new Nil))
    // ==== ==== ==== (new Cons)
    Value *second_new_object = call_new_then_init("Cons");

    // ==== ==== ==== .init(1, (new Cons).init(2, new Nil))
    // ==== ==== ==== ==== (new Cons).init(2, new Nil)
    // ==== ==== ==== ==== ==== (new Cons)
    Value *third_new_object = call_new_then_init("Cons");
    // ==== ==== ==== ==== ==== .init(2, new Nil)
    // ==== ==== ==== ==== ==== ==== new Nil
    Value *fourth_new_object = call_new_then_init("Nil");
    // ==== ==== ==== ==== ==== init()
    Value* result = call_method(
        third_new_object, 
        2, 
        "Cons", 
        {builder->getInt32(2), fourth_new_object});
    // ==== ==== ==== init()
    result = call_method(
        second_new_object, 
        1, 
        "Cons", 
        {builder->getInt32(2), result});
    // ==== init()
    result = call_method(
        first_new_object, 
        0, 
        "Cons", 
        {builder->getInt32(2), result});
    
    Value* xs = result;
    /*
        to to this -> must link object
        {
            print("List has length ");
            printInt32(xs.length());
            print("\n");
            0
        }
    */
    
    set_return_value(0);

// M table save---------------------------
    save_m_table(m_table_type, class_name, methods_types, methods);

// Mandatory "method"

// ==== .init()
    make_init(class_name, class_type);

// ==== .new()
    make_new(class_name, class_type);
    
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
    string method_name,
    bool put_self){
        return make_method(    
            args_name, 
            args_type, 
            get_type(return_type), 
            class_name, 
            class_type,
            method_name,
            put_self);
    }

Function* LLVM::make_method(    
    vector<string> args_name, 
    vector<string> args_type, 
    Type * return_type, 
    string class_name,
    StructType* class_type,
    string method_name,
    bool put_self){

    if(args_name.size() != args_type.size()){
        fprintf(stderr, "Error: make_method -> args_name and args_type must be the same size\n");
        return NULL;
    }

// ==== ==== Define args
    vector<Type *> method_arguments;

    // First arg is always Self
    if(put_self){
        Type *self = class_type->getPointerTo();
        method_arguments.push_back(self);
    }

    for (auto type : args_type)
        method_arguments.push_back(get_type(type));

// ==== ==== Create signature
    string true_method_name = class_name + "." + method_name;

    FunctionType *method_signature = FunctionType::get(
        return_type,        // Return type
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

Function* LLVM::make_new(string class_name, StructType* class_type){
    Function* new_function = make_method(    
        {}, 
        {}, 
        class_name, 
        class_name, 
        class_type,
        ".new", // there is a point to avoid collision with user methods
        false); 

    Value* new_object = implement_new(new_function, class_type);

    set_return_value(new_object);

    return new_function;
}

// Return the new object value
Value* LLVM::implement_new(Function* method_function, StructType* class_type){
    make_function_block("entry", method_function);

    // Get the size of the object
    Value* ptr_size = builder->CreateGEP(
        class_type, 
        ConstantPointerNull::get(class_type->getPointerTo()), 
        {builder->getInt32(1)}, 
        "");

    Value* bytes_size = builder->CreatePointerCast(
        ptr_size, 
        Type::getInt64Ty(*context), 
        "");

    // Now that we have the size, we can call malloc:
    Value *struct_ptr = builder->CreateCall(
        module->getFunction("malloc"), 
        {bytes_size});

    // Malloc returns a i8 pointer, we have to cast it as a pointer towards our structure
    return builder->CreatePointerCast(
        struct_ptr, 
        class_type->getPointerTo(), 
        "");
}

Function* LLVM::make_init(string class_name, StructType* class_type){
    Function* init_function = make_method(    
        {}, 
        {}, 
        class_name, 
        class_name, 
        class_type,
        ".init"); // there is a point to avoid collision with user methods

    make_function_block("entry", init_function);

    vector<Value *> arguments_values = get_function_args(init_function);

    // init m table
    Value* m_table_ptr = builder->CreateGEP(
        class_type, 
        arguments_values[0],  // self
        {builder->getInt32(0),
        builder->getInt32(0)}, // m table is at position 0
        "");

    GlobalVariable* m_table = module->getNamedGlobal(class_name + "_mtable");

    builder->CreateStore(m_table, m_table_ptr);

    set_return_value(0);

    return init_function;
}

void LLVM::make_function_block(string name, Function *function){
    // ==== ==== ==== Define block
    BasicBlock *function_block = BasicBlock::Create(
        *context,         // The LLVM context
        name,             // The label of the block
        function);        // The function in which should be inserted the block
    
    // ==== ==== ==== Define Builder
    builder->SetInsertPoint(function_block);
}

Value* LLVM::load(Value* object, uint position){
    Value* ptr = get_pointer(object, position);
    return builder->CreateLoad(ptr, "");
}

Value* LLVM::get_pointer(Value* object, uint position){
    return builder->CreateGEP(
            object, 
            {ConstantInt::get(Type::getInt32Ty(*context), 0),
                ConstantInt::get(Type::getInt32Ty(*context), position)},
            "");
}

Value* LLVM::call_method(
    Value* object, 
    uint position, 
    string return_type, 
    vector<Value *> args){
    // Load the value of mtable of object
    Value* m_table_value = load(object, 0);

    // Load object.method() in object m_tables
    Value* length_method_value = load(m_table_value, position);

    FunctionType *false_signature = FunctionType::get(
        get_type(return_type), // Return type
        {}, // do not need to specify because will be con$mpleted automaticaly by call (to be verified)
        false);           // No variable number of arguments

    args.insert(args.begin(), object);

    // call 
    return builder->CreateCall(false_signature, length_method_value, args, "");
}

Value* LLVM::call_new_then_init(string type){
    Function *new_function = module->getFunction(type + "..new");

    Value *new_object = builder->CreateCall(new_function);

    Function *init_function = module->getFunction(type + "..init");

    return builder->CreateCall(init_function, {new_object});
}

void LLVM::set_value(Value* object, Value* value){
    builder->CreateStore(value, object);
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

GlobalVariable* LLVM::save_m_table(StructType *mtable_type, string class_name, vector<Type *> methods_types, vector<Constant *> methods){
    mtable_type->setBody(methods_types);

    // Create a constant
    Constant *mtable_const = ConstantStruct::get(
        mtable_type, // Type of the constant structure
        methods);    // Values to give to the different fields

    // Assign the constant to a global variable
    GlobalVariable *mtable = new GlobalVariable(
        *module,                      // The LLVM module
        mtable_type,                  // The type of the constant
        true,                        // It is constant
        GlobalValue::InternalLinkage, // The linkage
        mtable_const,                 // The constant value
        class_name + "_mtable");      // The name of the variable

    return mtable;
}

void LLVM::set_return_value(bool return_value){
    builder->CreateRet(
        ConstantInt::get(
            Type::getInt1Ty(*context), 
            (return_value) ? 1 : 0
        )
    );
}

void LLVM::set_return_value(int return_value){
    builder->CreateRet(
        ConstantInt::get(
            Type::getInt32Ty(*context), 
            return_value
        )
    );
}

void LLVM::set_return_value(Value *return_value){
    builder->CreateRet(
        return_value
    );
}