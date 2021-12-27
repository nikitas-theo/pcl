#include "symbol.hpp"
#include "ast.hpp"

// why are types here? for some include reason
Stype typeInteger = new SemanticType(TYPE_INTEGER,nullptr,0);
Stype typeReal = new SemanticType(TYPE_REAL,nullptr,0);
Stype typeBoolean = new SemanticType(TYPE_BOOLEAN,nullptr,0);
Stype typeChar = new SemanticType(TYPE_CHAR,nullptr,0);
Stype typeVoid = new SemanticType(TYPE_VOID,nullptr,0);


Stype typeIArray(Stype refType) { 
    return new SemanticType(TYPE_IARRAY,refType,0);}
Stype typeArray(int size,Stype refType) { 
    return new SemanticType(TYPE_ARRAY,refType,size);}
Stype typePointer(Stype refType) { return new SemanticType(TYPE_POINTER,refType,0);}

/* hidden to external functions? */
FunctionEntry::~FunctionEntry()
{
    arguments.clear();
}

Scope::Scope()
{
    levelEntries = new HashMap();
}

Scope::~Scope()
{
    levelEntries->clear();
    delete(levelEntries);
}

bool Scope::addEntry(SymbolEntry* e)
{
    std::pair<HashMap::iterator, bool> mapInRes;

    mapInRes = levelEntries->insert(std::make_pair(e->id, e));

    return mapInRes.second;
}



SymbolEntry* Scope::lookupEntry(String id)
{
    HashMap::iterator it;

    it = levelEntries->find(id);

    if ( it == levelEntries->end() ) {
        return nullptr;
    }
    else {
        return it->second;
    }
    
}

/* symbol table */

SymbolTable::SymbolTable(){}

SymbolTable::~SymbolTable()
{
    while (!scopeStack.empty())
    {
        delete(scopeStack.back());
        scopeStack.pop_back();
    }
}

void SymbolTable::openScope(FunctionDef* function, FunctionDef* forward)
{
    Scope *s = new Scope();
    s->nestingLevel = scopeStack.empty() ? 0 : currentScope->nestingLevel + 1;
    scopeStack.push_back(s);
    s->function = function;  
    // if this isn't the Main scope
    if (currentScope != nullptr){
        s->function->static_parent = currentScope->function ;
        // need to do this fix, because in compile of forward def we need to have parent struct 
        if (forward != nullptr)
            forward->static_parent = currentScope->function ;
    }
    currentScope = s;
}

void SymbolTable::closeScope()
{
    for (std::string label_id : currentScope->labelNames){
        LabelEntry* l = lookupLabel(label_id);
        if (! l->isBound && l->isTargeted) 
            error("goto instruction to unbound label");        
    }
    delete(currentScope);
    scopeStack.pop_back();
    currentScope = scopeStack.empty() ? nullptr : scopeStack.back();
}


void SymbolTable::addEntry(SymbolEntry* e)
{
    currentScope->addEntry(e);
    if (e->entryType == ENTRY_LABEL) 
        currentScope->labelNames.push_back(e->id);
}

SymbolEntry* SymbolTable::lookupEntry(String id, LookupType lookup)
{
    SymbolEntry *e;

    switch (lookup)
    {
        case LOOKUP_CURRENT_SCOPE:
            if ( (e = currentScope->lookupEntry(id)) != nullptr )
                return e;
            break;
        
        case LOOKUP_ALL_SCOPES:
            for (auto it = scopeStack.rbegin(); it != scopeStack.rend(); ++it) {
                if ( (e = (*it)->lookupEntry(id)) != nullptr )
                    return e;
            }
            break;
    }

    return nullptr;
}

VariableEntry* SymbolTable::lookupVariable(String id, LookupType lookup)
{
    SymbolEntry *e;
    if ( (e = currentScope->lookupEntry(id)) != nullptr  && e->entryType == ENTRY_VARIABLE)
        return (VariableEntry*) e; 

    int nesting_diff; 
    int struct_idx; 

    if (lookup == LOOKUP_CURRENT_SCOPE) return nullptr;             
        for (auto it = scopeStack.rbegin(); it != scopeStack.rend(); ++it) {                
                if ( (e = (*it)->lookupEntry(id)) != nullptr && e->entryType == ENTRY_VARIABLE){
                    nesting_diff =  currentScope->nestingLevel - (*it)->nestingLevel;
                    VariableEntry* v = (VariableEntry*) e; 
                    struct_idx = (*it)->function->add_provide(id, v->type);
                    break; 
                    }
            }
    if (e == nullptr) return nullptr ;
    VariableEntry* v = (VariableEntry*) e; 
    currentScope->function->add_request(id, nesting_diff, struct_idx, v->type);    
    return v; 

}

LabelEntry* SymbolTable::lookupLabel(String id)
{
    SymbolEntry *e = lookupEntry(id, LOOKUP_CURRENT_SCOPE);

    if (e != nullptr && e->entryType == ENTRY_LABEL) {
        return (LabelEntry *)e;
    }
    else {
        return nullptr;
    }
    
}

FunctionEntry* SymbolTable::lookupFunction(String id, LookupType lookup)
{
    SymbolEntry *e = lookupEntry(id, lookup);

    if (e != nullptr && e->entryType == ENTRY_FUNCTION) {
        return (FunctionEntry *)e;
    }
    else {
        return nullptr;
    }
    
}





void CodeGenTable::openScope(FunctionDef *fun) {

    int ofs = scopes.empty() ? 0 : scopes.back().getOffset();

    scopes.push_back(CodeGenScope(ofs));
    scopes.back().function = fun;
}

void CodeGenTable::closeScope() { 
    for (std::string l : scopes.back().label_names){
        CodeGenEntry *e = lookup(l);
        for (GoTo * g : e->goto_nodes) g->compile_final(e->label);
    }
    scopes.pop_back(); 
    };

CodeGenEntry * CodeGenTable::lookup(std::string name, LookupType lookup) {

    CodeGenEntry *e;

    switch (lookup)
    {
        case LOOKUP_CURRENT_SCOPE:
        {
            CodeGenScope currentScope = scopes.back();
            if ( (e = currentScope.lookup(name)) != nullptr )
                return e;
            break;
        }        
        case LOOKUP_ALL_SCOPES:
        {
            for (auto i = scopes.rbegin(); i != scopes.rend(); ++i) {
                e = i->lookup(name);
                if (e != nullptr) return e;
            }    
            break;
        }
    }

    return nullptr;
}


void CodeGenTable::addToLabel(std::string label, GoTo* g){
    CodeGenEntry *l = lookup(label);
    l->goto_nodes.push_back(g);
}

void CodeGenTable::addLabel(std::string label){
    scopes.back().label_names.push_back(label);
}

FunctionDef * CodeGenTable::get_fun(){
    return  scopes.back().function;
}

int CodeGenTable::getSizeOfCurrentScope() { 
    return scopes.back().getSize(); 
}

int CodeGenTable::getNumScopes() { 
    return scopes.size();
}

int CodeGenTable::insert(std::string name, llvm::Value* v,PassMode pb) { 
    return scopes.back().insert(name, v, pb); 
}


CodeGenEntry * CodeGenScope::lookup(std::string name) {

    if (locals.find(name) == locals.end()) return nullptr;
    return &(locals[name]);
}
int CodeGenScope::insert(std::string name, llvm::Value* v, PassMode pb) {

    if (locals.find(name) != locals.end()) {
        return 1;
    }
    locals[name] = CodeGenEntry(v, offset++,pb);
    ++size;

    return 0; 
}
