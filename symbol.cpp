#include "symbol.hpp"

Stype typeInteger = new TypeClass(TYPE_INTEGER,nullptr,0);
Stype typeReal = new TypeClass(TYPE_REAL,nullptr,0);
Stype typeBoolean = new TypeClass(TYPE_BOOLEAN,nullptr,0);
Stype typeChar = new TypeClass(TYPE_CHAR,nullptr,0);
Stype typeVoid = new TypeClass(TYPE_VOID,nullptr,0);


Stype typeIArray(Stype refType) { return new TypeClass(TYPE_IARRAY,refType,0);}
Stype typeArray(int size,Stype refType) { return new TypeClass(TYPE_ARRAY,refType,size);}
Stype typePointer(Stype refType) { return new TypeClass(TYPE_POINTER,refType,0);}
