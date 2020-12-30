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

template <class T>
struct SymbolEntry {
  int offset;
  // for code generation
  T* value; 
  SymbolEntry() {}
  SymbolEntry(T* v, int ofs) : value(v), offset(ofs) {}
};

template< class T> 
class Scope {
public:
  Scope() : locals(), offset(-1), size(0) {}
  Scope(int ofs) : locals(), offset(ofs), size(0) {}
  int getOffset() const { return offset; }
  int getSize() const { return size; }
  SymbolEntry<T> *lookup(std::string name) {
    if (locals.find(name) == locals.end()) return nullptr;
    return &(locals[name]);
  }
  int insert(std::string name, T* v) {
    if (locals.find(name) != locals.end()) {
      return 1;
    }
    locals[name] = SymbolEntry<T>(v, offset++);
    ++size;
    return 0; 
  }
private:
  std::map<std::string, SymbolEntry<T>> locals;
  int offset;
  int size;
};

class SymbolTable {
/*
  Modify this to account for error messages
  when we insert and a name is already defined in the same scope it should be an eror 
  when we lookup and it's not there it probably should be error etc. 
  if it's the same as with CodeGenTable we can merge, no problem. 
*/
public:
  void openScope() {
    int ofs = scopes.empty() ? 0 : scopes.back().getOffset();
    scopes.push_back(Scope<TypeClass>(ofs));
  }
  void closeScope() { scopes.pop_back(); };
  SymbolEntry<TypeClass> *lookup(std::string name) {
    SymbolEntry<TypeClass> *e;
    for (auto i = scopes.rbegin(); i != scopes.rend(); ++i) {
      e = i->lookup(name);
      if (e != nullptr) return e;
    }    
    return e; 
  }
  int getSizeOfCurrentScope() const { return scopes.back().getSize(); }
  int insert(std::string name, Stype v) { return scopes.back().insert(name, v); }
private:
  std::vector<Scope<TypeClass>> scopes;
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
    scopes.push_back(Scope<llvm::Value>(ofs));
  }
  void closeScope() { scopes.pop_back(); };
  SymbolEntry<llvm::Value> *lookup(std::string name) {
    SymbolEntry<llvm::Value> *e;
    for (auto i = scopes.rbegin(); i != scopes.rend(); ++i) {
      e = i->lookup(name);
      if (e != nullptr) return e;
    }    
    return e; 
  }
  int getSizeOfCurrentScope() const { return scopes.back().getSize(); }
  int insert(std::string name, llvm::Value* v) { return scopes.back().insert(name, v); }
private:
  std::vector<Scope<llvm::Value>> scopes;
};



