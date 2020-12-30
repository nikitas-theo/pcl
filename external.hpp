#include <llvm/IR/Value.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include "symbol.hpp"
using namespace llvm;

inline void add_func(FunctionType *type, std::string name, SymbolTable &st, Module &TheModule){
    Function *func = Function::Create(type, 
            Function::ExternalLinkage, name, TheModule
        );
        st.insert(name,func);
    };

inline void add_libs(Module &TheModule, SymbolTable &st,LLVMContext &TheContext){
    Type* i1 = IntegerType::get(TheContext,8);
    Type* i8 = IntegerType::get(TheContext,8);
    Type* i32 = IntegerType::get(TheContext,32);
    Type* r64 =  Type::getDoubleTy(TheContext);
    Type* voidTy = Type::getVoidTy(TheContext);

    

   // PREDEFINED LIBRARY FUNCTIONS 

    // WRITE UTILS 

    add_func(FunctionType::get(voidTy,{i32},false),"writeInteger",st, TheModule);
    add_func(FunctionType::get(voidTy,{i1},false), "writeBoolean", st, TheModule);
    add_func(FunctionType::get(voidTy,{i8},false), "writeChar", st, TheModule);
    add_func(FunctionType::get(voidTy,{r64},false), "writeReal", st, TheModule);
    add_func(FunctionType::get(voidTy,{PointerType::get(i8, 0)}),"writeString",st, TheModule);

    // READ UTILS

    add_func(FunctionType::get(r64,{},false),"readInteger",st, TheModule);
    add_func(FunctionType::get(i1,{},false), "readBoolean", st, TheModule);
    add_func(FunctionType::get(i8,{},false), "readChar", st, TheModule);
    add_func(FunctionType::get(r64,{},false), "readReal", st, TheModule);
    add_func(FunctionType::get(PointerType::get(i8, 0),{},false),"readString",st, TheModule);


    // MATH UTILS 

    add_func(FunctionType::get(i32,{i32},false),"abs",st, TheModule);
    add_func(FunctionType::get(r64,{r64},false),"fabs",st, TheModule);
    add_func(FunctionType::get(r64,{r64},false),"sqrt",st, TheModule);
    add_func(FunctionType::get(r64,{r64},false),"sin",st, TheModule);
    add_func(FunctionType::get(r64,{r64},false),"cos",st, TheModule);
    add_func(FunctionType::get(r64,{r64},false),"tan",st, TheModule);
    add_func(FunctionType::get(r64,{r64},false),"arctan",st, TheModule);
    add_func(FunctionType::get(r64,{r64},false),"exp",st, TheModule);
    add_func(FunctionType::get(r64,{r64},false),"ln",st, TheModule);
    add_func(FunctionType::get(r64,{},false),"pi",st, TheModule);


};
