#include "ast.hpp"


void error(const char* str)
{ 
    std::cerr << str; 
    std::exit(1);
}

Type* AST::TypeConvert(Stype t) 
{
    switch(t->kind) {
        case TYPE_VOID      : return voidTy;
        case TYPE_INTEGER   : return i32; 
        case TYPE_BOOLEAN   : return i1; 
        case TYPE_CHAR      : return i8; 
        case TYPE_REAL      : return r64;
        case TYPE_ARRAY : 
            return ArrayType::get(TypeConvert(t->refType), t->size);
        case TYPE_IARRAY :
            // seems like for llvm semantics this is the same 
            return PointerType::get(TypeConvert(t->refType),0);
        case TYPE_POINTER : 
            return PointerType::get(TypeConvert(t->refType),0);
    }
    return voidTy;
}

bool Expr::is_arithmetic() 
{ 
    return this->type->kind == TYPE_REAL || this->type->kind == TYPE_INTEGER;
}

bool Expr::is_concrete() 
{
    return ! (this->type->kind == TYPE_IARRAY);
}

bool Expr::type_verify(Stype t)
{
    return this->type->equals(t);
}

void String::ReplaceStringInPlace(std::string& subject, const std::string& search, const std::string& replace)
{
    size_t pos = 0;
    while((pos = subject.find(search, pos)) != std::string::npos) {
        subject.replace(pos, search.length(), replace);
        pos += replace.length();
    }
}

void Block::push_local(Stmt *l)
{
    locals.list.push_back(l);
}

void VarDef::push(std::vector<std::string>* var_ids, Stype t)
{
    for (std::string s : *var_ids ) {
        vars.list.push_back(new Variable(s,t));
    }
}

void FunctionDef::set_forward()
{
    this->isForward = true;
}

void FunctionDef::add_body(Block* theBody)
{
    this->body = theBody;
}
