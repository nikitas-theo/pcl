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

    switch( hashf(op.c_str()) ){
        case "+"_ : case "-"_ : case "*"_ : 
            if (! numeric)
                error("Not numeric");
            if (left->type_verify(typeReal) || right->type_verify(typeReal))
                this->type = typeReal;
            else 
                this->type = typeInteger;       
            break; 

        case "/"_ : 
            if (! numeric)
                error("Not numeric");
            this->type = typeReal;
            break; 

        case "div"_ : case "mod"_ : 
            if (left->type_verify(typeInteger) && right->type_verify(typeInteger))
                this->type = typeInteger;
            else
                error("Not integer");
            break; 

        case "and"_  : case "or"_ : 
            if (! (left->type_verify(typeBoolean) && right->type_verify(typeBoolean)) )
                error("Not boolean");
            this->type = typeBoolean ;  
            break;

        case "<>"_ : case "="_ : 
        {
            bool same_type = left->type_verify(right->type);
            // type must be same but not of type array 
            same_type = same_type && !(left->type->kind == TYPE_IARRAY || left->type->kind == TYPE_ARRAY);
            if (numeric || same_type)
                this->type = typeBoolean;
            else
                error("Equality error");
            break; 
        }
        case ">"_ :                     
        case "<"_ :
        case "<="_ : 
        case ">="_ : 
            if (! numeric)
                error("Not numeric");
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
    VariableEntry* e = st.lookupVariable(name);
    if (e == nullptr) error("Id not found");
    this->type = e->type; 
}

void Result::semantic()
{
    VariableEntry* e = st.lookupVariable("result", LOOKUP_CURRENT_SCOPE);
    if (e == nullptr)
        error("use of \"result\" is only allowed in functions");
    else
        this->type = e->type;
}

void Const::semantic() /* override */
{
    /* empty */
}

void CallFunc::semantic() /* override */
{
    // SymbolEntry * e = st.lookup(fname);   

    // // find corresponding function definition in symbol table 
    // FunctionDef f = e->function;
    // if (FunctionDef == nullptr) error("Function" + fname + " not defined");
    // std::vector<Stype> defined_types = f->param_types;

    // // check if parameters are correct
    // if (defined_types->length() != parameters->length() )  
    //     error("different type of parameters");

    // for(int i ; i < vector.length() ; i++){
    //     Expr* ex = parameters[i] 
    //     Stype t = defined_types[i]; 
    //     ex->semantic();
    //     if (ex->type != t) error("function call parameters do not match")
    // }            
    // // return type 
    // this->type = e->type;

    FunctionEntry *f = st.lookupFunction(fname, LOOKUP_ALL_SCOPES);

    if (f == nullptr)
        error("procedure not found");
    
    size_t psize = parameters->nodes.size();
    if (psize != f->arguments.size())
        error("not enough parameters");

    // for (int i = 0; i < psize; i++) {
    //     Expr* e = parameters->nodes[i];
    //     ParameterGroup* p = f->arguments[i];
    std::list<AST*>::iterator ie;
    std::list<ParameterGroup*>::iterator ip;

    for (ie = parameters->nodes.begin(), ip = f->arguments.begin(); ie != parameters->nodes.end() && ip != f->arguments.end(); ++ie, ++ip) {
        Expr* e = (Expr*) *ie;
        ParameterGroup* p = *ip;

        e->semantic();
        if (!e->type->equals(p->type))
            error("unmatching types");
    }

    this->type = f->resultType;
    
}

void CallProc::semantic() /* override */
{
    FunctionEntry *f = st.lookupFunction(fname, LOOKUP_ALL_SCOPES);

    if (f == nullptr)
        error("procedure not found");
    
    size_t psize = parameters->nodes.size();
    if (psize != f->arguments.size())
        error("not enough parameters");

    // for (int i = 0; i < psize; i++) {
    //     Expr* e = parameters->nodes[i];
    //     ParameterGroup* p = f->arguments[i];
    std::list<AST*>::iterator ie;
    std::list<ParameterGroup*>::iterator ip;

    for (ie = parameters->nodes.begin(), ip = f->arguments.begin(); ie != parameters->nodes.end() && ip != f->arguments.end(); ++ie, ++ip) {
        Expr* e = (Expr*) *ie;
        ParameterGroup* p = *ip;

        e->semantic();
        if (!e->type->equals(p->type))
            error("unmatching types");
    }
}

void StringValue::semantic() /* override */
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
    locals->semantic();
    body->semantic();
}

void Variable::semantic() /* override */
{
    // just insert into symbol table
    st.addEntry(new VariableEntry(name, type));
}

void VarDef::semantic() /* override */
{
    for (Variable* var : vars) { 
        var->semantic();
    }  
}

void LabelDef::semantic() /* override */
{
    for(std::string l : labels)
        st.addEntry(new LabelEntry(l));
}

void FormalsGroup::semantic() /* override */
{
    // collection of variable names with specific type that represent the formal parameters of a function
    // each of the variables needs to be added to the current function scope
    // variables that pass by value, can't have pointer type

    return;
}

void FunctionDef::semantic() /* override */
{
    FunctionEntry* f = st.lookupFunction(name, LOOKUP_CURRENT_SCOPE);

    if (f == nullptr) {
        f = new FunctionEntry(name);
        f->pardef = PARDEF_DEFINE;
        st.addEntry(f);
    }

    switch (f->pardef) {
        case PARDEF_DEFINE:
            //clone parameters to f->arguments
            f->arguments = parameters;

            //set result type
            f->resultType = type;
            
            break;
        case PARDEF_PENDING_CHECK:
            for (std::list<ParameterGroup*>::iterator defit = parameters.begin(), symbit = f->arguments.begin(); defit != parameters.end() && symbit != f->arguments.end(); ++defit, ++symbit) {
                if (*defit != *symbit)
                    error("non matching parameters");
            }

            if (!f->resultType->equals(type))
                error("non matching function type");
            
            break;
        case PARDEF_COMPLETE:
            error("cannot redefine function");
            break;
    }

    if (isForward) {
        f->pardef = PARDEF_PENDING_CHECK;
    }
    else {
        st.openScope();

        if (!type->equals(typeVoid))
            st.addEntry(new VariableEntry("result", type));

        for (ParameterGroup* p : parameters) {
            for (std::string v : p->names) {
                st.addEntry(new VariableEntry(v, p->type));
            }
        }

        f->pardef = PARDEF_COMPLETE;
        body->semantic();

        st.closeScope();
    }

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
    LabelEntry *l = st.lookupLabel(label);
    if (l == nullptr)
        error("label not declared");
    if (l->isBound)
        error("label already assigned");
    l->isBound = true;
}

void GoTo::semantic() /* override */
{
    //check if label exists (and is bound ???) IN CURRENT SCOPE ONLYYYYY
    LabelEntry *l = st.lookupLabel(label);
    if (l == nullptr)
     error("label not declared");
    if (!l->isBound)
     error("label not bound to a target");
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