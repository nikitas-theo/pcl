#pragma once
#include <stdlib.h> 
#include <string>
#include <iostream>
#include <vector>
#include "symbol/symbol.h"

using namespace std; 
typedef int NIL ;
class AST {
public:
    virtual ~AST() { }
    virtual void printOn(ostream &out) const = 0;
    virtual void sem() {}
};
// Operator << on AST
inline std::ostream& operator<<(std::ostream &out, const AST &t) {
  t.printOn(out);
  return out;
}

class Expr: public AST {
public : 
virtual void eval(){};
};
class Stmt: public AST {
public : 
virtual void run(){};
};

class ExprList : public Expr{
public:
    vector<Expr*> list ; 
    ExprList(Expr* e) { append(e);}
    void append(Expr* e){
        list.push_back(e);
    }
    void printOn(std::ostream &out) const { 
        for (Expr* s : list){
            out << " " << *s;
        }
    }

};
class StmtList : public Stmt{
public:
    vector<Stmt*> list ; 
    StmtList(Stmt* e) { append(e);};
    StmtList() {};

    void append(Stmt* e){
        list.push_back(e);
    }
    void printOn(std::ostream &out) const { 
        for (Stmt* s : list){
            out << " " << *s;
        }
    }

};


/* 
   virtual keyword ensures runtime polymorphishm:
   if a base class function is virtual, then when i call Base->fun()
   and base points to a derived class Der, then Der.fun() will be called. 
*/
class Empty : public Stmt{
public:
    Empty(){};
    void printOn(std::ostream &out) const { out << "Empty";}

};

class VarDef : public Stmt{
public : 
    VarDef(StmtList* s, Type t ){
        append(s,t);      
    }
    vector<StmtList*> vars;
    vector<Type> types;
    void append(StmtList* s, Type t){
        vars.push_back(s);
        types.push_back(t);
    }
    void printOn(std::ostream &out) const { out << "VarDef";}

};


enum localof {var_def,label_def,func_def,forward_def};
class Local : public Stmt{
public: 
    localof def;
    Stmt* header ;
    StmtList* list;
    Local(localof t, StmtList* list,Stmt* header) : def(t) , header(header), list(list) {};
    Local(localof t, Stmt* header) : def(t) , header(header){};
    Local(localof t,StmtList* list) : def(t) , list(list){};
    void printOn(std::ostream &out) const { out << "Local";}


};


class Body : public Stmt {
public:
    StmtList* locals ; 
    Stmt* block;
    Body(Stmt* block) : block(block){};
    void append(Stmt* s){
        locals->append(s);
    }
    void printOn(std::ostream &out) const { 
        out << "Body(";  
        if (locals != NULL) out << "locals :" << *locals; 
        if (block != NULL) out << "block :" << *block << endl ;
        out << ")";
        }
    

};

enum pass {byvalue,byreference} ;
class Formal : public Stmt{
public:
    pass p; 
    StmtList* list;
    Type t; 
    Formal(pass p, StmtList* list, Type t): p(p) ,list(list), t(t){};
    void printOn(std::ostream &out) const { out << "Formal";}

};
class Decl : public Stmt{
public:
    Expr* var, *value; 
    Decl(Expr* l_value, Expr* value ) : var(l_value), value(value){};
    void printOn(std::ostream &out) const { out << "Decl";}

};
class IfThenElse : public Stmt{
public:
    Expr *cond;
    Stmt *then, *else_do; 
    IfThenElse(Expr* cond, Stmt* then) : cond(cond), then(then) {};
    IfThenElse(Expr* cond, Stmt* then, Stmt* else_do) : cond(cond), then(then), else_do(else_do){};
    void printOn(std::ostream &out) const { out << "If";}

};

class While : public Stmt{
public:
    Expr* cond; 
    Stmt* action;
    While(Expr* cond, Stmt* action) : cond(cond), action(action){};
    void printOn(std::ostream &out) const { out << "While";}

};
class Label : public Stmt{
public:
    Stmt* cmd; 
    string label;
    Label(string label, Stmt* cmd) : cmd(cmd), label(label) {};
    void printOn(std::ostream &out) const { out << "Label";}

};
class GoTo : public Stmt{
public:
    string label; 
    GoTo(string label) : label(label) {};
    void printOn(std::ostream &out) const { out << "GoTo";}

};
class ReturnStmt : public Stmt{
public:
    ReturnStmt(){};
    void printOn(std::ostream &out) const { out << "Return";}

};
class Init : public Stmt{
public:
    Expr *l_value, *size;
    Init(Expr* l_value) : l_value(l_value) {};
    Init(Expr* l_value,Expr* size) : l_value(l_value)  , size(size) {};
    void printOn(std::ostream &out) const { out << "Init";}

};

class Dispose : public Stmt{
public:
    Expr* l_value;
    Dispose(Expr* l_value) : l_value(l_value) {};
    void printOn(std::ostream &out) const { out << "Dispose";}

};

enum typeHead {function,procedure};
class Header : public Stmt{
public:
    typeHead call;
    string name ;
    StmtList* params;
    Type t; 
    Header(typeHead k,string name, StmtList* params, Type t) :call(k), name(name), params(params), t(t) {};
    Header(typeHead k,string name,StmtList* params) :call(k), name(name), params(params) {};
    void printOn(std::ostream &out) const { out << "Header";}

};

/* ----------------------- Expr ----------------------------- */




class UniOp : public Expr{
public : 
    string op ;
    Expr* e; 
    void printOn(std::ostream &out) const { 
        out << "Op " << op <<  *e;}
    UniOp(string c , Expr* e): op(c),e(e) {};
};
class BinOp : public Expr{
public : 
    string op; 
    Expr *left, *right; 
    BinOp(Expr* e1, string c , Expr* e2) : op(c), left(e1), right(e2){};
    void printOn(std::ostream &out) const { out << *left << op <<  *right;}
};
template <class T>
class Const : public Expr{
public: 
    T value ; 
    Const(T e) : value(e){};
    void printOn(std::ostream &out) const { out << "Const(" << value << " )";}
};
class Call : public Expr, public Stmt{
    public : 
        string name ;
        ExprList* params ;
        Call(string id) : name(id),  params() {}; 
        Call(string id, ExprList* list) : name(id), params(list){}; 
        void printOn(std::ostream &out) const { 
            out << "Call " << "name(" ;
            if (params != NULL) out << *params;
            out << ")";
            }
        void eval(){};
        void run(){};
};

class Id : public Expr, public Stmt {
    public:
        string name; 
        Id(string name) : name(name){};
        void printOn(std::ostream &out) const { out << "Id(" << name << " )";}
        void eval(){};
};
class String : public Expr{
    public:
        string value; 
        String(string value) : value(value){};
        void printOn(std::ostream &out) const { out << "String(" << value << ")";}
};
class Access : public Expr{
    public:
        Expr* l_value, *index; 

        Access(Expr* l_value, Expr* index) : l_value(l_value),index(index){};
        void printOn(std::ostream &out) const { out << "Access(" << l_value << " at " << index << " )";}
};
class Dereference : public Expr{
    public:
        Expr* pointer ;
        Dereference(Expr* exp) : pointer(exp){};
        void printOn(std::ostream &out) const { out << "Pointer( " << pointer << " )";}
};

