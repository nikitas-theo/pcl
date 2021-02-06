#pragma once

#include <list>
#include <string>

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

      bool is_concrete()
      {
        return kind != TYPE_IARRAY;
      }

      /* One way function: parameter type IS ASSIGNMENT COMPATIBLE with the caller. */
      bool is_compatible_with(SemanticType* t)
      {
        switch(kind)
        {
          case TYPE_BOOLEAN:
          case TYPE_CHAR:
          case TYPE_INTEGER:
            return t->kind == kind;
          case TYPE_REAL:
            return t->kind == TYPE_REAL || t->kind == TYPE_INTEGER;
          case TYPE_POINTER:
            return t->kind == TYPE_POINTER && refType->is_compatible_with(t->refType);
          case TYPE_ARRAY:
          case TYPE_IARRAY:
            return t->kind == TYPE_ARRAY && refType->equals(t->refType);
          default:
            return false;
        }
      }
};

typedef  SemanticType* Stype;

typedef enum {            
   PASS_BY_VALUE,                        /* Κατ' αξία                  */
   PASS_BY_REFERENCE                     /* Κατ' αναφορά               */
} PassMode;

typedef struct
{
    std::list<std::string> names;
    Stype type;
    PassMode pmode;
} ParameterGroup;

extern Stype typeInteger;
extern Stype typeReal;
extern Stype typeBoolean;
extern Stype typeChar;
extern Stype typeVoid;

extern Stype typeIArray(Stype refType);
extern Stype typeArray(int size,Stype refType);
extern Stype typePointer(Stype refType);