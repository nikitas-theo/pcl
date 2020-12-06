#ifndef __AST_HPP__
#define __AST_HPP__

#include <stdlib.h> 
#include <string>
#include <deque>
#include <iostream>
#include "symbol/symbol.h"
#include "symbol/error.h"
#include "symbol_compatible.hpp"

using namespace std;

typedef deque<string> IdStack;

class AST {
    public:
        virtual ~AST() {}
        virtual void semantic() {}
        virtual void compile() {}
        
        //virtual void printOn(ostream &out) const = 0;
};
class Expr : public AST {
    protected:
        Type mytype;
    
    public:
        bool type_check(Type t)
        {
            return equalType(mytype, t);
        }

        bool type_check_strict(Type t)
        {
            if (this->type_check(t))
                return true;
            error("AAAAAAAAAAAAAAAAAAAAAA");
            return false;
        }
        
        void set_type(Type t)
        {
            mytype = t;
        }
        
        Type get_type()
        {
            return mytype;
        }
};

class Stmt : public AST {
};

class StatementStack : public deque<Stmt*>, public Stmt {
};

//typedef deque<Stmt*> StatementStack;
typedef deque<Expr*> ExpressionStack;

/* Expressions */

class BinOp : public Expr{
    private:
        Expr *left, *right;
        string op;

        bool is_arithmetic(Type t)
        {
            if (t->kind && (TYPE_INTEGER || TYPE_REAL))
                return true;
            error("AAAAAAAAAAAAAAAAAAAAA");
            return false;
        }
        
    public:
        BinOp(Expr* l, string o, Expr* r) : left(l), right(r), op(o) {}
        ~BinOp() { delete left; delete right; }
        
        void semantic() override
        {
            left->semantic();
            right->semantic();
            
            switch(op[0]) {
                case '+': case '-': case '*':
                    is_arithmetic(left->get_type());
                    is_arithmetic(right->get_type());
                    if (left->type_check(typeReal) || right->type_check(typeReal))
                        this->set_type(typeReal);
                    else
                        this->set_type(typeInteger);
                    break;
                case '/':
                    is_arithmetic(left->get_type());
                    is_arithmetic(right->get_type());
                    this->set_type(typeReal);
                    break;
                //case "div": case "mod":
                case 'd': case 'm':
                    left->type_check_strict(typeInteger);
                    right->type_check_strict(typeInteger);
                    this->set_type(typeInteger);
                    break;
                //case "and": case "or":
                case 'a': case 'o':
                    left->type_check_strict(typeBoolean);
                    right->type_check_strict(typeBoolean);
                    this->set_type(typeBoolean);
                    break;
                //case '=': case "<>":
                case '=':
                    //?????
                    this->set_type(typeBoolean);
                    break;
                case '>': case '<': //case "<=": case ">=":
                    is_arithmetic(left->get_type());
                    is_arithmetic(right->get_type());
                    this->set_type(typeBoolean);
                    break;
            }
        }
};

class UnOp : public Expr {
    private:
        Expr *e;
        string op;
        
    public:
        UnOp(string _o, Expr* _e) : op(_o), e(_e) {}
        ~UnOp() { delete e; }
        
        void semantic() override
        {
            e->semantic();
            
            switch(op[0]) {
                case '@':
                    //???
                    break;
                //case "not":
                case 'n':
                    if (e->type_check(typeBoolean)) this->set_type(typeBoolean);
                    else //error!
                    break;
                case '+': case '-':
                    if (e->type_check(typeInteger)) this->set_type(typeInteger);
                    else //error!
                    break;
            }
        }
};

class Id : public Expr {
    private:
    public:
        Id(string n) {}
};

class Const : public Expr {
    private:
    
    public:
        Const() {}
        
        void semantic() override
        {
            
        }
};

class FuncCall : public Expr {
    private:
        string fname;
        ExpressionStack* parameters;
        
    public:
        FuncCall(string name, ExpressionStack* params=nullptr) : fname(name), parameters(params) {}
};

class String : public Expr {
    private:
    public:
        String(string s) {}
};

class ArrayAccess : public Expr {
    private:
    public:
        ArrayAccess(Expr* lval, Expr* pos) {}
};

class Dereference : public Expr {
    private:
    public:
        Dereference(Expr* e) {}
};

/* Statements */

class Program : public Stmt {
    private:
        StatementStack *locals;
        StatementStack *body;
    
    public:
        Program(StatementStack* theBody) : body(theBody)
        {
            locals = new StatementStack();
        }
        ~Program() { delete locals; }

        void semantic() override
        {
            for (Stmt *l : *locals) {
                l->semantic();
            }
            for (Stmt *s : *body) {
                s->semantic();
            }
        }

        void push_local(Stmt *l)
        {
            locals->push_front(l);
        }
};

class Variable : public Stmt {
    private:
        string name;
        Type type;
    
    public:
        Variable(string n, Type t) : name(n), type(t) {}

        void semantic() override
        {
            newVariable(name, type);
        }
};

class VariableGroupStack : public Stmt {
    private:
        deque<Variable*> *vars;
    
    public:
        VariableGroupStack()
        {
            vars = new deque<Variable*>;
        }
        ~VariableGroupStack() { delete vars; }

        void semantic() override
        {
            for (Variable *v : *vars) {
                v->semantic();
            }
        }

        void push(IdStack* var_ids, Type t)
        {
            for (string s : *var_ids) {
                vars->push_front(new Variable(s, t));
            }
        }
};

class Label : public Stmt {
    private:
        IdStack *lblnames;
    public:
        Label(IdStack* names) : lblnames(names) {}

        void semantic() override
        {
            for (string lbl : *lblnames)
                newConstant(lbl, typeVoid);
        }
};

class FormalsGroup : public Stmt {
    private:
        IdStack *formals;
        Type type;
        PassMode pass_by;
        SymbolEntry *function_entry;
    
    public:
        FormalsGroup(IdStack* f, Type t, PassMode pm) : formals(f), type(t), pass_by(pm) {}

        void set_function_entry(SymbolEntry *f) { function_entry = f; }

        void semantic() override
        {
            for (string f : *formals) {
                newParameter(f, type, pass_by, function_entry);
            }
        }
    
};

typedef deque<FormalsGroup*> FormalsGroupStack;

class Routine : public Stmt {
    private:
        string name;
        Type type;
        FormalsGroupStack* parameters;
        Program* body;
        bool isForward;
        
    public:
        Routine(string n, FormalsGroupStack* params, Type t) : name(n), parameters(params), type(t) { isForward = false;}
        ~Routine() {}
        
        void semantic() override
        {
            SymbolEntry *p;
            
            p = newFunction(name);
            if (isForward) forwardFunction(p);
            
            openScope();
            
            for (FormalsGroup* fg : *parameters) {
                fg->set_function_entry(p);
                fg->semantic();
            }
            
            endFunctionHeader(p, typeVoid);

            if (!isForward) {
                body->semantic();
            }
            
            closeScope();
        }

        void set_forward()
        {
            this->isForward = true;
        }
        
        void add_body(Program* theBody)
        {
            this->body = theBody;
        }
};

class ProcCall : public Stmt {
    private:
        string pname;
        ExpressionStack *parameters;
    
    public:
        ProcCall(string pn) : pname(pn) { parameters = nullptr; }
        ProcCall(string pn, ExpressionStack* params) : pname(pn), parameters(params) {}

        void semantic() override
        {
            SymbolEntry *p = lookupEntry(pname, LOOKUP_ALL_SCOPES, true);

            if (p->entryType != ENTRY_FUNCTION) ;//error

            //?????
        }
};

class Declaration : public Stmt {
    private:
    public:
        Declaration(Expr* lval, Expr* val) {}
};

class IfThenElse : public Stmt {
    private:
        Expr *cond;
        Stmt *st_then, *st_else;
        
    public:
        IfThenElse(Expr* c, Stmt* t, Stmt* e=nullptr) : cond(c), st_then(t), st_else(e) {};
        ~IfThenElse() { delete cond; delete st_then; delete st_else; }
        
        void semantic() override
        {
            cond->semantic();
            cond->type_check(typeBoolean); //error handling??
            
            st_then->semantic();
            if (st_else != nullptr) st_else->semantic();
        }
            
};

class While : public Stmt {
    private:
        Expr *cond;
        Stmt *body;
        
    public:
        While(Expr* c, Stmt *b) : cond(c), body(b) {}
        ~While() { delete cond; delete body; }
        
        void semantic() override
        {
            cond->semantic();
            cond->type_check(typeBoolean); //error handling??
            
            body->semantic();
        }
};

class LabelBind : public Stmt {
    private:
        string lbl;
        Stmt *target;
    public:
        LabelBind(string name, Stmt* st) : lbl(name), target(st) {}
        ~LabelBind() {}
        
        void semantic() override
        {
            lookupEntry(lbl, LOOKUP_ALL_SCOPES, true);
        }
};

class GoTo : public Stmt {
    private:
        string label;
    
    public:
        GoTo(string lbl) : label(lbl) {}

        void semantic() override
        {
            lookupEntry(label, LOOKUP_ALL_SCOPES, true);
        }
};

class ReturnStmt : public Stmt {
};

class Init : public Stmt {
    private:
        string vname;
    public:
        Init(Expr* lval) {}
    
};

class InitArray : public Stmt {
    private:
    public:
        InitArray(Expr* lval, Expr* sz) {}
};

class Dispose : public Stmt {
    private:
        string name;
    
    public:
        Dispose(string n) : name(n) {}
        Dispose(Expr *lval) {} //????????????/

        void semantic() override
        {
            SymbolEntry *l = lookupEntry(name, LOOKUP_ALL_SCOPES, true);
            l->u.eVariable.type->kind == TYPE_IARRAY;
        }
};

class DisposeArray : public Stmt {
    private:
    public:
        DisposeArray(Expr* lval) {} //???????????
};

#endif
