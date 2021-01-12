#include "symbol.hpp"

Stype typeInteger = new SemanticType(TYPE_INTEGER,nullptr,0);
Stype typeReal = new SemanticType(TYPE_REAL,nullptr,0);
Stype typeBoolean = new SemanticType(TYPE_BOOLEAN,nullptr,0);
Stype typeChar = new SemanticType(TYPE_CHAR,nullptr,0);
Stype typeVoid = new SemanticType(TYPE_VOID,nullptr,0);


Stype typeIArray(Stype refType) { return new SemanticType(TYPE_IARRAY,refType,0);}
Stype typeArray(int size,Stype refType) { return new SemanticType(TYPE_ARRAY,refType,size);}
Stype typePointer(Stype refType) { return new SemanticType(TYPE_POINTER,refType,0);}
