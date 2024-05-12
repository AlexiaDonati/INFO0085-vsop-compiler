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
                return PointerType::get(module->getTypeByName(type), 0);
        }

        void optimize();

        void print();

        void executable(const std::string &fileName);
};

#endif // LLVM_HH