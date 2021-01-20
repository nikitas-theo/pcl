#pragma once 
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <variant> 

// #include "symbol.hpp"
// #include "external.hpp"
#include "types.hpp"

#include <llvm/IR/Function.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Type.h>


// for garbage collection of malloc arrays 
// add -lgc to lib flags 

using namespace llvm;

// ConstantInt* c_i32(int n);
// ConstantInt* c_i8(char c);
// ConstantInt* c_i1(int n);
// ConstantFP* c_r64(double d);

extern Type* i1; 
extern Type* i8; 
extern Type* i32;
extern Type* i64;
extern Type* r64;
extern Type* voidTy;

typedef std::vector<std::string> IdCollection;

typedef std::variant<int,double,char,bool> data_const ;
// https://en.cppreference.com/w/cpp/utility/variant/holds_alternative
// https://en.cppreference.com/w/cpp/utility/variant

typedef enum
{
    PASS_BY_REFERENCE,
    PASS_BY_VALUE
} PassMode;

extern void error(const char* str);  

class AST
{
    private:
        inline void add_func(FunctionType *type, std::string name);
        inline void add_libs();
        
    public:
        virtual ~AST() {}

        virtual void semantic() = 0;

        virtual void printOn(std::ostream &out) const = 0;

        virtual Value* compile() = 0;
        void compile_llvm(std::string  program_name, bool optimize, bool imm_stdout );

        Type* TypeConvert(Stype t);
        bool check_type(Stype t1,Stype t2,bool check_size = true);
};

inline std::ostream& operator<<(std::ostream &out, const AST &t)
{
  t.printOn(out);
  return out;
}

inline std::ostream& operator<<(std::ostream &out, const std::vector<std::string> &t)
{
        out << "[";
        for (std::size_t i = 0; i < t.size(); i++) {
            out << t[t.size() - 1 - i] << (i == t.size() - 1 ? "" : ",");
        }
        return out << "]" ;
}

class Expr : public AST
{
    public:
        Stype type;
        bool lvalue = false;
        Expr(Stype t) : type(t) {};
        Expr() {};
        bool is_arithmetic();
        bool is_concrete();
        bool type_verify(Stype t);
};

class Stmt : public AST {};

template<class T>
class ASTvector : public Stmt , public Expr
{
    public :
        std::vector<T> list;

        void printOn(std::ostream &out) const
        {
            out << "[";
            for (std::size_t i = 0; i < list.size(); i++) {
                out << *(list[list.size() - 1 - i]) << (i == list.size() - 1 ? "" : ",");
            }
            out << "]"; 
        }

        void semantic()
        {

        }
        
        Value* compile()
        {
            return nullptr;
        }

        void push(T t)
        {
            list.push_back(t);
        }

};

constexpr unsigned int hashf(const char *s, int off = 0)
{                        
    return !s[off] ? 5381 : (hashf(s, off+1)*33) ^ s[off];                           
}

constexpr inline unsigned int operator "" _(char const * p, size_t)
{
    return hashf(p);
}


class EmptyStmt : public Stmt {
    void printOn(std::ostream &out) const;
    void semantic();
    Value* compile(); 
};



class BinOp : public Expr {
    private:
        Expr *left, *right;
        std::string op;        
    public:
        BinOp(Expr* l, std::string o, Expr* r) : left(l), right(r), op(o) {}
        
        void printOn(std::ostream &out) const;
        void semantic();
        Value* compile(); 
};

class UnOp : public Expr {
    private:
        Expr *e;
        std::string op;
        
    public:
        UnOp(std::string _o, Expr* _e) : op(_o), e(_e) {}
        
        void printOn(std::ostream &out) const;
        void semantic();
        Value* compile(); 
};

class Id : public Expr {
    /* Id as an l-value */
    public:
        std::string name ;
        Id(std::string n) : name(n)
        { 
            this->lvalue = true; 
        }
        
        void printOn(std::ostream &out) const;
        void semantic();
        Value* compile(); 
};

class Const : public Expr {

    public:
        data_const val;     
        Const(data_const val, Stype t) : val(val) , type(t) {};       
        void printOn(std::ostream &out) const;
        void semantic();
        Value* compile(); 
};

class CallFunc : public Expr {
    private:
        std::string fname;
        ASTvector<Expr*> parameters;
    public:
        CallFunc(std::string name, ASTvector<Expr*>* params) : fname(name), parameters(*params) {}
        CallFunc(std::string name) : fname(name) {}
        
        void printOn(std::ostream &out) const;
        void semantic();
        Value* compile(); 
};

class CallProc : public Stmt {
    private:
        std::string fname;
        ASTvector<Expr*> parameters;
    public:
        CallProc(std::string name, ASTvector<Expr*>* params) : fname(name), parameters(*params) {}
        CallProc(std::string name) : fname(name){}
        
        void printOn(std::ostream &out) const;
        void semantic();
        Value* compile(); 
};

class StringLiteral : public Expr {
    /*
        l-value, type : array[n] of char
        n = #characters + '\0'
        the only array-type constant
        so you can use it as str[i], but not to assign since it's a constant 
    */
    private:
        const char* s; 
        Stype t; 
    public:
        StringLiteral(const char* s) : s(s), t(typeArray(std::string(s).length(),typeChar))
        {
            this->lvalue = true;
        }
        void ReplaceStringInPlace(std::string& subject, const std::string& search, const std::string& replace);
        
        void printOn(std::ostream &out) const;
        void semantic();
        Value* compile(); 
};

class ArrayAccess : public Expr {
    private:
        Expr* lval; 
        Expr* pos; 
    public:
        ArrayAccess(Expr* lval, Expr* pos) : lval(lval) , pos(pos){}
        
        void printOn(std::ostream &out) const;
        void semantic();
        Value* compile(); 
};

class Dereference : public Expr {
    private:
        Expr* e;
    public:
        Dereference(Expr* e) : e(e) {}
        
        void printOn(std::ostream &out) const;
        void semantic();
        Value* compile(); 
};

/* Statements */

class Block : public Stmt {
    /*
        This class implements the body of each structure

        local :
            variable definitions : VarDef( [variables]) , variable = {type,id}
            label definitions : LabelDef( [id] ) 
            a function definition : FuncDdef() 
            a "forward" function definition : Routine() with forward=True, body = undefined; 
                - a Routine is a structure and has its own Block() 
            ;
        body : one composite statement (or a list of statements)

    */
    private:
        ASTvector<Stmt*> locals;
        ASTvector<Stmt*> body;
    
    public:
        Block( ASTvector<Stmt*>* theBody) : body(*theBody){}

        void push_local(Stmt *l);
        
        void printOn(std::ostream &out) const;
        void semantic();
        Value* compile(); 
};


/* These 2 classes can be merged but we leave it as is for now 
*/ 

class Variable : public Stmt {
    public:
        Stype type;
        std::string name; 
        Variable(std::string s, Stype t) : name(s), type(t){}
    
        void printOn(std::ostream &out) const;
        void semantic();
        Value* compile(); 
};

class VarDef : public Stmt {
    /* Class containing variable definitions
    */ 
    private:
        ASTvector<Variable*> vars;    
    public:
        void push(std::vector<std::string>* var_ids, Stype t);
        void printOn(std::ostream &out) const;
        void semantic();
        Value* compile(); 
};

class LabelDef : public Stmt {
    private:
        std::vector<std::string> labels;
    public:
        LabelDef(std::vector<std::string>* names) : labels(*names) {}
        
        void printOn(std::ostream &out) const;
        void semantic();
        Value* compile(); 
};

class FormalsGroup : public Stmt {
    public:
        std::vector<std::string> formals;
        Stype type;
        PassMode pass_by;

        FormalsGroup(std::vector<std::string>* f, Stype t, PassMode pm) : formals(*f), type(t), pass_by(pm) {}
        
        void printOn(std::ostream &out) const;
        void semantic();
        Value* compile(); 
};


/*
    Function::getArgumentList()
    to get arguments of funcion in block code 
    so allocas should be replaced with the actual function
    arguments created internally by builder

    if a parameter is by reference we just pass a pointer
    instead of the actual parameter
*/ 
class FunctionDef : public Stmt {
    private:
        std::string name;
        Stype type;
        ASTvector<FormalsGroup*> parameters;
        Block* body;
        bool isForward;
        
    public:
        FunctionDef(std::string n, ASTvector<FormalsGroup*>* params, Stype t) 
        : name(n), parameters(*params), type(t) { isForward = false;}

        void set_forward();
        void add_body(Block* theBody);
        
        void printOn(std::ostream &out) const;
        void semantic();
        Value* compile(); 
};

class Declaration : public Stmt {
    private:
        Expr *lval,*rval ;
    public:
        Declaration(Expr* lval, Expr* rval) : lval(lval) , rval(rval) {}
        
        void printOn(std::ostream &out) const;
        void semantic();
        Value* compile(); 
};

class IfThenElse : public Stmt {
    private:
        Expr *cond;
        Stmt *st_then, *st_else = nullptr;
        bool hasElse; 
        
    public:
        IfThenElse(Expr* c, Stmt* t) : cond(c), st_then(t) , hasElse(false) {};
        IfThenElse(Expr* c, Stmt* t, Stmt* e) : cond(c), st_then(t), st_else(e) , hasElse(true) {};
        
        void printOn(std::ostream &out) const;
        void semantic();
        Value* compile(); 
};

class While : public Stmt {
    private:
        Expr *cond;
        Stmt *body;
        
    public:
        While(Expr* c, Stmt *b) : cond(c), body(b) {}
        
        void printOn(std::ostream &out) const;
        void semantic();
        Value* compile(); 
};

class Label : public Stmt {
/*
    Implements actual label encounter in code
*/
    private:
        std::string lbl;
        Stmt *target;
    public:
        Label(std::string name, Stmt* stmt) : lbl(name), target(stmt) {}
        
        void printOn(std::ostream &out) const;
        void semantic();
        Value* compile(); 
};

class GoTo : public Stmt {
    private:
        std::string label;
    public:
        GoTo(std::string lbl) : label(lbl) {}
        
        void printOn(std::ostream &out) const;
        void semantic();
        Value* compile(); 
};

class ReturnStmt : public Stmt {
    public: 
        ReturnStmt(){};
        
        void printOn(std::ostream &out) const;
        void semantic();
        Value* compile(); 
};


// could be merged but we leave it as 2 classes for now.
class Init : public Stmt {
    private:
        Expr *lval;
    public:
        Init(Expr* lval) : lval(lval) {}
        
        void printOn(std::ostream &out) const;
        void semantic();
        Value* compile(); 
};

class InitArray : public Stmt {
    private:
        Expr *lval;
        Expr *size;
    public:
        InitArray(Expr* lval, Expr* sz) : lval(lval), size(sz) {}
        
        void printOn(std::ostream &out) const;
        void semantic();
        Value* compile(); 
};

class Dispose : public Stmt {
    private:
        Expr *lval;
    public:
        Dispose(Expr *lval) : lval(lval) {} 
        
        void printOn(std::ostream &out) const;
        void semantic();
        Value* compile(); 
};

class DisposeArray : public Stmt {
    private:
        Expr *lval;
    public:
        Dispose(Expr *lval) : lval(lval) {} 
        
        void printOn(std::ostream &out) const;
        void semantic();
        Value* compile(); 
};

