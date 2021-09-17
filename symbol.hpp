// #ifndef __SYMBOL_H__
// #define __SYMBOL_H__
#pragma once

#include <map>
#include <unordered_map>
#include <string>
#include <deque>
#include <functional>
#include <utility>
#include <list>
#include <vector>
#include <iosfwd>

#include "types.hpp"
#include "ast.hpp"

typedef int           RepInteger;        
typedef unsigned char RepBoolean;        
typedef char          RepChar;           
typedef long double   RepReal;           
typedef const char *  RepString;        

typedef union {                    
    RepInteger vInteger;           
    RepBoolean vBoolean;           
    RepChar    vChar;              
    RepReal    vReal;              
    RepString  vString;            
} ConstValueType;


typedef enum {            
   ENTRY_VARIABLE,                    
   ENTRY_FUNCTION,                    
   ENTRY_LABEL,                       
} EntryType;


typedef enum {                       
    PARDEF_DEFINE,                   
    PARDEF_PENDING_CHECK,             
    PARDEF_COMPLETE                   
} PardefType;



typedef enum {
    LOOKUP_CURRENT_SCOPE,
    LOOKUP_ALL_SCOPES
} LookupType;


typedef size_t HashType;
typedef std::string String;


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

class LabelEntry : public SymbolEntry
{
    public:
        bool isBound;

        LabelEntry(String name) : SymbolEntry(name, ENTRY_LABEL), isBound(false) {}
};


class FunctionEntry : public SymbolEntry
{
    public:
        PardefType pardef;
        std::list<ParameterGroup *> arguments;

        Stype resultType;

        FunctionEntry(String name) : SymbolEntry(name, ENTRY_FUNCTION) {}

        ~FunctionEntry();
};

class Scope
{
    private:
        HashMap* levelEntries;
        int dummy_variable;

    public:
        int nestingLevel;
        // AST* owingFunctionNode;

        Scope();

        ~Scope();

        bool addEntry(SymbolEntry* e);

        SymbolEntry* lookupEntry(String id);
};

class SymbolTable
{
    private:
        std::list<Scope*> scopeStack;

    public:
        Scope* currentScope;
        
        SymbolTable();

        ~SymbolTable();

        // void Initialize();

        void openScope();

        void closeScope();

        void addEntry(SymbolEntry* e);

        SymbolEntry* lookupEntry(String id, LookupType lookup=LOOKUP_ALL_SCOPES);

        VariableEntry* lookupVariable(String id, LookupType lookup=LOOKUP_ALL_SCOPES);

        LabelEntry* lookupLabel(String id);

        FunctionEntry* lookupFunction(String id, LookupType lookup);
};


/* ------------------------------------------------------------------------------------------------------------------------------------------------------ */

/* ---------------------------- CODE GENERATION TABLE ------------------------ */

class CodeGenEntry {
public:
  int offset;
  llvm::Value* value; 
  PassMode pass_by;
  std::vector<PassMode> arguments; 
  std::vector<Stype> types; 
  CodeGenEntry() {}
  CodeGenEntry(llvm::Value* v, int ofs, PassMode pb) : value(v), offset(ofs), pass_by(pb) {}

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
  int insert(std::string name, llvm::Value* v, PassMode pb) {

    if (locals.find(name) != locals.end()) {
      return 1;
    }
    locals[name] = CodeGenEntry(v, offset++,pb);
    ++size;

    return 0; 
  }


private:

  std::map<std::string, CodeGenEntry> locals;
  int offset;
  int size;

};


class CodeGenTable {
/*  
  
  - lookup(name::string) : search for a name in SymbolTable
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
  int insert(std::string name, llvm::Value* v,PassMode pb = PASS_BY_VALUE) { return scopes.back().insert(name, v, pb); }
private:

  std::list<CodeGenScope> scopes;
};

// #endif