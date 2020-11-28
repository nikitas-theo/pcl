#ifndef __AST_HPP__
#define __AST_HPP__

#include <stdlib.h> 
#include <string>
#include <stack>
#include <iostream>
#include <vector>
#include "symbol/symbol.h"

using namespace std;

typedef stack<string> IdStack;
typedef stack<FormalsGroup*> FormalsGroupStack;
typedef stack<Stmt*> StatementStack;
typedef stack<Expr*> ExpressionStack;

class AST {
    public:
        virtual ~AST() {}
        virtual void semantic() {}
        virtual void compile() {}
        
        virtual void printOn(ostream &out) const = 0;
};

inline std::operator<< (std::ostream &out, const AST &node)
{
    node.printOn(out);
    return out;
}

class Expr : public AST {
    protected:
        Type mytype;
    
    public:
        bool type_check(Type t)
        {
            if (!(mytype->kind && t->kind))
                return false;
            switch (mytype->kind) {
                case TYPE_ARRAY:
                    if (mytype->size != t->size)
                        return false;
                case TYPE_IARRAY:
                case TYPE_POINTER:
                    return type_check(mytype->refType, t->refType);
            }
            return true
        }

        bool type_check_strict(Type t)
        {
            if (this.type_check(t))
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
            
            switch(op) {
                case '+': case '-': case '*':
                    is_arithmetic(left->get_type());
                    is_arithmetic(right->get_type());
                    if (left->type_check(typeReal) || right->type_check(typeReal))
                        this->set_type(typeReal);
                    else
                        this->set_stype(typeInteger);
                    break;
                case '/':
                    is_arithmetic(left->get_type());
                    is_arithmetic(right->get_type());
                    this->set_type(typeReal);
                    break;
                case "div": case "mod":
                    left->type_check_strict(typeInteger);
                    right->type_check_strict(typeInteger);
                    this->set_type(typeInteger);
                    break;
                case "and": case "or":
                    left->type_check_strict(typeBoolean);
                    right->type_check_strict(typeBoolean);
                    this->set_type(typeBoolean);
                    break;
                case '=': case "<>":
                    //?????
                    this->set_type(typeBoolean);
                    break;
                case '>': case '<': case "<=": case ">=":
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
        UnOp(string _o, Expr _e) : op(_o), e(_e) {}
        ~UnOp() { delete e; }
        
        void semantic override
        {
            e->semantic();
            
            switch(op) {
                case '@':
                    //???
                    break;
                case "not":
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

class Const : public Expr {
    private:
        string name;
        Type ctype;
        object value;
    
    public:
        Const(string nm, Type t, object val) : name(nm), ctype(t), value(val) {}
        
        void semantic() override
        {
            newConstant(name, ctype, value);
        }
};

class FuncCall : public Expr {
};

class String : public Expr {
};

class ArrayAccess : public Expr {
};

class Dereference : public Expr {
};

/* Statements */

class Program : public Stmt {
    private:
        stack<Stmt*> *locals;
        stack<Stmt*> body;
    
    public:
        Program(stack<Stmt> theBody) : body(theBody)
        {
            locals = new stack<Stmt*>;
        }
        ~Program() { delete locals; }

        void semantic() override
        {
            for (Stmt *l in locals) {
                l->semantic();
            }
            for (Stmt *s in body) {
                s->semantic();
            }
        }

        void push_local(Stmt *l)
        {
            locals->push(l);
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
        stack<Variable*> *vars;
    
    public:
        VariableGroupStack()
        {
            vars = new stack<Variable*>;
        }
        ~VariableGroupStack() { delete vars; }

        void semantic() override
        {
            for (Variable *v in vars) {
                v->semantic();
            }
        }

        void push(IdStack* var_ids, Type t)
        {
            for (string s in var_ids) {
                vars->push(new Variable(s, t));
            }
        }
};

class Label : public Stmt {
    private:
        string lblname;
    public:
        Label(string name) : lblname(name) {}

        void semantic() override
        {
            newConstant(name, typeVoid);
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

        void set_function_entry(SymbolEntry *f) : function_entry(f) {}

        void semantic() override
        {
            for (string f in formals) {
                newParameter(f, type, pass_by, function_entry);
            }
        }
    
};

class Routine : public Stmt {
    private:
        string name;
        Type type;
        FormalsGroupStack parameters;
        StatementStack body;
        bool isForward;
        
    public:
        Routine(string n, stack<Stmt*> params, Type t) : name(n), parameters(params), type(t) { isForward = false;}
        ~Routine() {}
        
        void semantic() override
        {
            SymbolEntry *p;
            
            p = newFunction(name);
            if (isForward) forwardFunction(p);
            
            openScope();
            
            for (FormalsGroup* fg in parameters) {
                fg->set_function_entry(p);
                fg->semantic();
            }
            
            endFunctionHeader(p, typeVoid);

            if (!isForward) {
                for (Stmt* s in body) {
                    s->semantic();
                }
            }
            
            closeScope();
        }

        void set_forward()
        {
            this.isForward = true;
        }
};

class ProcCall : public Stmt {
    private:
        string pname;
        ExpressionStack *parameters;
    
    public:
        ProcCall(string pn) : panme(pn) { parameters = nullptr; }
        ProcCall(string pn, ExpressionStack* params) : pname(pn), parameters(params) {}

        void semantic() override
        {
            SymbolEntry *p = lookupEntry(pname, LOOKUP_ALL_SCOPES, true);

            if (p->entryType != ENTRY_FUNCTION) ;//error

            //?????
        }
};

class Declaration : public Stmt {
};

class IfThenElse : public Stmt {
    private:
        Expr *cond;
        Stmt *st_then, *st_else;
        
    public:
        IfThenElse(Expr* c, Stmt* t, Stmt* e=nullptr) : cond(c), st_then(t), st_else(e) {};
        ~IfThenElse() { delete cond; delete st_then; delete st_else; }
        
        void semantic override
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
            lookupEntry(name, LOOKUP_ALL_SCOPES, true);
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
    
};

class InitArray : public Stmt {
};

class Dispose : public Stmt {
    private:
        string name;
    
    public:
        Dispose(string n) : name(n) {}

        void semantic() override
        {
            SymbolEntry *l = lookupEntry(name, LOOKUP_ALL_SCOPES, true;)
            l->eVariable.type == typeIArray;
        }
};

class DisposeArray : public Stmt {
};

#endif
