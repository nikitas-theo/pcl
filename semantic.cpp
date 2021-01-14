#include "ast.hpp"
#include "symbol.hpp"

SymbolTable st = SymbolTable();

void EmptyStmt::semantic() {return;}

void BinOp::semantic() /* override */
{
    // calclulate types of operants and check for validity 
    left->semantic();
    right->semantic();
    bool numeric = left->is_arithmetic() && right->is_arithmetic();
    bool boolean = check_type(left->type,typeBoolean) && check_type(right->type,typeBoolean);

    switch( hashf(op.c_str()) ){
        case "+"_ : case "-"_ : case "*"_ : 
            if (! numeric) {error("Not numeric");}
            if (left->type_verify(typeReal) || right->type_verify(typeReal))
                this->type = typeReal;
            else 
                this->type = typeInteger;       
            break; 

        case "/"_ : if (! numeric) {error("Not numeric");}
            this->type = typeReal;
            break; 

        case "div"_ : case "mod"_ : 
            if (check_type(left->type,typeInteger) && check_type(right->type,typeInteger)) 
                this->type = typeInteger;
            else   { error("Not integer");}
            break; 


        case "and"_  : case "or"_ : 
            if (! boolean) {error("Not boolean");}
            this->type = typeBoolean ;  

        case "<>"_ : case "="_ : 
        {
            bool same_type = check_type(left->type,right->type);
            // type must be same but not of type array 
            same_type = same_type && (left->type->kind == TYPE_IARRAY || left->type->kind == TYPE_ARRAY);
            if (numeric || same_type) { this->type = typeBoolean;}
            else { error("Equality error");}
            break; 
        }
        case ">"_ :                     
        case "<"_ :
        case "<="_ : 
        case ">="_ : 
            if (! numeric) { error("Not numeric");}
            this->type = typeBoolean;
            break; 
        default : 
            error("should not be reached");
    }
}

void UnOp::semantic() /* override */
{
    e->semantic();
    switch( hashf(op.c_str()) ){
        case "+"_ : case "-"_ :
            if (! e->is_arithmetic()) error("not arithmetic");
            this->type = e->type;  
            break;
        case "not"_ : 
            if (!check_type(e->type,typeBoolean)) error("not bool");
            this->type = e->type; 
            break;
        case "@"_ : 
            if (!e->lvalue)  error("not l-value");
            this->type = typePointer(e->type);
            break;
    }
}

void Id::semantic() /* override */
{
    SymbolEntry* e =  st.lookup(name);
    this->type = e->type; 
    int depth = e->depth; 
    while(depth --);
    //FunctionDef* f = functions.back();
    //f->add_parameter(name,e->depth);
}

void Const::semantic() /* override */
{

}

void CallFunc::semantic() /* override */
{
    //SymbolEntry * e = st.lookup(fname);  
    //std::vector<Stype> defined_types = e->param_types;
    //if (defined_types->length() != parameters->length() )  
    //    error("different type of parameters");
    /*for(int i ; i < vector.length() ; i++){
        Expr* ex = parameters[i] 
        Stype t = efined_types[i]; 
        ex->semantic();
        if (ex->type != t) error("function call parameters do not match")
    }            
    this->type = e->type;
    */
}

void CallProc::semantic() /* override */
{
    
    /*SymbolEntry * e = st.lookup(fname);  
    vector<Stype> defined_types = e->param_types;
    if (defined_types->length() != parameters->length() )  error("different type of parameters")
    for(int i ; i < vector.length() ; i++){
        Expr* ex = parameters[i] 
        Stype t = efined_types[i]; 
        ex->semantic();
        if (ex->type != t) error("function call parameters do not match")
    }            
    */
}

void String::semantic() /* override */
{

}

void ArrayAccess::semantic() /* override */
{
    if (lval->type->kind != TYPE_ARRAY) error("accesing non array");
    if (!check_type(pos->type,typeInteger)) error("non integer access constant");
    this->type = lval->type->refType;
}

void Dereference::semantic() /* override */
{
    if (e->type->kind != TYPE_POINTER) error("dereferencing non pointer");
    this->type = e->type->refType;
}

void Block::semantic() /* override */
{   
    for (auto x : locals.list) x->semantic();
    for (auto x : body.list) x->semantic();
}

void Variable::semantic() /* override */
{
    // just insert into symbol table
    st.insert(name,type);
}

void VarDef::semantic() /* override */
{
    for (auto var : vars.list){ 
        var->semantic();
    }  
}

void LabelDef::semantic() /* override */
{
    // we assume type void for labels 
    for(auto l : labels)
        st.insert(l,typeVoid);
}

void FormalsGroup::semantic() /* override */
{
    // variables that pass by value, can't have pointer type 
}

void FunctionDef::semantic() /* override */
{

}

void Declaration::semantic() /* override */
{
    lval->semantic();
    rval->semantic();
    bool cond = lval->type_verify(rval->type) && lval->is_concrete();
    cond = cond || (lval->type_verify(typeReal) && rval->type_verify(typeInteger));
    // find out how to check for : ^array of t := ^array [n] of t
    }

void IfThenElse::semantic() /* override */
{
    cond->semantic();
    cond->type_verify(typeBoolean);

    st_then->semantic();
    if (st_else != nullptr) st_else->semantic();
}

void While::semantic() /* override */
{
    cond->semantic();
    cond->type_verify(typeBoolean);
    body->semantic();
}

void Label::semantic() /* override */
{
    
}

void GoTo::semantic() /* override */
{
    
}

void ReturnStmt::semantic() /* override */
{
    
}

void Init::semantic() /* override */
{
    
}

void InitArray::semantic() /* override */
{
    
}

void Dispose::semantic() /* override */
{
    
}