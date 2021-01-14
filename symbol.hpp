#pragma once

#include <vector>
#include <map> 
#include <iosfwd>

#include "types.hpp"
#include "ast.hpp"

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


/* -------------------- SYMBOL TABLE ---------------------------- */



struct SymbolEntry {
  int offset;
  Stype type; 
  // set depth upon lookup
  int depth;
  SymbolEntry() {}
  SymbolEntry(Stype v, int ofs) : type(v), offset(ofs) {}
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
  int insert(std::string name, Stype v) {
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

// TODO : handle symboltable errors
class SymbolTable {
/*
  Modify this to account for error messages
  when we insert and a name is already defined in the same scope it should be an eror 
  when we lookup and it's not there it probably should be error etc. 

  - lookup(name::string) : search for a name in SymbolTable
    return nullptr upon not founding 
  
  - insert(name::string, v::llvm::Value*)
    return 0 on correct insertion
    return 1 on duplicate entry 
*/
public:
  SymbolTable() {openScope();}
  void openScope() {
    int ofs = scopes.empty() ? 0 : scopes.back().getOffset();
    scopes.push_back(Scope(ofs));
  }
  void closeScope() { scopes.pop_back(); };
  SymbolEntry *lookup(std::string name) {
    SymbolEntry *e;
    int depth = 0;
    for (auto i = scopes.rbegin(); i != scopes.rend(); ++i) {
      e = i->lookup(name);
      depth++; 
      if (e != nullptr){
        e->depth = depth;
        return e;
      }
    }
    return e; 
  }

  int getSizeOfCurrentScope() const { return scopes.back().getSize(); }
  int insert(std::string name, Stype v) { return scopes.back().insert(name, v); }

private:
  std::vector<Scope> scopes;
};




