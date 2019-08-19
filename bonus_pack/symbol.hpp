#pragma once

#include <cstdlib>
#include <vector>

enum Type { TYPE_int, TYPE_bool };

struct SymbolEntry {
  Type type;
  int offset;
  SymbolEntry() {}
  SymbolEntry(Type t, int ofs) : type(t), offset(ofs) {}
};

class Scope {
public:
  Scope() : locals(), offset(-1), size(0) {}
  Scope(int ofs) : locals(), offset(ofs), size(0) {}
  int getOffset() const { return offset; }
  int getSize() const { return size; }
  SymbolEntry *lookup(char c) {
    if (locals.find(c) == locals.end()) return nullptr;
    return &(locals[c]);
  }
  void insert(char c, Type t) {
    if (locals.find(c) != locals.end()) {
      std::cerr << "Duplicate variable " << c << std::endl;
      exit(1);
    }
    locals[c] = SymbolEntry(t, offset++);
    ++size;
  }
private:
  std::map<char, SymbolEntry> locals;
  int offset;
  int size;
};

class SymbolTable {
public:
  void openScope() {
    int ofs = scopes.empty() ? 0 : scopes.back().getOffset();
    scopes.push_back(Scope(ofs));
  }
  void closeScope() { scopes.pop_back(); };
  SymbolEntry *lookup(char c) {
    for (auto i = scopes.rbegin(); i != scopes.rend(); ++i) {
      SymbolEntry *e = i->lookup(c);
      if (e != nullptr) return e;
    }
    std::cerr << "Unknown variable " << c << std::endl;
    exit(1);
  }
  int getSizeOfCurrentScope() const { return scopes.back().getSize(); }
  void insert(char c, Type t) { scopes.back().insert(c, t); }
private:
  std::vector<Scope> scopes;
};

extern SymbolTable st;
