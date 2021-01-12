#pragma once

// Stype for llvm::Type disambiguation 
typedef enum 
{                       
   TYPE_VOID, 
   TYPE_INTEGER, 
   TYPE_BOOLEAN, 
   TYPE_CHAR,   
   TYPE_REAL, 
   TYPE_ARRAY, 
   TYPE_IARRAY, 
   TYPE_POINTER
} OfType;

class SemanticType
{
    public: 
      OfType kind; 
      SemanticType* refType; 
      int size;

      SemanticType(OfType k, SemanticType* t, int s) : kind(k) , refType(t) , size(s) {};

      bool equals(SemanticType* targetType)
      {
        if (kind != targetType->kind) return false;
        if (kind == TYPE_ARRAY && size != targetType->size) return false;
        if (kind == TYPE_POINTER || kind == TYPE_IARRAY || kind == TYPE_ARRAY)
            return refType->equals(targetType->refType);
        return true;
      }
};

typedef  SemanticType* Stype;

extern Stype typeInteger;
extern Stype typeReal;
extern Stype typeBoolean;
extern Stype typeChar;
extern Stype typeVoid;

extern Stype typeIArray(Stype refType);
extern Stype typeArray(int size,Stype refType);
extern Stype typePointer(Stype refType);