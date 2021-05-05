#pragma once 
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <variant>
#include <list>

// #include "symbol.hpp"
// #include "external.hpp"
#include "types.hpp"
#include "helpers.hpp"

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

// template<typename ...Ts>
// extern void error(Ts&&... args);

template<typename ...Ts>
void error(Ts&&... args)
{
    _error(std::cerr, args...) << "\n";
    std::exit(1);
}

class AST
{
    private:
        // inline void add_func(FunctionType *type, std::string name);
        // inline void add_libs();
        
    public:
        virtual ~AST() {}

        virtual void semantic() = 0;
        virtual void printOn(std::ostream &out) const = 0;
        virtual Value* compile() = 0;

        friend inline std::ostream& operator<<(std::ostream &out, const AST &t)
        {
            t.printOn(out);
            return out;
        }

        Type* TypeConvert(Stype t);
        bool check_type(Stype t1,Stype t2,bool check_size = true);
        
        int linecnt;

        // template<typename ...Ts>
        // void error(Ts&&... args);
        template<typename ...Ts>
        void error(Ts&&... args)
        {
            std::cerr << "ERROR in line " << linecnt << ":\n";
            _error(std::cerr, args...) << "\n";
            std::exit(1);
        }

};


class Program
{
    private:
        AST* rootNode;
        std::string program_name;
        bool optimize;
        bool imm_stdout;
        bool print_ast;

        Function* main;

        inline void add_func_llvm(FunctionType *type, std::string name);
        void add_libs_llvm();

        inline std::list<ParameterGroup*>* make_single_parameter(Stype type, PassMode pm);
        void add_lib_func_semantic(std::string name, Stype resultType, std::list<ParameterGroup*>* parameters);
    
    public:
        // Program(std::string name, AST* root) : program_name(name), rootNode(root), optimize(false), imm_stdout(false) {}
        Program() : optimize(false), imm_stdout(false), print_ast(false) {}

        void mark_optimizable(bool on=true);
        void mark_console_interactive(bool dump=true);
        void mark_printable(bool mark=true);

        bool is_console_interactive();

        void set_name_if_blank(std::string name);

        void attach_AST(AST* root);

        void semantic_initialize();
        void semantic_run();
        void semantic_finalize();

        void compile_initalize();
        void compile_run();
        void compile_finalize();
};


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

class ASTnodeCollection : public Stmt, public Expr
{
    
    public:
        std::list<AST *> nodes;

        void printOn(std::ostream &out) const;
        void semantic();
        
        Value* compile();

        void push(AST* node);
};

/* ------ legacy ---------

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

inline std::ostream& operator<<(std::ostream &out, const std::vector<std::string> &t)
{
        out << "[";
        for (std::size_t i = 0; i < t.size(); i++) {
            out << t[t.size() - 1 - i] << (i == t.size() - 1 ? "" : ",");
        }
        return out << "]" ;
}

};
*/ 

constexpr unsigned int hashf(const char *s, int off = 0)
{                        
    return !s[off] ? 5381 : (hashf(s, off+1)*33) ^ s[off];                           
}

constexpr inline unsigned int operator "" _(char const * p, size_t)
{
    return hashf(p);
}


class EmptyStmt : public Stmt {
    public : 
    EmptyStmt(int cnt) {linecnt = cnt;};
    void printOn(std::ostream &out) const;
    void semantic();
    Value* compile(); 
};



class BinOp : public Expr {
    private:
        Expr *left, *right;
        std::string op;        
    public:
        BinOp(Expr* l, std::string o, Expr* r, int cnt) : left(l), right(r), op(o) {linecnt = cnt;};
        
        void printOn(std::ostream &out) const;
        void semantic();
        Value* compile(); 
};

class UnOp : public Expr {
    private:
        Expr *e;
        std::string op;
        
    public:
        UnOp(std::string _o, Expr* _e, int cnt) : op(_o), e(_e)  {linecnt = cnt;};
        
        void printOn(std::ostream &out) const;
        void semantic();
        Value* compile(); 
};

class Id : public Expr {
    /* Id as an l-value */
    public:
        std::string name ;
        Id(std::string n, int cnt) : name(n) 
        {   linecnt = cnt;
            this->lvalue = true; 
        }
        
        void printOn(std::ostream &out) const;
        void semantic();
        Value* compile(); 
};

class Result : public Expr {
    public:
        Result(int cnt) {linecnt = cnt;};
        void printOn(std::ostream &out) const;
        void semantic();
        Value* compile();
};

class Const : public Expr {

    public:
        data_const val;     
        Const(data_const val, Stype t, int cnt) : val(val) , Expr(t) {linecnt = cnt;};       
        void printOn(std::ostream &out) const;
        void semantic();
        Value* compile(); 
};

class CallFunc : public Expr {
    private:
        std::string fname;
        ASTnodeCollection *parameters;
    public:
        CallFunc(std::string name, ASTnodeCollection* params, int cnt) : fname(name), parameters(params) {linecnt = cnt;}
        CallFunc(std::string name, int cnt) : fname(name) {linecnt = cnt; parameters=nullptr;}
        
        void printOn(std::ostream &out) const;
        void semantic();
        Value* compile(); 
};

class CallProc : public Stmt {
    private:
        std::string fname;
        ASTnodeCollection *parameters;
    public:
        CallProc(std::string name, ASTnodeCollection* params, int cnt ) : fname(name), parameters(params) {linecnt = cnt;}
        CallProc(std::string name, int cnt) : fname(name)  {linecnt = cnt; parameters=nullptr;}
        
        void printOn(std::ostream &out) const;
        void semantic();
        Value* compile(); 
};

class StringValue : public Expr {
    /*
        l-value, type : array[n] of char
        n = #characters + '\0'
        the only array-type constant
        so you can use it as str[i], but not to assign since it's a constant 
    */
    private:
        std::string strvalue; 
    public:
        StringValue(const char* s, int cnt) 
        {
            linecnt = cnt;
            size_t len = strlen(s);
            type = typeArray(len, typeChar);
            strvalue = s ;
            //this->lvalue = true;
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
        ArrayAccess(Expr* lval, Expr* pos, int cnt) : lval(lval) , pos(pos){linecnt = cnt;}
        
        void printOn(std::ostream &out) const;
        void semantic();
        Value* compile(); 
};

class Dereference : public Expr {
    private:
        Expr* e;
    public:
        Dereference(Expr* e, int cnt) : e(e) {linecnt = cnt;}
        
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
        ASTnodeCollection* locals;
        ASTnodeCollection* body;
    
    public:
        Block( ASTnodeCollection* theBody, int cnt) : body(theBody) 
        {
            linecnt = cnt;
            locals = new ASTnodeCollection();
        }

        ~Block()
        {
            delete(locals);
            delete(body);
        }

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
        Variable(std::string s, Stype t) : name(s), type(t) {};
    
        void printOn(std::ostream &out) const;
        void semantic();
        Value* compile(); 
};

class VarDef : public Stmt {
    /* Class containing variable definitions
    */ 
    private:
        std::list<Variable *> vars;   
    public:
        VarDef(int cnt) {linecnt = cnt;};
        void push(std::list<std::string>* var_ids, Stype t);
        void printOn(std::ostream &out) const;
        void semantic();
        Value* compile(); 
};

class LabelDef : public Stmt {
    private:
        std::list<std::string> labels;
    public:
        LabelDef(std::list<std::string>* names, int cnt) : labels(*names)  {linecnt = cnt;}
        
        void printOn(std::ostream &out) const;
        void semantic();
        Value* compile(); 
};

class FormalsGroup : public Stmt {
    public:
        std::list<std::string> formals;
        Stype type;
        PassMode pass_by;

        FormalsGroup(std::list<std::string>* f, Stype t, PassMode pm, int cnt) : formals(*f), type(t), pass_by(pm) {linecnt = cnt;}
        
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
        std::list<ParameterGroup*> parameters;
        Block* body;
        bool isForward;
        
    public:
        FunctionDef(std::string n, std::list<ParameterGroup*>* params, Stype t, int cnt) 
        : name(n), parameters(*params), type(t) { isForward = false; linecnt = cnt;}

        void set_forward();
        void add_body(Block* theBody);
        
        void printOn(std::ostream &out) const;
        void semantic();
        Value* compile(); 
};

class Assignment : public Stmt {
    private:
        Expr *lval,*rval ;
    public:
        Assignment(Expr* lval, Expr* rval, int cnt) : lval(lval) , rval(rval)  {linecnt = cnt;}
        
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
        IfThenElse(Expr* c, Stmt* t, int cnt) : cond(c), st_then(t) , hasElse(false) {linecnt = cnt;};
        IfThenElse(Expr* c, Stmt* t, Stmt* e, int cnt) : cond(c), st_then(t), st_else(e) , hasElse(true) {linecnt = cnt;};
        
        void printOn(std::ostream &out) const;
        void semantic();
        Value* compile(); 
};

class While : public Stmt {
    private:
        Expr *cond;
        Stmt *body;

    public:
        While(Expr* c, Stmt *b, int cnt) : cond(c), body(b) {linecnt = cnt;}
        
        void printOn(std::ostream &out) const;
        void semantic();
        Value* compile(); 
};

class Label : public Stmt {
/*
    Implements actual label encounter in code
*/
    private:
        std::string label;
        Stmt *target;
    public:
        Label(std::string name, Stmt* stmt, int cnt) : label(name), target(stmt) {linecnt = cnt;}
        
        void printOn(std::ostream &out) const;
        void semantic();
        Value* compile(); 
};

class GoTo : public Stmt {
    private:
        std::string label;
    public:
        GoTo(std::string lbl, int cnt) : label(lbl){linecnt = cnt;}
        
        void printOn(std::ostream &out) const;
        void semantic();
        Value* compile(); 
};

class ReturnStmt : public Stmt {
    public: 
        ReturnStmt(int cnt) {linecnt = cnt;};
        
        void printOn(std::ostream &out) const;
        void semantic();
        Value* compile(); 
};


// could be merged but we leave it as 2 classes for now.
class Init : public Stmt {
    private:
        Expr *lval;
    public:
        Init(Expr* lval, int cnt) : lval(lval) {linecnt = cnt;}
        
        void printOn(std::ostream &out) const;
        void semantic();
        Value* compile(); 
};

class InitArray : public Stmt {
    private:
        Expr *lval;
        Expr *size;
    public:
        InitArray(Expr* lval, Expr* sz, int cnt) : lval(lval), size(sz) {linecnt = cnt;}
        
        void printOn(std::ostream &out) const;
        void semantic();
        Value* compile(); 
};


class Dispose : public Stmt {
    private:
        Expr *lval;
    public:
        Dispose(Expr *lval, int cnt) : lval(lval){linecnt = cnt;} 
        
        void printOn(std::ostream &out) const;
        void semantic();
        Value* compile(); 
};

class DisposeArray : public Stmt {
    private:
        Expr *lval;
    public:
        DisposeArray(Expr *lval, int cnt) : lval(lval) {linecnt = cnt;} 
        
        void printOn(std::ostream &out) const;
        void semantic();
        Value* compile(); 
};

