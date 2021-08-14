#include "ast.hpp"
#include "symbol.hpp"

SymbolTable st;

void Program::add_lib_func_semantic(std::string name, Stype resultType, std::list<ParameterGroup*>* parameters=nullptr)
{
    FunctionEntry* f = new FunctionEntry(name);
    f->pardef = PARDEF_COMPLETE;
    if (parameters != nullptr)
        f->arguments = *parameters;
    f->resultType = resultType;
    st.addEntry(f);
}

inline std::list<ParameterGroup*>* Program::make_single_parameter(Stype type, PassMode pm)
{
    return new std::list<ParameterGroup*> {
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
                error("Operator '", op, "' requires numeric operands but was given ", left->type, " and ", right->type);
            if (left->type_verify(typeReal) || right->type_verify(typeReal))
                this->type = typeReal;
            else 
                this->type = typeInteger;       
            break; 

        case "/"_ : 
            if (! numeric)
                error("Operator '", op, "' requires numeric operands but was given ", left->type, " and ", right->type);
            this->type = typeReal;
            break; 

        case "div"_ : case "mod"_ : 
            if (left->type_verify(typeInteger) && right->type_verify(typeInteger))
                this->type = typeInteger;
            else
                error("Operator '", op, "' requires integer operands but was given ", left->type, " and ", right->type);
            break; 

        case "and"_  : case "or"_ : 
            if (! (left->type_verify(typeBoolean) && right->type_verify(typeBoolean)) )
                error("Operator '", op, "' requires boolean operands but was given ", left->type, " and ", right->type);
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
                error("Operator '", op, "' requires same non array operands but was given ", left->type, " and ", right->type);
            break; 
        }
        case ">"_ :                     
        case "<"_ :
        case "<="_ : 
        case ">="_ : 
            if (! numeric)
                error("Operator '", op, "' requires numeric operands but was given ", left->type, " and ", right->type);
            this->type = typeBoolean;
            break; 
        default : 
            error("BinOp: Should not be reached");
    }
}

void UnOp::semantic() /* override */
{
    e->semantic();
    switch( hashf(op.c_str()) ){
        case "+"_ : case "-"_ :
            if (! e->is_arithmetic()) error("Cannot apply ' ", op, "' operator to expression of type ", e->type);
            this->type = e->type;  
            break;
        case "not"_ : 
            if (!e->type_verify(typeBoolean)) error("Cannot apply 'not' operator to expression of type ", e->type);
            this->type = e->type; 
            break;
        case "@"_ : 
            if (!e->lvalue)  error("Dereferencing non l-value expression");
            this->type = typePointer(e->type);
            break;
    }
}

void Id::semantic() /* override */
{
    VariableEntry* e = st.lookupVariable(name);
    if (e == nullptr) error("Id ", name, " not found");
    this->type = e->type; 
}

void Result::semantic()
{
    VariableEntry* e = st.lookupVariable("result", LOOKUP_CURRENT_SCOPE);
    if (e == nullptr)
        error("Use of \"result\" is only allowed in functions");
    else
        this->type = e->type;
}

void Const::semantic() /* override */
{
    /* empty */
}


Stype create_call(std::string fname, ASTnodeCollection *parameters, int linecnt, std::string caller){

    FunctionEntry *f = st.lookupFunction(fname, LOOKUP_ALL_SCOPES);
    if (f == nullptr) error("Function ",fname, " does not exist in current scope.");
    std::list<std::tuple<AST *, PassMode>> params_flat;

    for (auto x : f->arguments){
        for (auto n : x->names){
            Id* id = new Id(n,linecnt);
            id->type = x->type;
            params_flat.push_back(
                std::make_tuple(id,x->pmode));

         }
    } 
    if (f == nullptr)
        error("Procedure", fname, " not found");
    
    if (parameters == nullptr) {
        if (0 != params_flat.size())
            error("Number of arguments mistmatch");
    }
    else {
        size_t psize = parameters->nodes.size();
        if (psize != params_flat.size())
            error("Number of arguments mistmatch");
        
        std::list<AST*>::iterator ie;
        std::list<std::tuple<AST*, PassMode>>::iterator ip;

        for (ie = parameters->nodes.begin(), ip = params_flat.begin(); 
            ie != parameters->nodes.end() && ip != params_flat.end(); ++ie, ++ip) {
            
            Expr* e = (Expr*) *ie;
            Id* p = (Id*)  std::get<0>(*ip);
            PassMode pm = std::get<1>(*ip);
            e->semantic();

            bool condition;

            if (pm == PASS_BY_VALUE) 
                condition = p->type->is_compatible_with(e->type);
            else 
                condition = typePointer(p->type)->is_compatible_with(typePointer(e->type)); 

            if ( !condition)
                error("Parameter ", p->name, " have type ", p->type, " which is incompatible with ", e->type);
        }
    }
    return f->resultType;
}

void CallProc::semantic() /* override */
{
    create_call(fname, parameters, linecnt , "Procedure");    
}

void CallFunc::semantic() /* override */
{
    this->type = create_call(fname, parameters, linecnt, "Function");    
}


void StringValue::semantic() /* override */
{

}

void ArrayAccess::semantic() /* override */
{
    lval->semantic();
    pos->semantic();

    if (lval->type->kind != TYPE_ARRAY && lval->type->kind != TYPE_IARRAY)
        error("Accesing non array");
    if (!pos->type_verify(typeInteger))
        error("Non integer access constant");
    
    this->type = lval->type->refType;
}

void Dereference::semantic() /* override */
{
    e->semantic();
    if (e->type->kind != TYPE_POINTER)
        error("Dereferencing non pointer");
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
                    error("Formal parameters for function ", name, " do not match those on forward declaration");
            }

            if (!f->resultType->equals(type))
                error("Function ", name, " was forward declared with type ", f->resultType, " but now type ", type, " was given");
            
            break;
        case PARDEF_COMPLETE:
            error("Cannot redefine function ", name);
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
        error("Type ", rval->type, " cannot be assigned to variable of type ", lval->type);
}

void IfThenElse::semantic() /* override */
{
    cond->semantic();
    if (!cond->type_verify(typeBoolean))
        error("Invalid type ", cond->type, " for if condition");

    st_then->semantic();
    if (st_else != nullptr) st_else->semantic();
}

void While::semantic() /* override */
{
    cond->semantic();
    if (!cond->type_verify(typeBoolean))
        error("Invalid type ", cond->type, " for while condition");
    body->semantic();
}

void Label::semantic() /* override */
{
    //check if label exists and set it in bind state
    LabelEntry *l = st.lookupLabel(label);
    if (l == nullptr)
        error("Label ", label, " not declared");
    if (l->isBound)
        error("Label ", label, " already assigned");
    l->isBound = true;
}

void GoTo::semantic() /* override */
{
    //check if label exists (and is bound ???) IN CURRENT SCOPE ONLYYYYY
    LabelEntry *l = st.lookupLabel(label);
    if (l == nullptr)
     error("Label ", label, " not declared");
    if (!l->isBound)
     error("Label ", label, " not bound to a target");
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
        error("Invalid initialization on type ", lval->type);
}

void InitArray::semantic() /* override */
{
    lval->semantic();
    size->semantic();

    bool cc = size->type_verify(typeInteger) && ( lval->type->kind == TYPE_POINTER && (lval->type->refType->kind == TYPE_ARRAY || lval->type->refType->kind == TYPE_IARRAY) );

    if (!cc)
        error("Invalid initialization on type ", lval->type);
}

void Dispose::semantic() /* override */
{
    lval->semantic();

    bool cc = lval->type->kind == TYPE_POINTER && lval->type->refType->is_concrete();

    if (!cc)
        error("Invalid disposal of type ", lval->type);
}

void DisposeArray::semantic() /* override */
{
    lval->semantic();

    bool cc = lval->type->kind == TYPE_POINTER && (lval->type->refType->kind == TYPE_ARRAY || lval->type->refType->kind == TYPE_IARRAY);

    if (!cc)
        error("Invalid disposal of type ", lval->type);
}