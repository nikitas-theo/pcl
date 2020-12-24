#ifndef __SYMBOL_COMPATIBLE__
#define __SYMBOL_COMPATIBLE__

#include <stdlib.h> 
#include <string>
#include <iostream>
inline std::ostream& operator<<(std::ostream& os, const SymType& t){
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


SymbolEntry * newVariable        (std::string name, SymType type);
SymbolEntry * newConstant        (std::string name, SymType type, ...);
SymbolEntry * newFunction        (std::string name);
SymbolEntry * newParameter       (std::string name, SymType type,
                                  PassMode mode, SymbolEntry * f);
SymbolEntry * lookupEntry        (std::string name, LookupType type,
                                  bool err);

#endif
