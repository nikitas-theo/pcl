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
    bool boolean = left->type_verify(typeBoolean) && right->type_verify(typeBoolean);

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
            if (left->type_verify(typeInteger) && right->type_veify(typeInteger))
                this->type = typeInteger;
            else   { error("Not integer");}
            break; 


        case "and"_  : case "or"_ : 
            if (! boolean) {error("Not boolean");}
            this->type = typeBoolean ;  

        case "<>"_ : case "="_ : 
        {
            bool same_type = left->type_verify(right->type);
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
            if (!e->type_verify(typeBoolean)) error("not bool");
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
    if (e == nullptr) error("Id " + name + " not found");
    this->type = e->type; 
}

void Const::semantic() /* override */ {/* empty */}

void CallFunc::semantic() /* override */
{
    SymbolEntry * e = st.lookup(fname);   

    // find corresponding function definition in symbol table 
    FunctionDef f = e->function;
    if (FunctionDef == nullptr) error("Function" + fname + " not defined");
    std::vector<Stype> defined_types = f->param_types;

    // check if parameters are correct
    if (defined_types->length() != parameters->length() )  
        error("different type of parameters");

    for(int i ; i < vector.length() ; i++){
        Expr* ex = parameters[i] 
        Stype t = defined_types[i]; 
        ex->semantic();
        if (ex->type != t) error("function call parameters do not match")
    }            
    // return type 
    this->type = e->type;
    
}

void CallProc::semantic() /* override */
{
}

void String::semantic() /* override */ {/* empty */}

void ArrayAccess::semantic() /* override */
{
    if (lval->type->kind != TYPE_ARRAY) error("accesing non array");
    if (!pos->type_verify(typeInteger)) error("non integer access constant");
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
    if (type->kind == TYPE_POINTER && pass_by == PASS_BY_VALUE)
        error ("variables that pass by value, can't have pointer type")
    
    for (auto p : formals){    
        st.insert(p,type);
    }
}

void FunctionDef::semantic() /* override */
{   
    // if its forward just insert function to scope;
    if (isForward) {
        SymbolEntry* e = st.insert(name,type);
        e->add_func(this);
    }
    else {
        SymbolEntry *e = st.lookup(name);  
        // this has been forward defined before
        if  (e != nullptr){
            FunctionDef* f = e->function;
            if (f == nullptr) 
                error("forward definition, but previous function doesn't exist");
            
            // check if parameters are correct
            if (defined_types->length() != parameters->length() )  
                error("different type of parameters");

        }   
        else {
            SymbolEntry* e = st.insert(name,type);
            e->add_func(this);
            
        }
    }    
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