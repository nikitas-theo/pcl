#include "ast.hpp"

// Initialzie some  variables
LLVMContext AST::TheContext;
IRBuilder<> AST::Builder(TheContext);
std::unique_ptr<Module> AST::TheModule;

Type *AST::i8;
Type *AST::i32;
Type *AST::i64;
Type *AST::r64;
Type *AST::voidTy; 
Function *AST::WriteInteger;
Function *AST::WriteBoolean;
Function *AST::WriteChar;
Function *AST::WriteReal;
Function *AST::WriteString;

Function *AST::ReadInteger;
Function *AST::ReadBoolean;
Function *AST::ReadChar;
Function *AST::ReadReal;
Function *AST::ReadString;

Function *AST::Abs;
Function *AST::FAbs;
Function *AST::Sqrt;
Function *AST::Sin;
Function *AST::Cos;
Function *AST::Tan;
Function *AST::Arctan;
Function *AST::Exp;
Function *AST::Ln;
Function *AST::Pi;

Function *AST::TheMalloc;
Function *AST::TheInit;
