#include "ast.hpp"
#include "symbol.hpp"
#include "error.hpp"

SymbolTable st;

void Program::add_lib_func_semantic(std::string name, Stype resultType, std::list<ParameterGroup*> parameters={})
{
    FunctionEntry* f = new FunctionEntry(name);
    f->pardef = PARDEF_COMPLETE;
    f->arguments = parameters;
    f->resultType = resultType;
    st.addEntry(f);
}

inline std::list<ParameterGroup*> Program::make_single_parameter(Stype type, PassMode pm)
{
    return {
        new ParameterGroup {
            { "dummy" },
            type,
            pm
        }
    };
}

void Program::semantic_initialize()
{
    if (print_ast) {
        std::cout << "------------------------AST------------------------\n";
        rootNode->printOn(std::cout);
        std::cout << "---------------------------------------------------\n";
    }
    
    st = SymbolTable();

    /* 
        Initial Scope:
            Add predefined functions
    */
    st.openScope();
   
    add_lib_func_semantic("writeInteger", typeVoid, make_single_parameter(typeInteger, PASS_BY_VALUE));
    add_lib_func_semantic("writeChar", typeVoid, make_single_parameter(typeChar, PASS_BY_VALUE));
    add_lib_func_semantic("writeReal", typeVoid, make_single_parameter(typeReal, PASS_BY_VALUE));
    add_lib_func_semantic("writeBoolean", typeVoid, make_single_parameter(typeBoolean, PASS_BY_VALUE));
    add_lib_func_semantic("writeString", typeVoid, make_single_parameter(typeIArray(typeChar), PASS_BY_REFERENCE));

    add_lib_func_semantic("readInteger", typeInteger);
    add_lib_func_semantic("readChar", typeChar);
    add_lib_func_semantic("readReal", typeReal);
    add_lib_func_semantic("readBoolean", typeBoolean);
    add_lib_func_semantic("readString", typePointer(typeIArray(typeChar)));

    add_lib_func_semantic("abs", typeInteger, make_single_parameter(typeInteger, PASS_BY_VALUE));
    add_lib_func_semantic("fabs", typeReal, make_single_parameter(typeReal, PASS_BY_VALUE));
    add_lib_func_semantic("sqrt", typeReal, make_single_parameter(typeReal, PASS_BY_VALUE));
    add_lib_func_semantic("sin", typeReal, make_single_parameter(typeReal, PASS_BY_VALUE));
    add_lib_func_semantic("cos", typeReal, make_single_parameter(typeReal, PASS_BY_VALUE));
    add_lib_func_semantic("tan", typeReal, make_single_parameter(typeReal, PASS_BY_VALUE));
    add_lib_func_semantic("arctan", typeReal, make_single_parameter(typeReal, PASS_BY_VALUE));
    add_lib_func_semantic("exp", typeReal, make_single_parameter(typeReal, PASS_BY_VALUE));
    add_lib_func_semantic("ln", typeReal, make_single_parameter(typeReal, PASS_BY_VALUE));
    add_lib_func_semantic("pi", typeReal);

    add_lib_func_semantic("ord", typeInteger, make_single_parameter(typeChar, PASS_BY_VALUE));
    add_lib_func_semantic("chr", typeChar, make_single_parameter(typeInteger, PASS_BY_VALUE));

    add_lib_func_semantic("trunc", typeReal, make_single_parameter(typeInteger, PASS_BY_VALUE));
    add_lib_func_semantic("round", typeReal, make_single_parameter(typeInteger, PASS_BY_VALUE));

    /* Prepare global program scope */
    st.openScope();
}

void Program::semantic_run()
{
    rootNode->semantic();
}

void Program::semantic_finalize()
{
    /* If everything is ok then we will be left with the global and the initial scope which will be cleaned */
    st.closeScope();
    st.closeScope();

    if (st.currentScope != nullptr) {
        error("Semantic analysis does bad scope management!");
    }
}

void EmptyStmt::semantic()
{
    return;
}

void ASTnodeCollection :: semantic(){
    for (AST* n : nodes)
    n->semantic();
}

void BinOp::semantic() /* override */
{
    // calclulate types of operants and check for validity 
    left->semantic();
    right->semantic();
    bool numeric = left->is_arithmetic() && right->is_arithmetic();

    switch( hashf(op.c_str()) )
    {
        case "+"_ : case "-"_ : case "*"_ : 
            if (! numeric)
                error("%s%s%s %s %s %s", "Operator '", op, "' requires numeric operands but was given", left->type, "and", right->type);
            if (left->type_verify(typeReal) || right->type_verify(typeReal))
                this->type = typeReal;
            else 
                this->type = typeInteger;       
            break; 

        case "/"_ : 
            if (! numeric)
                error("%s%s%s %s %s %s", "Operator '", op, "' requires numeric operands but was given", left->type, "and", right->type);
            this->type = typeReal;
            break; 

        case "div"_ : case "mod"_ : 
            if (left->type_verify(typeInteger) && right->type_verify(typeInteger))
                this->type = typeInteger;
            else
                error("%s%s%s %s %s %s", "Operator '", op, "' requires integer operands but was given", left->type, "and", right->type);
            break; 

        case "and"_  : case "or"_ : 
            if (! (left->type_verify(typeBoolean) && right->type_verify(typeBoolean)) )
                error("%s%s%s %s %s %s", "Operator '", op, "' requires boolean operands but was given", left->type, "and", right->type);
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
                error("%s%s%s %s %s %s", "Operator '", op, "' requires same non array operands but was given", left->type, "and", right->type);
            break; 
        }
        case ">"_ :                     
        case "<"_ :
        case "<="_ : 
        case ">="_ : 
            if (! numeric)
                error("%s%s%s %s %s %s", "Operator '", op, "' requires numeric operands but was given", left->type, "and", right->type);
            this->type = typeBoolean;
            break; 
        default : 
            error("%s", "should not be reached");
    }
}

void UnOp::semantic() /* override */
{
    e->semantic();
    switch( hashf(op.c_str()) ){
        case "+"_ : case "-"_ :
            if (! e->is_arithmetic()) error("%s%s%s %s", "Cannot apply ' ", op, "' operator to expression of type", e->type);
            this->type = e->type;  
            break;
        case "not"_ : 
            if (!e->type_verify(typeBoolean)) error("%s %s", "Cannot apply 'not' operator to expression of type", e->type);
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
    if (e == nullptr) error("%s %s %s", "Id", name, "not found");
    this->type = e->type; 
}

void Result::semantic()
{
    VariableEntry* e = st.lookupVariable("result", LOOKUP_CURRENT_SCOPE);
    if (e == nullptr)
        error("%s", "Use of \"result\" is only allowed in functions");
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
        error("number of arguments mistmatch");

    // for (int i = 0; i < psize; i++) {
    //     Expr* e = parameters->nodes[i];
    //     ParameterGroup* p = f->arguments[i];
    std::list<AST*>::iterator ie;
    std::list<ParameterGroup*>::iterator ip;

    for (ie = parameters->nodes.begin(), ip = f->arguments.begin(); ie != parameters->nodes.end() && ip != f->arguments.end(); ++ie, ++ip) {
        Expr* e = (Expr*) *ie;
        ParameterGroup* p = *ip;

        e->semantic();
        if ( !e->type->is_compatible_with(p->type) )
            error("%s %s %s %s %s %s", "Parameters", p->names, "have type", p->type, "which is incompatible with", e->type);
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
        error("number of arguments mistmatch");

    // for (int i = 0; i < psize; i++) {
    //     Expr* e = parameters->nodes[i];
    //     ParameterGroup* p = f->arguments[i];
    std::list<AST*>::iterator ie;
    std::list<ParameterGroup*>::iterator ip;

    for (ie = parameters->nodes.begin(), ip = f->arguments.begin(); ie != parameters->nodes.end() && ip != f->arguments.end(); ++ie, ++ip) {
        Expr* e = (Expr*) *ie;
        ParameterGroup* p = *ip;

        e->semantic();
        if ( !p->type->is_compatible_with(e->type) )
            error("%s %s %s %s %s %s", "Parameters", p->names, "have type", p->type, "which is incompatible with", e->type);
    }
}

void StringValue::semantic() /* override */
{

}

void ArrayAccess::semantic() /* override */
{
    lval->semantic();
    pos->semantic();

    if (lval->type->kind != TYPE_ARRAY && lval->type->kind != TYPE_IARRAY)
        error("%s", "Accesing non array");
    if (!pos->type_verify(typeInteger))
        error("%s", "Non integer access constant");
    
    this->type = lval->type->refType;
}

void Dereference::semantic() /* override */
{
    e->semantic();
    if (e->type->kind != TYPE_POINTER)
        error("%s", "Dereferencing non pointer");
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
                    error("%s %s %s", "Formal parameters for function", name, "do not match thos on forward declaration");
            }

            if (!f->resultType->equals(type))
                error("%s %s %s %s %s %s %s", "Function", name, "was forward declared with type", f->resultType, "but now type", type, "was given");
            
            break;
        case PARDEF_COMPLETE:
            error("%s %s", "Cannot redefine function", name);
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

void Assignment::semantic() /* override */
{
    lval->semantic();
    rval->semantic();

    if ( !lval->type->is_compatible_with(rval->type) )
        error("%s %s %s %s", "Type", rval->type, "cannot be assigned to variable of type", lval->type);
}

void IfThenElse::semantic() /* override */
{
    cond->semantic();
    if (!cond->type_verify(typeBoolean))
        error("%s %s %s", "Invalid type", cond->type, "for if condition");

    st_then->semantic();
    if (st_else != nullptr) st_else->semantic();
}

void While::semantic() /* override */
{
    cond->semantic();
    if (!cond->type_verify(typeBoolean))
        error("%s %s %s", "Invalid type", cond->type, "for while condition");
    body->semantic();
}

void Label::semantic() /* override */
{
    //check if label exists and set it in bind state
    LabelEntry *l = st.lookupLabel(label);
    if (l == nullptr)
        error("%s %s %s", "Label", label, "not declared");
    if (l->isBound)
        error("%s %s %s", "Label", label, "already assigned");
    l->isBound = true;
}

void GoTo::semantic() /* override */
{
    //check if label exists (and is bound ???) IN CURRENT SCOPE ONLYYYYY
    LabelEntry *l = st.lookupLabel(label);
    if (l == nullptr)
     error("%s %s %s", "Label", label, "not declared");
    if (!l->isBound)
     error("%s %s %s", "Label", label, "not bound to a target");
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
        error("%s %s", "Invalid initialization on type", lval->type);
}

void InitArray::semantic() /* override */
{
    lval->semantic();
    size->semantic();

    bool cc = size->type_verify(typeInteger) && ( lval->type->kind == TYPE_POINTER && (lval->type->refType->kind == TYPE_ARRAY || lval->type->refType->kind == TYPE_IARRAY) );

    if (!cc)
        error("%s %s", "Invalid initialization on type", lval->type);
}

void Dispose::semantic() /* override */
{
    lval->semantic();

    bool cc = lval->type->kind == TYPE_POINTER && lval->type->refType->is_concrete();

    if (!cc)
        error("%s %s", "Invalid disposal of type", lval->type);
}

void DisposeArray::semantic() /* override */
{
    lval->semantic();

    bool cc = lval->type->kind == TYPE_POINTER && (lval->type->refType->kind == TYPE_ARRAY || lval->type->refType->kind == TYPE_IARRAY);

    if (!cc)
        error("%s %s", "Invalid disposal of type", lval->type);
}