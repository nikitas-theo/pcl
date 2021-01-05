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
Function* AST::GC_Free; 
Function* AST::GC_Malloc;
Function* AST::GC_Init;
std::vector<AST*> AST::functions();
CodeGenTable AST::ct = CodeGenTable();
SymbolTable AST::st = SymbolTable();

