#pragma once

#include <cstdlib>
#include <vector>
#include <map> 
#include <iostream>
#include <llvm/IR/Value.h>

// Stype for llvm::Type disambiguation 
typedef enum {                       
   TYPE_VOID, TYPE_INTEGER, TYPE_BOOLEAN, TYPE_CHAR,   
   TYPE_REAL, TYPE_ARRAY, TYPE_IARRAY, TYPE_POINTER 
} oftype;


class TypeClass {
    public: 
    oftype kind; 
    TypeClass* refType; 
    int size;
    TypeClass(oftype k, TypeClass* t, int s) : kind(k) , refType(t) , size(s) {};
};
typedef  TypeClass* Stype;

extern Stype typeInteger;
extern Stype typeReal;
extern Stype typeBoolean;
extern Stype typeChar;
extern Stype typeVoid;


extern Stype typeIArray(Stype refType) ;
extern Stype typeArray(int size,Stype refType) ;
extern Stype typePointer(Stype refType) ;

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


struct SymbolEntry {
  int offset;
  // for code generation
  llvm::Value* value; 
  SymbolEntry() {}
  SymbolEntry(llvm::Value* v, int ofs) : value(v), offset(ofs) {}
};

class Scope {
public:
  Scope() : locals(), offset(-1), size(0) {}
  Scope(int ofs) : locals(), offset(ofs), size(0) {}
  int getOffset() const { return offset; }
  int getSize() const { return size; }
  SymbolEntry *lookup(std::string name) {
    if (locals.find(name) == locals.end()) return nullptr;
    return &(locals[name]);
  }
  int insert(std::string name, llvm::Value* v) {
    if (locals.find(name) != locals.end()) {
      return 1;
    }
    locals[name] = SymbolEntry(v, offset++);
    ++size;
    return 0; 
  }
private:
  std::map<std::string, SymbolEntry> locals;
  int offset;
  int size;
};

class SymbolTable {
/*  - lookup(name::string) : search for a name in SymbolTable
    return nullptr upon not founding 

    - insert(name::string, v::llvm::Value*)
    return 0 on correct insertion
    return 1 on duplicate entry 
*/ 
public:
  void openScope() {
    int ofs = scopes.empty() ? 0 : scopes.back().getOffset();
    scopes.push_back(Scope(ofs));
  }
  void closeScope() { scopes.pop_back(); };
  SymbolEntry *lookup(std::string name) {
    for (auto i = scopes.rbegin(); i != scopes.rend(); ++i) {
      SymbolEntry *e = i->lookup(name);
      return e; 
    }    
    return nullptr; 
  }
  int getSizeOfCurrentScope() const { return scopes.back().getSize(); }
  int insert(std::string name, llvm::Value* v) { return scopes.back().insert(name, v); }
private:
  std::vector<Scope> scopes;
};

