#ifndef __SYMBOL_H__
#define __SYMBOL_H__

#include <map>
#include <string>
#include <deque>
#include <functional>
#include <utility>
#include <list>
#include <vector>
#include <iosfwd>

#include "types.hpp"
#include "ast.hpp"

/* ---------------------------------------------------------------------
   --------------- Ορισμός τύπων του πίνακα συμβόλων -------------------
   --------------------------------------------------------------------- */

/* Τύποι δεδομένων για την υλοποίηση των σταθερών */

typedef int           RepInteger;         /* Ακέραιες                  */
typedef unsigned char RepBoolean;         /* Λογικές τιμές             */
typedef char          RepChar;            /* Χαρακτήρες                */
typedef long double   RepReal;            /* Πραγματικές               */
typedef const char *  RepString;          /* Συμβολοσειρές             */

typedef union {                    /* Τιμή                  */
    RepInteger vInteger;              /*    ακέραια            */
    RepBoolean vBoolean;              /*    λογική             */
    RepChar    vChar;                 /*    χαρακτήρας         */
    RepReal    vReal;                 /*    πραγματική         */
    RepString  vString;               /*    συμβολοσειρά       */
} ConstValueType;

/* Τύποι εγγραφών του πίνακα συμβόλων */

typedef enum {            
   ENTRY_VARIABLE,                       /* Μεταβλητές                 */
   ENTRY_CONSTANT,                       /* Σταθερές                   */
   ENTRY_FUNCTION,                       /* Συναρτήσεις                */
   ENTRY_PARAMETER,                      /* Παράμετροι συναρτήσεων     */
   ENTRY_LABEL,                          /* Ετικέτες */
   ENTRY_TEMPORARY                       /* Προσωρινές μεταβλητές      */
} EntryType;


/* Τύποι περάσματος παραμετρων */

// typedef enum {            
//    PASS_BY_VALUE,                        /* Κατ' αξία                  */
//    PASS_BY_REFERENCE                     /* Κατ' αναφορά               */
// } PassMode;

/* Κατάσταση Ορισμού Συνάρτησης */

typedef enum {                        /* Κατάσταση παραμέτρων  */
    PARDEF_DEFINE,                       /* Εν μέσω ορισμού    */
    PARDEF_PENDING_CHECK,                /* Εν μέσω ελέγχου    */
    PARDEF_COMPLETE                      /* Πλήρης ορισμός     */
} PardefType;

/* Τύπος αναζήτησης στον πίνακα συμβόλων */

typedef enum {
    LOOKUP_CURRENT_SCOPE,
    LOOKUP_ALL_SCOPES
} LookupType;

/* Λοιποί ορισμοί τύπων */

typedef size_t HashType;
typedef std::string String;

template <class T>
using Container = std::list<T>;

/* Εγγραφές του πίνακα συμβόλων */

class SymbolEntry
{
    public:
        String          id;
        EntryType       entryType;
        HashType        hashValue;

        SymbolEntry(String name, EntryType et) : id(name), entryType(et) {} 
};

typedef std::map<String, SymbolEntry*> HashMap;

class VariableEntry : public SymbolEntry
{
    public:
        Stype type;

        VariableEntry(String name, Stype t) : SymbolEntry(name, ENTRY_VARIABLE), type(t) {}
};

class ConstantEntry : public SymbolEntry
{
    public:
        Stype type;
        ConstValueType value;
};

class LabelEntry : public SymbolEntry
{
    public:
        bool isBound;

        LabelEntry(String name) : SymbolEntry(name, ENTRY_LABEL), isBound(false) {}
};

class ParameterEntry : public VariableEntry
{
    public:
        PassMode mode;
        SymbolEntry *function;

        ParameterEntry(String name, Stype t, PassMode m) : VariableEntry(name, t), mode(m) {}

        bool equals(ParameterEntry* p)
        {
            return id == p->id && type->equals(p->type) && mode == p->mode;
        }
};

class FunctionEntry : public SymbolEntry
{
    public:
        PardefType pardef;
        Container<ParameterGroup *> arguments;
        Stype resultType;

        FunctionEntry(String name) : SymbolEntry(name, ENTRY_FUNCTION) {}

        ~FunctionEntry()
        {
            arguments.clear();
        }

        bool isDefinitionComplete()
        {
            return pardef == PARDEF_COMPLETE;
        }
};

class Scope
{
    private:
        HashMap levelEntries;

    public:
        int nestingLevel;
        AST* owingFunctionNode;

        ~Scope()
        {
            levelEntries.clear();
        }

        bool addEntry(SymbolEntry* e)
        {
            std::pair<HashMap::iterator, bool> mapInRes;

            mapInRes = levelEntries.insert(std::make_pair(e->id, e));

            return mapInRes.second;
        }

        SymbolEntry* lookupEntry(String id)
        {
            HashMap::iterator it;

            it = levelEntries.find(id);

            if ( it == levelEntries.end() ) {
                return nullptr;
            }
            else {
                return it->second;
            }
            
        }
};

class SymbolTable
{
    private:
        Container<Scope*> scopeStack;

    public:
        Scope* currentScope;
        
        SymbolTable()
        {
            Scope *s = new Scope();
            s->nestingLevel = 0;
            scopeStack.push_back(s);
            currentScope = s;
        }

        ~SymbolTable()
        {
            while (!scopeStack.empty())
            {
                delete(scopeStack.back());
                scopeStack.pop_back();
            }
        }

        void openScope()
        {
            Scope *s = new Scope();
            s->nestingLevel = currentScope->nestingLevel + 1;
            scopeStack.push_back(s);
            currentScope = s;
        }

        void closeScope()
        {
            delete(currentScope);
            scopeStack.pop_back();
            currentScope = scopeStack.back();
        }

        void addEntry(SymbolEntry* e)
        {
            currentScope->addEntry(e);
        }

        SymbolEntry* lookupEntry(String id, LookupType lookup=LOOKUP_ALL_SCOPES)
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

        VariableEntry* lookupVariable(String id, LookupType lookup=LOOKUP_ALL_SCOPES)
        {
            SymbolEntry *e = lookupEntry(id, lookup);

            if (e != nullptr && (e->entryType == ENTRY_VARIABLE || e->entryType == ENTRY_PARAMETER)) {
                return (VariableEntry *)e;
            }
            else {
                return nullptr;
            }
            
        }

        LabelEntry* lookupLabel(String id)
        {
            SymbolEntry *e = lookupEntry(id, LOOKUP_CURRENT_SCOPE);

            if (e != nullptr && e->entryType == ENTRY_LABEL) {
                return (LabelEntry *)e;
            }
            else {
                return nullptr;
            }
            
        }

        FunctionEntry* lookupFunction(String id, LookupType lookup)
        {
            SymbolEntry *e = lookupEntry(id, lookup);

            if (e != nullptr && e->entryType == ENTRY_FUNCTION) {
                return (FunctionEntry *)e;
            }
            else {
                return nullptr;
            }
            
        }
};


/* ------------------------------------------------------------------------------------------------------------------------------------------------------ */



inline std::ostream& operator<<(std::ostream& os, const Stype& t){
    switch(t->kind) {
        case TYPE_VOID :  os << "void"; break;
        case TYPE_INTEGER : os << "int"; break;
        case TYPE_BOOLEAN : os << "bool"; break;
        case TYPE_CHAR : os << "char"; break;
        case TYPE_REAL :  os << "real"; break;
        case TYPE_ARRAY : os << "array of " << t->refType << '[' << t->size << ']'; break;
        case TYPE_IARRAY : os << "array of " << t->refType; break;
        case TYPE_POINTER : os << t->refType << "*"; break;
    }
    return os ;
}

/* ---------------------------- CODE GENERATION TABLE ------------------------ */

struct CodeGenEntry {
  int offset;
  llvm::Value* value; 
  CodeGenEntry() {}
  CodeGenEntry(llvm::Value* v, int ofs) : value(v), offset(ofs) {}
};

class CodeGenScope {
public:
  CodeGenScope() : locals(), offset(-1), size(0) {}
  CodeGenScope(int ofs) : locals(), offset(ofs), size(0) {}
  int getOffset() const { return offset; }
  int getSize() const { return size; }
  CodeGenEntry *lookup(std::string name) {
    if (locals.find(name) == locals.end()) return nullptr;
    return &(locals[name]);
  }
  int insert(std::string name, llvm::Value* v) {
    if (locals.find(name) != locals.end()) {
      return 1;
    }
    locals[name] = CodeGenEntry(v, offset++);
    ++size;
    return 0; 
  }
private:
  std::map<std::string, CodeGenEntry> locals;
  int offset;
  int size;
};


class CodeGenTable {
/*  - lookup(name::string) : search for a name in SymbolTable
    return nullptr upon not founding 
    - insert(name::string, v::llvm::Value*)
    return 0 on correct insertion
    return 1 on duplicate entry 
*/ 
public:
  void openScope() {
    int ofs = scopes.empty() ? 0 : scopes.back().getOffset();
    scopes.push_back(CodeGenScope(ofs));
  }
  void closeScope() { scopes.pop_back(); };
  CodeGenEntry *lookup(std::string name) {
    CodeGenEntry *e;
    for (auto i = scopes.rbegin(); i != scopes.rend(); ++i) {
      e = i->lookup(name);
      if (e != nullptr) return e;
    }    
    return e; 
  }
  int getSizeOfCurrentScope() const { return scopes.back().getSize(); }
  int insert(std::string name, llvm::Value* v) { return scopes.back().insert(name, v); }
private:
  std::vector<CodeGenScope> scopes;
};

#endif