#include "ast.hpp"



Type* AST::TypeConvert(Stype t, bool is_var_def) 
{
    switch(t->kind) {
        case TYPE_VOID      : return voidTy;
        case TYPE_INTEGER   : return i32; 
        case TYPE_BOOLEAN   : return i1; 
        case TYPE_CHAR      : return i8; 
        case TYPE_REAL      : return r64;
        case TYPE_ARRAY : 
            return ArrayType::get(TypeConvert(t->refType, is_var_def), t->size);
        case TYPE_IARRAY :
            /* decided on IARRAY to have type refType* 
               where the * comes from alloca 
               the only way to have this is ^ array of integer 
               so it works because ^ array of integer --> int** with alloca
               and we remember to not extra load whenever we have IARRAY 
               e.g. not like int** where we need to load to int*

               this is all confusing but it is because array of Ty is always
               tied with pointer  
            */ 
            return TypeConvert(t->refType, is_var_def);
        case TYPE_POINTER : 
            return PointerType::get(TypeConvert(t->refType, is_var_def),0);
    }
    return voidTy;
}




bool AST::check_type(Stype t1,Stype t2,bool check_size)
{
    return t1->equals(t2);
}

void ASTnodeCollection::push(AST* node)
{
    nodes.push_front(node);
}

void Program::mark_optimizable(bool on)
{
    this->optimize = on;
}

void Program::mark_console_interactive(bool dump)
{
    this->imm_stdout = dump;
}

bool Program::is_console_interactive()
{
    return imm_stdout;
}

void Program::mark_printable(bool mark)
{
    this->print_ast = mark;
}

void Program::set_name_if_blank(std::string name)
{
    if (program_name.empty())
        program_name = name;
}

void Program::attach_AST(AST* root)
{
    rootNode = root;
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

void StringValue::ReplaceStringInPlace(std::string& subject, const std::string& search, const std::string& replace)
{
    size_t pos = 0;
    while((pos = subject.find(search, pos)) != std::string::npos) {
        subject.replace(pos, search.length(), replace);
        pos += replace.length();
    }
}

void Block::push_local(Stmt *l)
{
    locals->push(l);
}

void VarDef::push(std::list<std::string>* var_ids, Stype t, int cnt)
{
    for (std::string s : *var_ids ) {
        vars.push_back(new Variable(s, t, cnt));
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



void FunctionDef::add_request(std::string id, int nesting_diff, int struct_idx, Stype type){
    std::map<std::string, DepenVar*>::iterator it;
    it = requests.find(id);
    if (it == requests.end())
        requests.insert(std::make_pair(id, new DepenVar(id, nesting_diff, struct_idx, type)));    
}

int FunctionDef::add_provide(std::string id, Stype type){
    
    std::map<std::string, DepenVar*>::iterator it;
    it = provides.find(id);
    if (it != provides.end())
        return it->second->struct_idx;
    else { 
        int idx = provides.size() + 1;
        provides.insert(std::make_pair(id,new DepenVar(id, idx, type)));        
        return idx; 
    }

}
