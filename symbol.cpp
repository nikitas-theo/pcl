#include "symbol.hpp"

Stype typeInteger = new SemanticType(TYPE_INTEGER,nullptr,0);
Stype typeReal = new SemanticType(TYPE_REAL,nullptr,0);
Stype typeBoolean = new SemanticType(TYPE_BOOLEAN,nullptr,0);
Stype typeChar = new SemanticType(TYPE_CHAR,nullptr,0);
Stype typeVoid = new SemanticType(TYPE_VOID,nullptr,0);


Stype typeIArray(Stype refType) { return new SemanticType(TYPE_IARRAY,refType,0);}
Stype typeArray(int size,Stype refType) { return new SemanticType(TYPE_ARRAY,refType,size);}
Stype typePointer(Stype refType) { return new SemanticType(TYPE_POINTER,refType,0);}


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

SymbolTable::SymbolTable()
{
    // Scope *s = new Scope();
    // s->nestingLevel = 0;
    // scopeStack.push_back(s);
    // currentScope = s;
}

// void SymbolTable::Initialize()
// {
//     Scope *s = new Scope();
//     s->nestingLevel = 0;
//     scopeStack.push_back(s);
//     currentScope = s;
// }

SymbolTable::~SymbolTable()
{
    while (!scopeStack.empty())
    {
        delete(scopeStack.back());
        scopeStack.pop_back();
    }
}

void SymbolTable::openScope()
{
    Scope *s = new Scope();
    s->nestingLevel = scopeStack.empty() ? 0 : currentScope->nestingLevel + 1;
    scopeStack.push_back(s);
    currentScope = s;
}

void SymbolTable::closeScope()
{
    delete(currentScope);
    scopeStack.pop_back();
    currentScope = scopeStack.empty() ? nullptr : scopeStack.back();
}

void SymbolTable::addEntry(SymbolEntry* e)
{
    currentScope->addEntry(e);
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
    SymbolEntry *e = lookupEntry(id, lookup);

    if (e != nullptr && e->entryType == ENTRY_VARIABLE) {
        return (VariableEntry *)e;
    }
    else {
        return nullptr;
    }
    
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