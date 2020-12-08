#pragma once 
#include <stdlib.h> 
#include <string>
#include <vector>
#include <iostream>
#include <variant> 
#include "symbol/symbol.h"
#include "symbol/error.h"
#include "symbol_compatible.hpp"


using namespace std;



class AST {
    public:
        virtual ~AST() {}
        virtual void semantic() {}
        virtual void compile() {}
        virtual void printOn(ostream &out) const = 0;
};

// Operator << on AST
inline std::ostream& operator<<(std::ostream &out, const AST &t) {
  t.printOn(out);
  return out;
}


class Expr : public AST {
    protected:
        Type mytype;
    public:
        virtual void printOn(std::ostream &out) const = 0;
};
class Stmt : public AST {
    public : 
        virtual void printOn(std::ostream &out) const = 0;
};

class StatementStack : public Stmt {
    public : 
    vector<Stmt*> list ; 
    void printOn(std::ostream &out) const {
        out << "[" ;for (auto it = list.rbegin(); it != list.rend(); ++it) { (*it)->printOn(out) ; 
        if (it + 1 != list.rend()) out << ", "; } out << "]"; } 
    StatementStack(Stmt* s){list.push_back(s);}
    StatementStack() {}
    void push(Stmt* x) {list.push_back(x);}

};
class ExpressionStack : public Expr {
    public : 
    vector<Expr*> list ; 
    void printOn(std::ostream &out) const {
        out << "[" ;for (auto it = list.rbegin(); it != list.rend(); ++it) { (*it)->printOn(out) ; 
        if (it + 1 != list.rend()) out << ", "; } out << "]"; } 
    ExpressionStack(Expr* s){list.push_back(s);}
    ExpressionStack() {}
    void push(Expr* x) {list.push_back(x);}

};
/* Expressions */
constexpr unsigned int hashf(const char *s, int off = 0) {                        
    return !s[off] ? 5381 : (hashf(s, off+1)*33) ^ s[off];                           
}    
constexpr inline unsigned int operator "" _(char const * p, size_t) { return hashf(p); }

class BinOp : public Expr{
    private:
        Expr *left, *right;
        string op;
        
    public:
        BinOp(Expr* l, string o, Expr* r) : left(l), right(r), op(o) {}
        ~BinOp() { delete left; delete right; }
        void semantic() override{}
        void printOn(std::ostream &out) const {
            left->printOn(out);
            out << " " << op <<" ";
            right->printOn(out); 
        }
};

class UnOp : public Expr {
    private:
        Expr *e;
        string op;
        
    public:
        UnOp(string _o, Expr* _e) : op(_o), e(_e) {}
        ~UnOp() { delete e; }
        
        void semantic() override{}
        void printOn(std::ostream &out) const {out << "(" << op ;
        e->printOn(out); 
        out << ")";}
};

class Id : public Expr {
    private:
        string name ;
    public:
        Id(string n) : name(n) {}
        void printOn(std::ostream &out) const {out << "Id(" << name <<  ")";}

};

class IdStack : public Expr {
    public : 
    vector<string> list ; 
    void printOn(std::ostream &out) const {
        out << "[" ;for (auto it = list.rbegin(); it != list.rend(); ++it){ out << *it ; 
        if (it + 1 != list.rend()) out << ", "; }out << "]"; } 
    IdStack(string s){list.push_back(s);}
    IdStack() {}
    void push(string x) {list.push_back(x);}

};

typedef variant<int,double,char,bool> data_const ;
// https://en.cppreference.com/w/cpp/utility/variant/holds_alternative
// https://en.cppreference.com/w/cpp/utility/variant     

class Const : public Expr {

    private:
        Type type;
        data_const val;     
    public:
        Const(data_const val, Type t) : val(val) , type(t) {}      
        void printOn(std::ostream &out) const {
            switch(type->kind) {
                case TYPE_INTEGER : out << get<int>(val) ; break;
                case TYPE_BOOLEAN : out << get<bool>(val) ; break;
                case TYPE_CHAR : out << get<char>(val) ; break;
                case TYPE_REAL : out << get<double>(val) ; break;
                case TYPE_VOID : out << "Nil" ; break; 

            } 
        }
};




class FuncCall : public Expr {
    private:
        string fname;
        ExpressionStack* parameters;
        
    public:
        FuncCall(string name, ExpressionStack* params=nullptr) : fname(name), parameters(params) {}
        void printOn(std::ostream &out) const {
            out << "FuncCall(" << fname; 
            if (parameters != nullptr) parameters->printOn(out) ; out << ")";}
};

class String : public Expr {
    private:
        string s; 
    public:
        String(string s) : s(s){}
        void printOn(std::ostream &out) const {out << '\"' << s << '\"';}
};

class ArrayAccess : public Expr {
    private:
        Expr* lval; 
        Expr* pos; 
    public:
        ArrayAccess(Expr* lval, Expr* pos) : lval(lval) , pos(pos){}
        void printOn(std::ostream &out) const { lval->printOn(out); out << "[";  pos->printOn(out); out << "]";}
};

class Dereference : public Expr {
    private:
        Expr* e;
    public:
        Dereference(Expr* e) : e(e) {}
        void printOn(std::ostream &out) const { e->printOn(out) ; cout << "^";}
};

/* Statements */

class Block : public Stmt {
    private:
        StatementStack *locals;
        StatementStack *body;
    
    public:
        Block(StatementStack* theBody) : body(theBody)
        {
            locals = new StatementStack();
        }
        ~Block() { delete locals; }

        void semantic() override {}

        void push_local(Stmt *l)
        {
            locals->push(l);
        }
        void printOn(std::ostream &out) const {
            out << "Block of: "  << endl; 
            out << "\t\tlocals: " ; locals->printOn(out); out << endl; 
            out << "\t\tbody: " ;body->printOn(out); out << endl; 
        }
};



class Variable : public Stmt {
    private:
        string name;
        Type  type;
    
    public:
        Variable(string n, Type t) : name(n), type(t) {}

        void printOn(std::ostream &out) const {
            out << "var " << name << " :" << type ;  
        }
};

class VariableGroupStack : public Stmt {
    private:
        vector<Variable*> *vars;
    
    public:
        VariableGroupStack()
        {
            vars = new vector<Variable*>;
        }
        ~VariableGroupStack() { delete vars; }


        void push(IdStack* var_ids, Type t)
        {
            for (string s : var_ids->list ) {
                vars->push_back(new Variable(s, t));
            }
        }
        void printOn(std::ostream &out) const {}
};

class Label : public Stmt {
    private:
        IdStack *lblnames;
    public:
        Label(IdStack* names) : lblnames(names) {}
        void printOn(std::ostream &out) const {}
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
        void printOn(std::ostream &out) const { 
            if (pass_by == PASS_BY_REFERENCE) out << "var: ";
            // find a way to handle type printing , operator overloading doesnt want work 
            formals->printOn(out) ; out <<  " : " ; out << type; 
        }
    
};
class FormalsGroupStack : public Stmt {
    public : 
    vector<FormalsGroup*> list ; 
    void printOn(std::ostream &out) const {
        out << "[" ;for (auto it = list.rbegin(); it != list.rend(); ++it) { (*it)->printOn(out) ; 
        if (it + 1 != list.rend()) out << ", "; } out << "]"; } 
    FormalsGroupStack(FormalsGroup* s){list.push_back(s);}
    FormalsGroupStack() {}
    void push(FormalsGroup* x) {list.push_back(x);}

};


class Routine : public Stmt {
    private:
        string name;
        Type type;
        FormalsGroupStack* parameters;
        Block* body;
        bool isForward;
        
    public:
        Routine(string n, FormalsGroupStack* params, Type t) : name(n), parameters(params), type(t) { isForward = false;}
        ~Routine() {}
        
        void printOn(std::ostream &out) const { 
            out << "["<< name << "]("; 
            parameters->printOn(out);
            out << ")" << " : " << type <<endl ;
            body->printOn(out);
        }
        void set_forward()
        {
            this->isForward = true;
        }
        
        void add_body(Block* theBody)
        {
            this->body = theBody;
        }
};

class ProcCall : public Stmt {
    private:
        string pname;
        ExpressionStack *parameters;
    
    public:
        ProcCall(string pn) : pname(pn) { parameters = new ExpressionStack(); }
        ProcCall(string pn, ExpressionStack* params) : pname(pn), parameters(params) {}
        void printOn(std::ostream &out) const {out << "ProcCall[" << pname << "] ( "; parameters->printOn(out); out << " )" ;}
};

class Declaration : public Stmt {
    private:
        Expr *lval,*val ;
    public:
        Declaration(Expr* lval, Expr* val) : lval(lval) , val(val) {}
        void printOn(std::ostream &out) const {
            lval->printOn(out);
            out << " := " ; 
            val->printOn(out);
        }
};

class IfThenElse : public Stmt {
    private:
        Expr *cond;
        Stmt *st_then, *st_else;
        
    public:
        IfThenElse(Expr* c, Stmt* t, Stmt* e=nullptr) : cond(c), st_then(t), st_else(e) {};
        ~IfThenElse() { delete cond; delete st_then; delete st_else; }
        void printOn(std::ostream &out) const { 
            out << "If("; cond->printOn(out); out << "then" << endl;
            st_then->printOn(out); out << endl ;
            if (st_else != nullptr) { out  << "else : " << endl;  st_else->printOn(out); }

        }         
            
};

class While : public Stmt {
    private:
        Expr *cond;
        Stmt *body;
        
    public:
        While(Expr* c, Stmt *b) : cond(c), body(b) {}
        ~While() { delete cond; delete body; }
        void printOn(std::ostream &out) const {
            out << "While("; cond->printOn(out); out << ") do" << endl;
            body->printOn(out);
        }        
};

class LabelBind : public Stmt {
    private:
        string lbl;
        Stmt *target;
    public:
        LabelBind(string name, Stmt* st) : lbl(name), target(st) {}
        ~LabelBind() {}
        void printOn(std::ostream &out) const { out <<  lbl << " : "; target->printOn(out);  }
};

class GoTo : public Stmt {
    private:
        string label;
    
    public:
        GoTo(string lbl) : label(lbl) {}
        void printOn(std::ostream &out) const {out << "LABEL : " << label;}

};

class ReturnStmt : public Stmt {
    public: 
        ReturnStmt(){};
        void printOn(std::ostream &out) const {out << "RET" ; }
};

class Init : public Stmt {
    private:
        Expr *lval;
    public:
        Init(Expr* lval) {}
        void printOn(std::ostream &out) const {out << "Init"; lval->printOn(out); }
};

class InitArray : public Stmt {
    private:
        Expr *lval;
    public:
        InitArray(Expr* lval, Expr* sz) {}
        void printOn(std::ostream &out) const {out << "InitArr"; lval->printOn(out); }
};

class Dispose : public Stmt {
    private:
        Expr *lval;
            
    public:
        Dispose(Expr *lval) : lval(lval) {} 
        void printOn(std::ostream &out) const {out << "Destroy"; lval->printOn(out); }
        
};

class DisposeArray : public Stmt {
    private:
        Expr* lval ;
    public:
        DisposeArray(Expr* lval) {} 
        void printOn(std::ostream &out) const {out << "DestroyArr " ; lval->printOn(out) ;}
};
