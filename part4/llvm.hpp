#ifndef LLVM_HH
#define LLVM_HH

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Type.h"

#include "llvm/IR/LegacyPassManager.h"

#include "llvm/Support/raw_ostream.h"


#include <map>
#include <vector>

using namespace std;
using namespace llvm;

class LLVM {
    private:
        static LLVM *instance; // Singleton instance.
        std::string fileName;

    public:
        LLVMContext *context; // Used to generate the LLVM module and the types
        Module *module; // Module containing the LLVM IR (compilation unit)
        IRBuilder<> *builder; // Used to generate the LLVM IR

        LLVM(Program *program, const string &fileName);

        static LLVM *get_instance(Program *program, const string &fileName);

        static Type *get_type(string type);

        void optimize();

        void print();

        void executable(const std::string &fileName);
};

#endif // LLVM_HH