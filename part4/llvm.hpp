#ifndef LLVM_HH
#define LLVM_HH

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/IRBuilder.h"

#include "llvm/IR/LegacyPassManager.h"

#include "llvm/Support/raw_ostream.h"

#include <map>
#include <vector>

#include "ast.hpp"

using namespace std;
using namespace llvm;

class LLVM {
    private:
        static LLVM *instance; // Singleton instance
        std::string fileName;

    public:
        LLVMContext *context; // Used to generate the LLVM module and the types
        Module *module; // Module containing the LLVM IR (compilation unit)
        IRBuilder<> *builder; // Used to generate the LLVM IR

        LLVM(AST::Program *program, const string &fileName);

        static LLVM *get_instance(AST::Program *program, const string &fileName){
            if (instance == nullptr){
                instance = new LLVM(program, fileName);
            }
            return instance;
        }

        Type *get_type(string type){
            if (type == "int32")
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
                return PointerType::get(module->getTypeByName(type), 0);
        }

        void optimize();

        void print();

        void executable(const std::string &fileName);

        Function* make_method(    
            vector<string> args_name, 
            vector<string> args_type, 
            string return_type, 
            string class_name,
            StructType* class_type,
            string method_name);

        Function* make_method(    
            vector<string> args_name, 
            vector<string> args_type, 
            Type * return_type, 
            string class_name,
            StructType* class_type,
            string method_name);

        Function* make_new(string class_name, StructType* class_type);

        Value* implement_new(Function* method_function, StructType* class_type);

        Function* make_init(string class_name, StructType* class_type);

        void make_function_block(string name, Function *function);

        Value* load(Value* object, uint position);

        Value* get_pointer(Value* object, uint position);

        Value* call_method(
            Value* object, 
            uint position, 
            string return_type, 
            vector<Value *> args);

        void set_value(Value* object, Value* value);

        vector<Value *> get_function_args(Function *function);

        StructType * create_mtable(string class_name);

        GlobalVariable* save_m_table(StructType *mtable_type, string class_name, vector<Type *> methods_types, vector<Constant *> methods);

        void set_return_value(bool return_value);

        void set_return_value(int return_value);

        void set_return_value(Value *return_value);
};

#endif // LLVM_HH