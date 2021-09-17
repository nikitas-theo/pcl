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
      bool pointer_special_case(SemanticType* t){
          return kind == TYPE_POINTER && t->kind == TYPE_POINTER &&
          refType->kind == TYPE_IARRAY && t->refType->kind == TYPE_ARRAY
          &&  refType->refType->equals(t->refType->refType) ;
      }
      /* One way function: parameter type IS ASSIGNMENT COMPATIBLE with the caller. */
      bool is_compatible_with(SemanticType* t)
      {
        switch(kind)
        {
          case TYPE_BOOLEAN:
          case TYPE_CHAR:
          case TYPE_INTEGER:
            return this->equals(t);
          case TYPE_REAL:
            return t->kind == TYPE_REAL || t->kind == TYPE_INTEGER;
          case TYPE_POINTER:
            return this->equals(t) || this->pointer_special_case(t);            
          case TYPE_ARRAY:
            return this->equals(t);
          case TYPE_IARRAY:
            return false;  
          default:
            return false;
        }
      }
};

typedef  SemanticType* Stype;

typedef enum {            
   PASS_BY_VALUE,
   PASS_BY_REFERENCE
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