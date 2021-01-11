#include "ast.hpp"

ConstantInt* AST::c_i32(int n)
{
    return ConstantInt::get(TheContext,APInt(32,n,true));
}

ConstantInt* AST::c_i8(char c)
{
    return ConstantInt::get(TheContext,APInt(8,c,false));
}

ConstantInt* AST::c_i1(int n)
{
    return ConstantInt::get(TheContext,APInt(1,n,false));
}

ConstantFP* AST::c_r64(double d)
{
    return ConstantFP::get(TheContext,APFloat(d));
}

void error(const char* str)
{ 
    std::cerr << str; 
}

// inline std::ostream& operator<<(std::ostream &out, const AST &t)
// {
//   t.printOn(out);
//   return out;
// }

// inline std::ostream& operator<<(std::ostream &out, const std::vector<std::string> &t)
// {
//         out << "[";
//         for (std::size_t i = 0; i < t.size(); i++) {
//             out << t[t.size() - 1 - i] << (i == t.size() - 1 ? "" : ",");
//         }
//         return out << "]" ;
// }

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

bool AST::check_type(Stype t1,Stype t2,bool check_size)
{ 
    if (t1->kind != t2->kind ) return false;
    if (t1->kind == TYPE_ARRAY && check_size && t1->size != t2->size) return false ;
    if (t1->kind == TYPE_POINTER || t1->kind == TYPE_IARRAY || t1->kind == TYPE_ARRAY) 
        return check_type(t1->refType, t2->refType);
    return true; 
}

// Type* Expr::ToCompilerType()
// {
//     switch(this->type->kind)
//     {
//         case TYPE_VOID    : return voidTy;
//         case TYPE_INTEGER : return i32;
//         case TYPE_BOOLEAN : return i1;
//         case TYPE_CHAR    : return i8;
//         case TYPE_REAL    : return i64;
//         case TYPE_ARRAY   : return ArrayType::get(???, this->type->size);
//         case TYPE_IARRAY  : return PointerType::get(???, 0);
//         case TYPE_POINTER : return PointerType::get(???, 0);
//         default           : return voidTy;
//     }
// }

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
