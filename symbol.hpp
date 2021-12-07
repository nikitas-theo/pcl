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
        bool isTargeted = false; 
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
        std::vector<std::string> labelNames; 
        FunctionDef *function; 
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
        Scope* currentScope = nullptr;
        
        SymbolTable();

        ~SymbolTable();

        // void Initialize();

        void openScope(FunctionDef*);

        void closeScope();

        void addEntry(SymbolEntry* e);



        SymbolEntry* lookupEntry(String id, LookupType lookup=LOOKUP_ALL_SCOPES);

        VariableEntry* lookupVariable(String id, LookupType lookup=LOOKUP_ALL_SCOPES);

        LabelEntry* lookupLabel(String id);

        FunctionEntry* lookupFunction(String id, LookupType lookup);
};


/* ------------------------------------------------------------------------------------------------------------------------------------------------------ */

/* ---------------------------- CODE GENERATION TABLE ------------------------ */
// should move this to .cpp 

class CodeGenEntry {
public:
  int offset;
  llvm::Value* value; 
  PassMode pass_by;
  bool is_library_fun = false; 
  std::vector<PassMode> arguments; 
  std::vector<Stype> types; 
  Label* label;
  std::vector<GoTo*> goto_nodes;
  CodeGenEntry() {}
  CodeGenEntry(llvm::Value* v, int ofs, PassMode pb) : value(v), offset(ofs), pass_by(pb) {}
};

class CodeGenScope {

public:

  CodeGenScope() : locals(), offset(-1), size(0) {}
  CodeGenScope(int ofs) : locals(), offset(ofs), size(0) {}
  
  

  int getOffset() const { return offset; }
  int getSize() const { return size; }

  std::vector<std::string> label_names; 

  FunctionDef *function; 

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
  void openScope(FunctionDef *fun) {

    int ofs = scopes.empty() ? 0 : scopes.back().getOffset();

    scopes.push_back(CodeGenScope(ofs));
    scopes.back().function = fun;
  }

  void closeScope() { 

    for (std::string l : scopes.back().label_names){
      CodeGenEntry *e = lookup(l);
      for (GoTo * g : e->goto_nodes) g->compile_final(e->label);
    }


    scopes.pop_back(); 
    };

  CodeGenEntry *lookup(std::string name) {

    CodeGenEntry *e;

    for (auto i = scopes.rbegin(); i != scopes.rend(); ++i) {
      e = i->lookup(name);
      if (e != nullptr) return e;
    }    

    return e; 
  }

  void addToLabel(std::string label, GoTo* g){
      CodeGenEntry *l = lookup(label);
      l->goto_nodes.push_back(g);
  }
  void addLabel(std::string label){
    scopes.back().label_names.push_back(label);
  }
  FunctionDef *get_fun(){
    return  scopes.back().function;
  }
  int getSizeOfCurrentScope() const { return scopes.back().getSize(); }
  int insert(std::string name, llvm::Value* v,PassMode pb = PASS_BY_VALUE) { return scopes.back().insert(name, v, pb); }

private:

  std::list<CodeGenScope> scopes;
};

// #endif