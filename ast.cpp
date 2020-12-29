#include "ast.hpp"

// Initialzie some  variables
LLVMContext AST::TheContext;
IRBuilder<> AST::Builder(TheContext);
Module* AST::TheModule;

Type* AST::i1;
Type* AST::i8;
Type* AST::i32;
Type* AST::i64;
Type* AST::r64;
Type* AST::voidTy;
SymbolTable AST::st = SymbolTable();
void add_libs(AST* ast){
    Function *WriteInteger = Function::Create(
        FunctionType::get(ast->voidTy,{ast->i32},false), 
        Function::ExternalLinkage, "writeInteger", ast->TheModule
    );
    ast->st.insert("writeInteger",WriteInteger);
}

   // PREDEFINED LIBRARY FUNCTIONS 

        // WRITE UTILS 


        /*
        Function *WriteBoolean = Function::Create(
            FunctionType::get(voidTy,{i1},false), 
            Function::ExternalLinkage, "writeString", TheModule.get()
        );
        Function *WriteChar = Function::Create(
            FunctionType::get(voidTy,{i8},false), 
            Function::ExternalLinkage, "writeInteger", TheModule.get()
        );
        Function *WriteReal = Function::Create(
            FunctionType::get(voidTy,{r64},false), 
            Function::ExternalLinkage, "writeInteger", TheModule.get()
        );
        Function *WriteString = Function::Create(
            FunctionType::get(voidTy,{PointerType::get(i8, 0)},false), 
            Function::ExternalLinkage, "writeInteger", TheModule.get()
        );

        // READ UTILS

        Function *ReadInteger = Function::Create(
            FunctionType::get(r64,voidTy,false), 
            Function::ExternalLinkage, "writeInteger", TheModule.get()
        );
        Function *ReadBoolean = Function::Create(
            FunctionType::get(i1,voidTy,false), 
            Function::ExternalLinkage, "writeInteger", TheModule.get()
        );
        Function *ReadChar = Function::Create(
            FunctionType::get(i8,voidTy,false), 
            Function::ExternalLinkage, "writeInteger", TheModule.get()
        );
        Function *ReadReal = Function::Create(
            FunctionType::get(r64,voidTy,false), 
            Function::ExternalLinkage, "writeInteger", TheModule.get()
        );
        Function *ReadString = Function::Create(
            FunctionType::get(PointerType::get(i8, 0),voidTy,false), 
            Function::ExternalLinkage, "writeInteger", TheModule.get()
        );

        // MATH UTILS 

        Function *Abs = Function::Create(
            FunctionType::get(i32,{i32},false), 
            Function::ExternalLinkage, "Abs", TheModule.get()
        );
        Function *FAbs = Function::Create(
            FunctionType::get(r64,{r64},false), 
            Function::ExternalLinkage, "FAbs", TheModule.get()
        );
        Function *Sqrt = Function::Create(
            FunctionType::get(r64,{r64},false), 
            Function::ExternalLinkage, "Sqrt", TheModule.get()
        );
        Function *Sin = Function::Create(
            FunctionType::get(r64,{r64},false), 
            Function::ExternalLinkage, "Sin", TheModule.get()
        );
        Function *Cos = Function::Create(
            FunctionType::get(r64,{r64},false), 
            Function::ExternalLinkage, "Cos", TheModule.get()
        );
        Function *Tan = Function::Create(
            FunctionType::get(r64,{r64},false), 
            Function::ExternalLinkage, "Tan", TheModule.get()
        );
        Function *Arctan = Function::Create(
            FunctionType::get(r64,{r64},false), 
            Function::ExternalLinkage, "Arctan", TheModule.get()
        );
        Function *Exp = Function::Create(
            FunctionType::get(r64,{r64},false), 
            Function::ExternalLinkage, "Exp", TheModule.get()
        );
        Function *Ln = Function::Create(
            FunctionType::get(r64,{r64},false), 
            Function::ExternalLinkage, "Ln", TheModule.get()
        );
        Function *Pi = Function::Create(
            FunctionType::get(r64,voidTy,false), 
            Function::ExternalLinkage, "Pi", TheModule.get()
        );
        */

