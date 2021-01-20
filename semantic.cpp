#include "ast.hpp"
#include "symbol.hpp"

SymbolTable st = SymbolTable();

void EmptyStmt::semantic()
{
    return;
}

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
    return;
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

void StringLiteral::semantic() /* override */
{

}

void ArrayAccess::semantic() /* override */
{
    lval->semantic();
    pos->semantic();

    if (lval->type->kind != TYPE_ARRAY || lval->type->kind != TYPE_IARRAY)
        error("accesing non array");
    if (pos->type_verify(typeInteger))
        error("non integer access constant");
    
    this->type = lval->type->refType;
}

void Dereference::semantic() /* override */
{
    e->semantic();
    if (e->type->kind != TYPE_POINTER)
        error("dereferencing non pointer");
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
    //st.newVariable(name, type);
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
        //st.newVariable(l, typeVoid);
}

void FormalsGroup::semantic() /* override */
{
    // collection of variable names with specific type that represent the formal parameters of a function
    // each of the variables needs to be added to the current function scope
    // variables that pass by value, can't have pointer type

    for (std::string f : formals)
        //newParameter(????)
}

void FunctionDef::semantic() /* override */
{
    //need symbol table
}

void Declaration::semantic() /* override */
{
    lval->semantic();
    rval->semantic();

    if ( !lval->type->is_compatible_with(rval->type) )
        error("incompatible types");
}

void IfThenElse::semantic() /* override */
{
    cond->semantic();
    if (!cond->type_verify(typeBoolean))
        error("invalid type");

    st_then->semantic();
    if (st_else != nullptr) st_else->semantic();
}

void While::semantic() /* override */
{
    cond->semantic();
    if (!cond->type_verify(typeBoolean))
        error("invalid type");
    body->semantic();
}

void Label::semantic() /* override */
{
    //check if label exists and set it in bind state
}

void GoTo::semantic() /* override */
{
    //check if label exists (and is bound ???) IN CURRENT SCOPE ONLYYYYY
}

void ReturnStmt::semantic() /* override */
{
    return;
}

void Init::semantic() /* override */
{
    lval->semantic();

    bool cc = lval->type->kind == TYPE_POINTER && lval->type->refType->is_concrete();

    if (!cc)
        error("invalid type");
}

void InitArray::semantic() /* override */
{
    lval->semantic();
    size->semantic();

    bool cc = size->type_verify(typeInteger) && ( lval->type->kind == TYPE_POINTER && (lval->type->refType->kind == TYPE_ARRAY || lval->type->refType->kind == TYPE_IARRAY) );

    if (!cc)
        error("invalid type");
}

void Dispose::semantic() /* override */
{
    lval->semantic();

    bool cc = lval->type->kind == TYPE_POINTER && lval->type->refType->is_concrete();

    if (!cc)
        error("invalid type");
}

void DisposeArray::semantic() /* override */
{
    lval->semantic();

    bool cc = lval->type->kind == TYPE_POINTER && (lval->type->refType->kind == TYPE_ARRAY || lval->type->refType->kind == TYPE_IARRAY);

    if (!cc)
        error("Invalid type");
}