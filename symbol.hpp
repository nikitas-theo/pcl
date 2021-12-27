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
        FunctionDef* function;
        FunctionEntry(String name, FunctionDef* function) : SymbolEntry(name, ENTRY_FUNCTION) , function(function) {}

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

        void openScope(FunctionDef* f, FunctionDef* forward = nullptr);

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
    bool is_library_fun = false; 
    std::vector<PassMode> arguments; 
    std::vector<Stype> types; 
    int nesting_level;
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

    // we need label names to jump to label defined afterwards 
    std::vector<std::string> label_names; 

    // every scope is associated with a function
    // we need it to create hidden structs 
    FunctionDef *function; 

    CodeGenEntry *lookup(std::string name);
    int insert(std::string name, llvm::Value* v, PassMode pb);

  private:

    std::map<std::string, CodeGenEntry> locals;
    int offset;
    int size;

};


class CodeGenTable {

  public:

    void openScope(FunctionDef *fun);
    void closeScope();

    CodeGenEntry *lookup(std::string name, LookupType lookup = LOOKUP_ALL_SCOPES);

    void addToLabel(std::string label, GoTo* g);
    void addLabel(std::string label);

    FunctionDef *get_fun();

    int getSizeOfCurrentScope();
    int getNumScopes();
    int insert(std::string name, llvm::Value* v,PassMode pb = PASS_BY_VALUE);

  private:
    std::list<CodeGenScope> scopes;

};

// #endif