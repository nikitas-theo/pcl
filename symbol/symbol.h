/******************************************************************************
 *  CVS version:
 *     $Id: symbol.h,v 1.1 2003/05/13 22:21:01 nickie Exp $
 ******************************************************************************
 *
 *  C header file : symbol.h
 *  Project       : PCL Compiler
 *  Version       : 1.0 alpha
 *  Written by    : Nikolaos S. Papaspyrou (nickie@softlab.ntua.gr)
 *  Date          : May 14, 2003
 *  Description   : Generic symbol table in C
 *
 *  Comments: (in Greek iso-8859-7)
 *  ---------
 *  ������ �������� �����������.
 *  ����� ������������ ��������� ��� ��������� �����������.
 *  ������ ����������� ������������ ��� �����������.
 *  ���������� ����������� ����������
 */

/* Include guard goes here */

#ifdef __cplusplus
extern "C" {
#endif


#ifndef __SYMBOL_H__
#define __SYMBOL_H__


/* ---------------------------------------------------------------------
   -------------------------- ����� bool -------------------------------
   --------------------------------------------------------------------- */

#include <stdbool.h>

/*
 *  �� �� �������� include ��� ������������� ��� ��� ���������
 *  ��� C ��� ��������������, �������������� �� �� �� ��������:
 */

#if 0
typedef enum { false=0, true=1 } bool;
#endif


/* ---------------------------------------------------------------------
   ------------ ������� �������� ��� ������ �������� -------------------
   --------------------------------------------------------------------- */

#define START_POSITIVE_OFFSET 8     /* ������ ������ offset ��� �.�.   */
#define START_NEGATIVE_OFFSET 0     /* ������ �������� offset ��� �.�. */


/* ---------------------------------------------------------------------
   --------------- ������� ����� ��� ������ �������� -------------------
   --------------------------------------------------------------------- */

/* ����� ��������� ��� ��� ��������� ��� �������� */

typedef int           RepInteger;         /* ��������                  */
typedef unsigned char RepBoolean;         /* ������� �����             */
typedef char          RepChar;            /* ����������                */
typedef long double   RepReal;            /* �����������               */
typedef const char *  RepString;          /* �������������             */


/* ����� ��������� ��� ������������� ����������� */

typedef struct Type_tag * SymType;
typedef enum {                       /***** �� ����� ��� ����� ****/
   TYPE_VOID    = 0b00000001,        /* ����� ����� ������������� */
   TYPE_INTEGER = 0b00000010,        /* ��������                  */
   TYPE_BOOLEAN = 0b00000100,        /* ������� �����             */
   TYPE_CHAR    = 0b00001000,        /* ����������                */
   TYPE_REAL    = 0b00010000,        /* �����������               */
   TYPE_ARRAY   = 0b00100000,        /* ������� ������� ��������  */
   TYPE_IARRAY  = 0b01000000,        /* ������� �������� �������� (Incomplete) */
   TYPE_POINTER = 0b10000000         /* �������                   */
} oftype;

struct Type_tag {
    oftype         kind;
    SymType        refType;              /* ����� �������� (��� array, pointer)           */
    RepInteger     size;                 /* �������, �� ����� ������� */
    unsigned int   refCount;             /* �������� ��������         */
};


/* ����� �������� ��� ������ �������� */

typedef enum {            
   ENTRY_VARIABLE,                       /* ����������                 */
   ENTRY_CONSTANT,                       /* ��������                   */
   ENTRY_FUNCTION,                       /* �����������                */
   ENTRY_PARAMETER,                      /* ���������� �����������     */
   ENTRY_TEMPORARY                       /* ���������� ����������      */
} EntryType;


/* ����� ���������� ���������� */

typedef enum {            
   PASS_BY_VALUE,                        /* ���' ����                  */
   PASS_BY_REFERENCE                     /* ���' �������               */
} PassMode;


/* ����� �������� ���� ������ �������� */

typedef struct SymbolEntry_tag SymbolEntry;

struct SymbolEntry_tag {
   const char   * id;                 /* ����� ��������������          */
   EntryType      entryType;          /* ����� ��� ��������            */
   unsigned int   nestingLevel;       /* ����� �����������             */
   unsigned int   hashValue;          /* ���� ���������������          */
   SymbolEntry  * nextHash;           /* ������� ������� ���� �.�.     */
   SymbolEntry  * nextInScope;        /* ������� ������� ���� �������� */

   union {                            /* ������� �� ��� ���� ��������: */

      struct {                                /******* ��������� *******/
         SymType          type;                  /* �����                 */
         int           offset;                /* Offset ��� �.�.       */
      } eVariable;

      struct {                                /******** ������� ********/
         SymType          type;                  /* �����                 */
         union {                              /* ����                  */
            RepInteger vInteger;              /*    �������            */
            RepBoolean vBoolean;              /*    ������             */
            RepChar    vChar;                 /*    ����������         */
            RepReal    vReal;                 /*    ����������         */
            RepString  vString;               /*    ������������       */
         } value;
      } eConstant;

      struct {                                /******* ��������� *******/
         bool          isForward;             /* ������ forward        */
         SymbolEntry * firstArgument;         /* ����� ����������      */
         SymbolEntry * lastArgument;          /* ��������� ����������  */
         SymType          resultType;            /* ����� �������������   */
         enum {                               /* ��������� ����������  */
             PARDEF_COMPLETE,                    /* ������ �������     */
             PARDEF_DEFINE,                      /* �� ���� �������    */
             PARDEF_CHECK                        /* �� ���� �������    */
         } pardef;
         int           firstQuad;             /* ������ �������        */
      } eFunction;

      struct {                                /****** ���������� *******/
         SymType          type;                  /* �����                 */
         int           offset;                /* Offset ��� �.�.       */
         PassMode      mode;                  /* ������ ����������     */
         SymbolEntry * next;                  /* ������� ����������    */
      } eParameter;

      struct {                                /** ��������� ��������� **/
         SymType          type;                  /* �����                 */
         int           offset;                /* Offset ��� �.�.       */
         int           number;
      } eTemporary;

   } u;                               /* ����� ��� union               */
};


/* ����� ������� �������� ��� ���������� ���� ���� �������� */

typedef struct Scope_tag Scope;

struct Scope_tag {
    unsigned int   nestingLevel;             /* ����� �����������      */
    unsigned int   negOffset;                /* ������ �������� offset */
    Scope        * parent;                   /* ������������ ��������  */
    SymbolEntry  * entries;                  /* ������� ��� ���������  */
};


/* ����� ���������� ���� ������ �������� */

typedef enum {
    LOOKUP_CURRENT_SCOPE,
    LOOKUP_ALL_SCOPES
} LookupType;


/* ---------------------------------------------------------------------
   ------------- ��������� ���������� ��� ������ �������� --------------
   --------------------------------------------------------------------- */

extern Scope        * currentScope;       /* �������� ��������         */
extern unsigned int   quadNext;           /* ������� �������� �������� */
extern unsigned int   tempNumber;         /* �������� ��� temporaries  */

extern const SymType typeVoid;
extern const SymType typeInteger;
extern const SymType typeBoolean;
extern const SymType typeChar;
extern const SymType typeReal;


/* ---------------------------------------------------------------------
   ------ ��������� ��� ����������� ��������� ��� ������ �������� ------
   --------------------------------------------------------------------- */

void          initSymbolTable    (unsigned int size);
void          destroySymbolTable (void);

void          openScope          (void);
void          closeScope         (void);

SymbolEntry * newVariable        (const char * name, SymType type);
SymbolEntry * newConstant        (const char * name, SymType type, ...);
SymbolEntry * newFunction        (const char * name);
SymbolEntry * newParameter       (const char * name, SymType type,
                                  PassMode mode, SymbolEntry * f);
SymbolEntry * newTemporary       (SymType type);

void          forwardFunction    (SymbolEntry * f);
void          endFunctionHeader  (SymbolEntry * f, SymType type);
void          destroyEntry       (SymbolEntry * e);
SymbolEntry * lookupEntry        (const char * name, LookupType type,
                                  bool err);

SymType          typeArray          (RepInteger size, SymType refType);
SymType          typeIArray         (SymType refType);
SymType          typePointer        (SymType refType);
void          destroyType        (SymType type);
unsigned int  sizeOfType         (SymType type);
bool          equalType          (SymType type1, SymType type2);
void          printType          (SymType type);
void          printMode          (PassMode mode);


#endif


#ifdef __cplusplus
}
#endif

/* #endif of the include guard here */
