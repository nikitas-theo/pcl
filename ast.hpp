#pragma once 
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <variant> 
#include "symbol/symbol.h"
#include "symbol/error.h"
#include "symbol_compatible.hpp"

#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>


using namespace llvm;
/*
    *   How to handle variables ? 
        locals are moleded by alloca, this seems ot be adequate, 
        see llvm tutorial pdf  
    
    *   Do I need to define a global variable for each \t, ... etc? 
        like pap did for \n == NL ? 
        need to check writeString library by Achileas


*/


class AST {
    public:
        virtual ~AST() {}
        virtual void semantic() {}
        virtual void printOn(std::ostream &out) const = 0;
        void compile_llvm(){
            // Initialize 
            TheModule = std::make_unique<Module>("add program name here?",TheContext);
            i8 = IntegerType::get(TheContext,8);
            i64 = IntegerType::get(TheContext,8);

            FunctionType *WriteInteger_type = FunctionType::get(Type::getVoidTy(TheContext), {i64}, false );
            WriteInteger = Function::Create(WriteInteger_type, Function::ExternalLinkage, "writeInteger", TheModule.get());
        }
    protected:

        static LLVMContext TheContext; 
        static IRBuilder<>  Builder; 
        static std::unique_ptr<Module> TheModule;
        
        static ConstantInt* c32(int n){ return ConstantInt::get(TheContext,APInt(32,n,true)); }
        static ConstantInt* c8(char c){ return ConstantInt::get(TheContext,APInt(8,c,false)); }
        static ConstantInt* c1(int n){ return ConstantInt::get(TheContext,APInt(1,n,false)); }
        // should this be float , not sure
        static ConstantFP* c_real(double d) {return ConstantFP::get(TheContext,APFloat(d));}

        static Type *i8; 
        static Type *i64; 

        // need to do this for all pre-defined libraries ? 
        static Function *WriteInteger;

}   
;

// Operator << on AST
inline std::ostream& operator<<(std::ostream &out, const AST &t) {
  t.printOn(out);
  return out;
}


class Expr : public AST {
    protected:
        SymType mytype;
    public:
        virtual void printOn(std::ostream &out) const = 0;
        virtual Value* compile() const = 0;
};
class Stmt : public AST {
    public : 
        virtual void printOn(std::ostream &out) const = 0;
        virtual Value* compile() const = 0;
};

class StatementStack : public Stmt {
    public : 
    std::vector<Stmt*> list ; 
    void printOn(std::ostream &out) const {
        out << "[" ;for (auto it = list.rbegin(); it != list.rend(); ++it) { (*it)->printOn(out) ; 
        if (it + 1 != list.rend()) out << ", "; } out << "]"; } 
    StatementStack(Stmt* s){list.push_back(s);}
    StatementStack() {}
    void push(Stmt* x) {list.push_back(x);}
    
    Value* compile() const
    {   // do I need to reverse compile here ? 
        for (auto it = list.rbegin(); it != list.rend(); ++it) (*it)->compile();
        return nullptr ;
    }
};
class ExpressionStack : public Expr {
    public : 
    std::vector<Expr*> list ; 
    void printOn(std::ostream &out) const {
        out << "[" ;for (auto it = list.rbegin(); it != list.rend(); ++it) { (*it)->printOn(out) ; 
        if (it + 1 != list.rend()) out << ", "; } out << "]"; } 
    ExpressionStack(Expr* s){list.push_back(s);}
    ExpressionStack() {}
    void push(Expr* x) {list.push_back(x);}
    Value* compile() const{
        // do I just compile here and return nullptr since this is an Expr. stack ? 
        for (auto it = list.rbegin(); it != list.rend(); ++it) (*it)->compile();
        return nullptr; 
    }
};
/* Expressions */
constexpr unsigned int hashf(const char *s, int off = 0) {                        
    return !s[off] ? 5381 : (hashf(s, off+1)*33) ^ s[off];                           
}    
constexpr inline unsigned int operator "" _(char const * p, size_t) { return hashf(p); }

class BinOp : public Expr{
    private:
        Expr *left, *right;
        std::string op;
        
    public:
        BinOp(Expr* l, std::string o, Expr* r) : left(l), right(r), op(o) {}
        ~BinOp() { delete left; delete right; }
        void semantic() override{}
        void printOn(std::ostream &out) const {
            left->printOn(out);
            out << " " << op <<" ";
            right->printOn(out); 
        }
        Value* compile() const {
            Value *l = left->compile();
            Value *r = right->compile();
            switch(hashf(op.c_str())){
                case "+"_ : return Builder.CreateAdd(l,r,"addtmp");
                case "-"_ : return Builder.CreateSub(l,r,"subtmp");
                case "*"_ : return Builder.CreateMul(l,r,"multmp");
                case "div"_ : return Builder.CreateSDiv(l,r,"multmp");
                case "mod"_ : return Builder.CreateSRem(l,r,"multmp");
                // fix these from llvm doc 
                case "/"_ : return Builder.CreateMul(l,r,"multmp");
                case "and"_ : return Builder.CreateMul(l,r,"multmp");
                case "="_ : return Builder.CreateMul(l,r,"multmp");
                case "<>"_ : return Builder.CreateMul(l,r,"multmp");
                case ">"_ : return Builder.CreateMul(l,r,"multmp");
                case "<"_ : return Builder.CreateMul(l,r,"multmp");
                case "<="_ : return Builder.CreateMul(l,r,"multmp");
                case ">="_ : return Builder.CreateMul(l,r,"multmp");

            }
            return nullptr; 
        }
};

class UnOp : public Expr {
    private:
        Expr *e;
        std::string op;
        
    public:
        UnOp(std::string _o, Expr* _e) : op(_o), e(_e) {}
        ~UnOp() { delete e; }
        
        void semantic() override{}
        void printOn(std::ostream &out) const {out << "(" << op ;

        e->printOn(out); 
        out << ")";}
        Value* compile() const {return nullptr;}
};

class Id : public Expr {
    private:
        std::string name ;
    public:
        Id(std::string n) : name(n) {}
        void printOn(std::ostream &out) const {out << "Id(" << name <<  ")";}
        Value* compile() const {return nullptr; }

};

class IdStack : public Expr {
    public : 
    std::vector<std::string> list ; 
    void printOn(std::ostream &out) const {
        out << "[" ;for (auto it = list.rbegin(); it != list.rend(); ++it){ out << *it ; 
        if (it + 1 != list.rend()) out << ", "; }out << "]"; } 
    IdStack(std::string s){list.push_back(s);}
    IdStack() {}
    Value* compile() const {return nullptr; }
    void push(std::string x) {list.push_back(x);}

};

typedef std::variant<int,double,char,bool> data_const ;
// https://en.cppreference.com/w/cpp/utility/variant/holds_alternative
// https://en.cppreference.com/w/cpp/utility/variant     

class Const : public Expr {

    private:
        SymType type;
        data_const val;     
    public:
        Const(data_const val, SymType t) : val(val) , type(t) {}      
        void printOn(std::ostream &out) const {
            switch(type->kind) {
                case TYPE_INTEGER : out << std::get<int>(val) ; break;
                case TYPE_BOOLEAN : out << std::get<bool>(val) ; break;
                case TYPE_CHAR : out << std::get<char>(val) ; break;
                case TYPE_REAL : out << std::get<double>(val) ; break;
                case TYPE_VOID : out << "Nil" ; break; 

            } 
        }
        Value* compile() const{
            switch(type->kind) {
            case TYPE_INTEGER : return c32(std::get<int>(val));
            case TYPE_BOOLEAN : return c1(std::get<bool>(val));
            case TYPE_CHAR : return c8(std::get<char>(val));
            case TYPE_REAL : return c_real(std::get<double>(val));;
            case TYPE_VOID : return nullptr;  

            }
            return nullptr;  
        }
};




class FuncCall : public Expr {
    private:
        std::string fname;
        ExpressionStack* parameters;
        
    public:
        FuncCall(std::string name, ExpressionStack* params=nullptr) : fname(name), parameters(params) {}
        void printOn(std::ostream &out) const {
            out << "FuncCall(" << fname; 
            if (parameters != nullptr) parameters->printOn(out) ; 
            out << ")";}
        Value* compile() const {return nullptr;}

};

class String : public Expr {
    private:
        std::string s; 
    public:
        String(std::string s) : s(s){}
        void printOn(std::ostream &out) const {out << '\"' << s << '\"';}
        Value* compile() const {return nullptr;}

};

class ArrayAccess : public Expr {
    private:
        Expr* lval; 
        Expr* pos; 
    public:
        ArrayAccess(Expr* lval, Expr* pos) : lval(lval) , pos(pos){}
        void printOn(std::ostream &out) const { lval->printOn(out); out << "[";  pos->printOn(out); out << "]";}
        Value* compile() const {return nullptr;}

};

class Dereference : public Expr {
    private:
        Expr* e;
    public:
        Dereference(Expr* e) : e(e) {}
        void printOn(std::ostream &out) const { e->printOn(out) ; out << "^";}
        Value* compile() const {return nullptr; }

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
            out << "Block of: "  << std::endl; 
            out << "\t\tlocals: " ; locals->printOn(out); out << std::endl; 
            out << "\t\tbody: " ;body->printOn(out); out << std::endl; 
        }
        Value* compile() const {return nullptr;}

};



class Variable : public Stmt {
    private:
        std::string name;
        SymType  type;
    
    public:
        Variable(std::string n, SymType t) : name(n), type(t) {}
        Value* compile() const {return nullptr; }
        void printOn(std::ostream &out) const {
            out << "var " << name << " :" << type ;  
        }

};

class VariableGroupStack : public Stmt {
    private:
        std::vector<Variable*> *vars;
    
    public:
        VariableGroupStack()
        {
            vars = new std::vector<Variable*>;
        }
        ~VariableGroupStack() { delete vars; }


        void push(IdStack* var_ids, SymType t)
        {
            for (std::string s : var_ids->list ) {
                vars->push_back(new Variable(s, t));
            }
        }
        void printOn(std::ostream &out) const {}
        Value* compile() const {return nullptr;}

};

class Label : public Stmt {
    private:
        IdStack *lblnames;
    public:
        Label(IdStack* names) : lblnames(names) {}
        void printOn(std::ostream &out) const {}
        Value* compile() const {return nullptr;}

};

class FormalsGroup : public Stmt {
    private:
        IdStack *formals;
        SymType type;
        PassMode pass_by;
        SymbolEntry *function_entry;
    
    public:
        FormalsGroup(IdStack* f, SymType t, PassMode pm) : formals(f), type(t), pass_by(pm) {}

        void set_function_entry(SymbolEntry *f) { function_entry = f; }
        void printOn(std::ostream &out) const { 
            if (pass_by == PASS_BY_REFERENCE) out << "var: ";
            // find a way to handle type printing , operator overloading doesnt want work 
            formals->printOn(out) ; out <<  " : " ; out << type; 
        }
        Value* compile() const {return nullptr;}

};
class FormalsGroupStack : public Stmt {
    public : 
    std::vector<FormalsGroup*> list ; 
    void printOn(std::ostream &out) const {
        out << "[" ;for (auto it = list.rbegin(); it != list.rend(); ++it) { (*it)->printOn(out) ; 
        if (it + 1 != list.rend()) out << ", "; } out << "]"; } 
    FormalsGroupStack(FormalsGroup* s){list.push_back(s);}
    FormalsGroupStack() {}
    void push(FormalsGroup* x) {list.push_back(x);}
    Value* compile() const {return nullptr;}

};


class Routine : public Stmt {
    private:
        std::string name;
        SymType type;
        FormalsGroupStack* parameters;
        Block* body;
        bool isForward;
        
    public:
        Routine(std::string n, FormalsGroupStack* params, SymType t) : name(n), parameters(params), type(t) { isForward = false;}
        ~Routine() {}
        
        void printOn(std::ostream &out) const { 
            out << "["<< name << "]("; 
            parameters->printOn(out);
            out << ")" << " : " << type <<std::endl ;
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
        Value* compile() const {return nullptr;}

};

class ProcCall : public Stmt {
    private:
        std::string pname;
        ExpressionStack *parameters;
    
    public:
        ProcCall(std::string pn) : pname(pn) { parameters = new ExpressionStack(); }
        ProcCall(std::string pn, ExpressionStack* params) : pname(pn), parameters(params) {}
        void printOn(std::ostream &out) const {out << "ProcCall[" << pname << "] ( "; parameters->printOn(out); out << " )" ;}
        Value* compile() const {return nullptr;}

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
        Value* compile() const {return nullptr;}

};

class IfThenElse : public Stmt {
    private:
        Expr *cond;
        Stmt *st_then, *st_else = nullptr;
        bool hasElse; 
        
    public:
        IfThenElse(Expr* c, Stmt* t) : cond(c), st_then(t) , hasElse(false) {};
        IfThenElse(Expr* c, Stmt* t, Stmt* e) : cond(c), st_then(t), st_else(e) , hasElse(true) {};
        ~IfThenElse() { delete cond; delete st_then; delete st_else; }
        void printOn(std::ostream &out) const { 
            out << "If("; cond->printOn(out); out << "then" << std::endl;
            st_then->printOn(out); out << std::endl ;
            if (hasElse) { out  << "else : " << std::endl;  st_else->printOn(out); }

        }         
        Value *compile() const{
            Value *v = cond->compile();
            Value *cond = Builder.CreateICmpNE(v,c32(0), "if_cond");
            Function *TheFunction = Builder.GetInsertBlock()->getParent();
            BasicBlock *ThenBB = BasicBlock::Create(TheContext,"then", TheFunction);
            BasicBlock *AfterBB = BasicBlock::Create(TheContext,"endif", TheFunction);
            BasicBlock *ElseBB = BasicBlock::Create(TheContext,"else", TheFunction);
            Builder.CreateCondBr(cond,ThenBB,ElseBB);
            Builder.SetInsertPoint(ThenBB);
            st_then->compile();
            Builder.CreateBr(AfterBB);
            Builder.SetInsertPoint(ElseBB);
            if (hasElse) st_else->compile();
            Builder.CreateBr(AfterBB);
            Builder.SetInsertPoint(AfterBB);
            return nullptr; 
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
            out << "While("; cond->printOn(out); out << ") do" << std::endl;
            body->printOn(out);
        }        
        Value *compile() const{
            // std::get current block 
            BasicBlock *PrevBB = Builder.GetInsertBlock();
            Function *TheFunction = PrevBB->getParent();
            BasicBlock *LoopBB = BasicBlock::Create(TheContext,"loop", TheFunction);
            BasicBlock *AfterBB = BasicBlock::Create(TheContext,"std::endloop", TheFunction);
            BasicBlock *BodyBB = BasicBlock::Create(TheContext,"body", TheFunction);
            // jump to loop block 
            Builder.CreateBr(LoopBB);

            Builder.SetInsertPoint(LoopBB);
            // calculate condition
            Value* Vcond = cond->compile(); 
            Builder.CreateCondBr(Vcond, BodyBB,AfterBB);

            Builder.SetInsertPoint(BodyBB);
            body->compile();
            Builder.CreateBr(LoopBB);

            Builder.SetInsertPoint(AfterBB);
            return nullptr; 
         }
};

class LabelBind : public Stmt {
    private:
        std::string lbl;
        Stmt *target;
    public:
        LabelBind(std::string name, Stmt* st) : lbl(name), target(st) {}
        ~LabelBind() {}
        void printOn(std::ostream &out) const { out <<  lbl << " : "; target->printOn(out);  }
        Value* compile() const {return nullptr;}

};

class GoTo : public Stmt {
    private:
        std::string label;
    
    public:
        GoTo(std::string lbl) : label(lbl) {}
        void printOn(std::ostream &out) const {out << "LABEL : " << label;}
        Value* compile() const {return nullptr;}

};

class ReturnStmt : public Stmt {
    public: 
        ReturnStmt(){};
        void printOn(std::ostream &out) const {out << "RET" ; }
        Value* compile() const {return nullptr;}

};

class Init : public Stmt {
    private:
        Expr *lval;
    public:
        Init(Expr* lval) {}
        void printOn(std::ostream &out) const {out << "Init"; lval->printOn(out); }
        Value* compile() const {return nullptr;}

};

class InitArray : public Stmt {
    private:
        Expr *lval;
    public:
        InitArray(Expr* lval, Expr* sz) {}
        void printOn(std::ostream &out) const {out << "InitArr"; lval->printOn(out); }
        Value* compile() const {return nullptr;}

};

class Dispose : public Stmt {
    private:
        Expr *lval;
            
    public:
        Dispose(Expr *lval) : lval(lval) {} 
        void printOn(std::ostream &out) const {out << "Destroy"; lval->printOn(out); }
        Value* compile() const {return nullptr;}

};

class DisposeArray : public Stmt {
    private:
        Expr* lval ;
    public:
        DisposeArray(Expr* lval) {} 
        void printOn(std::ostream &out) const {out << "DestroyArr " ; lval->printOn(out) ;}
        Value* compile() const {return nullptr;}

};
