#include <llvm/IR/Value.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include "symbol.hpp"
using namespace llvm;

inline void add_func(FunctionType *type, std::string name, CodeGenTable &ct, Module &TheModule){
    Function *func = Function::Create(type, 
            Function::ExternalLinkage, name, TheModule
        );
        ct.insert(name,func);
    };

inline void add_libs(Module &TheModule, CodeGenTable &ct,LLVMContext &TheContext){
    Type* i1 = IntegerType::get(TheContext,8);
    Type* i8 = IntegerType::get(TheContext,8);
    Type* i32 = IntegerType::get(TheContext,32);
    Type* r64 =  Type::getDoubleTy(TheContext);
    Type* voidTy = Type::getVoidTy(TheContext);

    

   // PREDEFINED LIBRARY FUNCTIONS 

    // WRITE UTILS 

    add_func(FunctionType::get(voidTy,{i32},false),"writeInteger",ct, TheModule);
    add_func(FunctionType::get(voidTy,{i1},false), "writeBoolean", ct, TheModule);
    add_func(FunctionType::get(voidTy,{i8},false), "writeChar", ct, TheModule);
    add_func(FunctionType::get(voidTy,{r64},false), "writeReal", ct, TheModule);
    add_func(FunctionType::get(voidTy,{PointerType::get(i8, 0)}),"writeString",ct, TheModule);

    // READ UTILS

    add_func(FunctionType::get(r64,{},false),"readInteger",ct, TheModule);
    add_func(FunctionType::get(i1,{},false), "readBoolean", ct, TheModule);
    add_func(FunctionType::get(i8,{},false), "readChar", ct, TheModule);
    add_func(FunctionType::get(r64,{},false), "readReal", ct, TheModule);
    add_func(FunctionType::get(PointerType::get(i8, 0),{},false),"readString",ct, TheModule);


    // MATH UTILS 

    add_func(FunctionType::get(i32,{i32},false),"abs",ct, TheModule);
    add_func(FunctionType::get(r64,{r64},false),"fabs",ct, TheModule);
    add_func(FunctionType::get(r64,{r64},false),"sqrt",ct, TheModule);
    add_func(FunctionType::get(r64,{r64},false),"sin",ct, TheModule);
    add_func(FunctionType::get(r64,{r64},false),"cos",ct, TheModule);
    add_func(FunctionType::get(r64,{r64},false),"tan",ct, TheModule);
    add_func(FunctionType::get(r64,{r64},false),"arctan",ct, TheModule);
    add_func(FunctionType::get(r64,{r64},false),"exp",ct, TheModule);
    add_func(FunctionType::get(r64,{r64},false),"ln",ct, TheModule);
    add_func(FunctionType::get(r64,{},false),"pi",ct, TheModule);


};
