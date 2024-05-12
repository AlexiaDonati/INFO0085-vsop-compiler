#include "llvm.hpp"

LLVM::LLVM(Program* program, const std::string &fileName): fileName(fileName){
    context = new LLVMContext(); // Used to generate the LLVM module and the types
    module = new Module(fileName, *context); // Module containing the LLVM IR (compilation unit)
    builder = new IRBuilder<>(*context); // Used to generate the LLVM IR

    // Create the types for the classes
    for (auto &classDef : program->classDefs){
        vector<Type *> types;
        for (auto &field : classDef->fields){
            types.push_back(get_type(field->type));
        }
        StructType::create(*context, types, classDef->name);
    }

    // Create the types for the methods
    for (auto &classDef : program->classDefs){
        for (auto &method : classDef->methods){
            vector<Type *> types;
            for (auto &arg : method->args){
                types.push_back(get_type(arg->type));
            }
            FunctionType *functionType = FunctionType::get(get_type(method->returnType), types, false);
            Function::Create(functionType, Function::ExternalLinkage, method->name, module);
        }
    }

}

LLVM *instance = nullptr; // Singleton

LLVM *get_instance(Program* program, const std::string &fileName){
    if (instance == nullptr)
        instance = new LLVM(program, fileName);
    return instance;
}

static Type *get_type(string type){
    if (type == "int")
        return Type::getInt32Ty(*context);
    else if (type == "bool")
        return Type::getInt1Ty(*context);
    else if (type == "void")
        return Type::getVoidTy(*context);
    else if (type == "string")
        return PointerType::getInt8PtrTy(*context);
    else if (type == "unit")
        return PointerType::getInt8PtrTy(*context);
    else
        return PointerType::get(mdl->getTypeByName(type), 0);
}

void optimize(){
    legacy::FunctionPassManager fpm(module);
    legacy::PassManager mpm;

    fpm.add(createPromoteMemoryToRegisterPass());
    fpm.add(createInstructionCombiningPass());
    fpm.add(createReassociatePass());
    fpm.add(createGVNPass());
    fpm.add(createCFGSimplificationPass());
    fpm.doInitialization();

    for (auto &f : *module){
        fpm.run(f);
    }

    mpm.add(createPromoteMemoryToRegisterPass());
    mpm.add(createInstructionCombiningPass());
    mpm.add(createReassociatePass());
    mpm.add(createGVNPass());
    mpm.add(createCFGSimplificationPass());
    mpm.doInitialization();

    mpm.run(*module);
}

void print(){
    string output;

    raw_string_ostream os(output);
    module->print(os, nullptr);

    std::cout << output << std::endl;
}

void executable(const string &fileName){
    string executable_fileName = fileName.substr(0, fileName.size() - 5);
    string llvm_fileName = executable_fileName + ".ll";

    std::error_code EC;
    raw_fd_ostream dest(llvm_fileName, EC, sys::fs::F_None);
    if (EC) {
        errs() << "Could not open file: " << EC.message() << endl;
        return;
    }

    module->print(dest, nullptr);
    dest.flush();

    string command = "clang -o " + executable_fileName + " " + llvm_fileName + " /usr/local/lib/vsop/*.c";
    system(command.c_str());
}