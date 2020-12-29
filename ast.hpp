#pragma once 
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <variant> 

#include "symbol.hpp"

#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>

// for garbage collection of malloc arrays 
// add -lgc to lib flags 

using namespace llvm;




class AST {
    public:
        virtual ~AST() {}
        virtual void semantic() {}
        virtual void printOn(std::ostream &out) const = 0;
        virtual Value* compile() = 0 ;
        void compile_llvm(){
            // Initialize Module
            TheModule = new Module("add program name here?",TheContext);

            // TODO : find how to make optimizations passes in new LLVM versions. 

            // USEFUL TYPES
            i1 = IntegerType::get(TheContext,1);
            i8 = IntegerType::get(TheContext,8);
            i32 = IntegerType::get(TheContext,32);
            i64 = IntegerType::get(TheContext,64);
            r64 =  Type::getDoubleTy(TheContext);
            voidTy = Type::getVoidTy(TheContext);


            /*
                These functions should be in the global namespcae
                unless shadowed by other variable nabes, so they should be added the symbol table
            */

            // GARBAGE COLLECTION
            // optional, let's leave it for now 
            /*TheMalloc = Function::Create(
                FunctionType::get( PointerType::get(i8,0), {i64}, false),
                Function::ExternalLinkage, "GC_malloc",TheModule
            );
            
            TheInit = Function::Create(
                FunctionType::get(voidTy, {}, false),
                Function::ExternalLinkage, "GC_init",TheModule
            );
            */
            // MAIN FUNCTION
            Function *main = Function::Create(
                FunctionType::get(i32,{}, false), 
                Function::ExternalLinkage,"main",TheModule
                );
            BasicBlock * BB = BasicBlock::Create(TheContext,"entry",main);
            Builder.SetInsertPoint(BB);
            //Builder.CreateCall(TheInit, {});

            compile();
            Builder.CreateRet(c_i32(0));

            // VERIFICATION
            bool failed = verifyModule(*TheModule,&errs());
            if (failed) { 
                std::cerr << "Problem in the IR" << std::endl; 
                TheModule->print(errs(), nullptr);
                std::exit(1);
            } 

            // PRINT INTERMEDIATE IR 
            // https://stackoverflow.com/questions/65000322/creating-raw-ostream-object-in-llvm
            TheModule->print(outs(),nullptr);
        }

        static LLVMContext TheContext; 
        static IRBuilder<>  Builder; 
        static Module* TheModule;
        static Type* i1; 
        static Type* i8; 
        static Type* i32;
        static Type* i64;
        static Type* r64;
        static Type* voidTy;
        ConstantInt* c_i32(int n){ return ConstantInt::get(TheContext,APInt(32,n,true)); }
        ConstantInt* c_i8(char c){ return ConstantInt::get(TheContext,APInt(8,c,false)); }
        ConstantInt* c_i1(int n){ return ConstantInt::get(TheContext,APInt(1,n,false)); }
        ConstantFP* c_r64(double d) {return ConstantFP::get(TheContext,APFloat(d));}
        static SymbolTable st; 


        Type* TypeConvert  (Stype t) {
            switch(t->kind) {
                case TYPE_VOID      : return voidTy;
                case TYPE_INTEGER   : return i32; 
                case TYPE_BOOLEAN   : return i1; 
                case TYPE_CHAR      : return i8; 
                case TYPE_REAL      : return r64;
                case TYPE_ARRAY : 
                    return ArrayType::get(TypeConvert(t->refType), t->size);
                case TYPE_IARRAY :
                    // seems like for llvm semantics this is the same 
                    return PointerType::get(TypeConvert(t->refType),0);
                case TYPE_POINTER : 
                    return PointerType::get(TypeConvert(t->refType),0);
            }
            return voidTy;
        }


}   
;

// Operator << on AST
inline std::ostream& operator<<(std::ostream &out, const AST &t) {
  t.printOn(out);
  return out;
}

inline std::ostream& operator<<(std::ostream &out, const std::vector<std::string> &t) {
        out << "[";
        for (std::size_t i = 0; i < t.size(); i++) {
            out << t[t.size() - 1 - i] << (i == t.size() - 1 ? "" : ",");
        }
        return out << "]" ;
}

class Expr : public AST {
    public:
        Stype type;
        virtual void printOn(std::ostream &out) const = 0;
        virtual Value* compile()  = 0;
};
class Stmt : public AST {
    public : 
        virtual void printOn(std::ostream &out) const = 0;
        virtual Value* compile()  = 0;
};

template<class T>
class ASTvector : public Stmt , public Expr{
    public : 
    Value* compile() {return nullptr;}
    std::vector<T> list; 
    void printOn(std::ostream &out) const {
        out << "[";
        for (std::size_t i = 0; i < list.size(); i++) {
            out << *(list[list.size() - 1 - i]) << (i == list.size() - 1 ? "" : ",");
        }
        out << "]"; 
    }
    void push(T t){ list.push_back(t);}

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
        void semantic() override{}
        void printOn(std::ostream &out) const {
            left->printOn(out); 
            out << " " << op; 
            out << " ";
            right->printOn(out); 
        }
        Value* compile()  {
            Value *l = left->compile();
            Value *r = nullptr; 
            // need to implement short-circuit
            if (op != "and" && op != "or") r = right->compile();
            bool real_ops = false; 
            // sign exted to real if necessary
            if (left->type->kind == TYPE_REAL || right->type->kind == TYPE_REAL) {
                // can also use Value type here for cmp 
                if (left->type->kind != TYPE_REAL) Builder.CreateSExt(l,r64);
                if (right->type->kind != TYPE_REAL) Builder.CreateSExt(r,r64);
                real_ops = true ;
            }
            switch( hashf(op.c_str()) ){
                case "+"_ :
                    if (real_ops)   return Builder.CreateFAdd(l,r,"addtmp");
                    else            return Builder.CreateAdd(l,r,"addtmp");
                case "-"_ : 
                    if (real_ops)   return Builder.CreateFSub(l,r,"addtmp");
                    else            return Builder.CreateSub(l,r,"addtmp");
                return Builder.CreateSub(l,r,"subtmp");
                case "*"_ : 
                    if (real_ops)   return Builder.CreateFMul(l,r,"addtmp");
                    else            return Builder.CreateMul(l,r,"addtmp");
                case "/"_ : 
                    if (left->type->kind == TYPE_INTEGER && right->type->kind == TYPE_INTEGER) {
                        Builder.CreateSExt(l,r64);
                        Builder.CreateSExt(r,r64);
                    }
                    return Builder.CreateFDiv(l,r,"addtmp");
    
                // we use signed modulo ops of integer operants
                case "div"_ : return Builder.CreateSDiv(l,r,"divtmp");
                case "mod"_ : return Builder.CreateSRem(l,r,"modtmp"); 

                // we implement short circuit with a branch 
                case "and"_  :
                {
                    Function *TheFunction = Builder.GetInsertBlock()->getParent();
                    BasicBlock *shortBB = BasicBlock::Create(TheContext,"short_and", TheFunction);
                    BasicBlock *fullBB = BasicBlock::Create(TheContext,"full_and", TheFunction);
                    BasicBlock *endBB = BasicBlock::Create(TheContext,"end_and", TheFunction);

                    Value* cond = Builder.CreateICmpEQ(l,c_i1(0)); 
                    Builder.CreateCondBr(cond, shortBB,fullBB);
                    
                    // just jump 
                    Builder.SetInsertPoint(shortBB);
                    Builder.CreateBr(endBB);

                    // need to evaluate second operant                    
                    Builder.SetInsertPoint(fullBB);
                    r = right->compile();

                    // get current block because compile can be arbitrary
                    BasicBlock *CurrBB = Builder.GetInsertBlock();
                    Value *OP =  Builder.CreateAnd(l,r,"andtmp");
                    Builder.CreateBr(endBB);

                    Builder.SetInsertPoint(endBB);
                    // combine the 2 branches 
                    PHINode *phi = Builder.CreatePHI(i32, 2, "phi_and");
                    phi->addIncoming(c_i1(0),shortBB);
                    phi->addIncoming(OP,CurrBB);

                    return phi;
                    }
                case "or"_ : 
                {
                    Function *TheFunction = Builder.GetInsertBlock()->getParent();
                    BasicBlock *shortBB = BasicBlock::Create(TheContext,"short_or", TheFunction);
                    BasicBlock *fullBB = BasicBlock::Create(TheContext,"full_or", TheFunction);
                    BasicBlock *endBB = BasicBlock::Create(TheContext,"end_or", TheFunction);

                    Value* cond = Builder.CreateICmpEQ(l,c_i1(1)); 
                    Builder.CreateCondBr(cond, shortBB,fullBB);
                    
                    // just jump 
                    Builder.SetInsertPoint(shortBB);
                    Builder.CreateBr(endBB);

                    // need to evaluate second operant                    
                    Builder.SetInsertPoint(fullBB);
                    r = right->compile();

                    // get current block because compile can be arbitrary
                    BasicBlock *CurrBB = Builder.GetInsertBlock();
                    Value *OP =  Builder.CreateAnd(l,r,"ortmp");
                    Builder.CreateBr(endBB);

                    Builder.SetInsertPoint(endBB);
                    // combine the 2 branches 
                    PHINode *phi = Builder.CreatePHI(i32, 2, "phi_or");
                    phi->addIncoming(c_i1(1),shortBB);
                    phi->addIncoming(OP,CurrBB);

                    return phi;
                    }
                case "="_ : 
                    if (real_ops)   return Builder.CreateFCmpOEQ(l,r,"eqtmp");
                    else            return Builder.CreateICmpEQ(l,r,"eqtmp");
                case "<>"_ : 
                    if (real_ops)   return Builder.CreateFCmpONE(l,r,"neqtmp");
                    else            return Builder.CreateICmpNE(l,r,"neqtmp");
                /* "=,<>" :  results are boolean, 
                    * if operants are both arithmetic then we compare values, 
                      we have taken care of type casting
                    * if they are of any other type we simply compare, 
                      type checking takes care of type matching
                */
                case ">"_ :                     
                    if (real_ops)   return Builder.CreateFCmpOGT(l,r);
                    else            return Builder.CreateICmpSGT(l,r);
                case "<"_ :
                    if (real_ops)   return Builder.CreateFCmpOLT(l,r);
                    else            return Builder.CreateICmpSLT(l,r);
                case "<="_ : 
                    if (real_ops)   return Builder.CreateFCmpOLE(l,r);
                    else            return Builder.CreateICmpSLE(l,r);
                case ">="_ : 
                    if (real_ops)   return Builder.CreateFCmpOGT(l,r);
                    else            return Builder.CreateICmpSGT(l,r);
                // results must be arithmetic, some typecasting req
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
        
        void semantic() override{}
        void printOn(std::ostream &out) const {out << "(" << op ;
        e->printOn(out); 
        out << ")";}
        Value* compile()  {
            Value* val = e->compile();
            switch( hashf(op.c_str()) ){
                case "+"_ : return val;
                case "-"_ : return Builder.CreateNeg(val,"negtmp");
                case "not"_ : return Builder.CreateNot(val,"nottmp");
                case "@"_ : 
                    // TODO 
                    // does not seem straightforward
                    /*  generally we have 2 types of variable,
                        those created with alloca, 
                        and heap arrays, 
                        need to see  
                    */
                    std::cerr << "NOT_IMPLEMENTED";
                    return nullptr; 
            }
            return nullptr; 

        }
};

class Id : public Expr {
    /* Id as an l-value
    */
    public:
        std::string name ;
        Id(std::string n) : name(n) {}
        void printOn(std::ostream &out) const {out << "Id(" << name <<  ")";}
        Value* compile() {
            // TODO 
            /*
                Search for name in symbol table, 
                return associated Value
            */
            return nullptr; }

};


typedef std::variant<int,double,char,bool> data_const ;
// https://en.cppreference.com/w/cpp/utility/variant/holds_alternative
// https://en.cppreference.com/w/cpp/utility/variant     

class Const : public Expr {

    public:
        data_const val;     
        Stype type;
        Const(data_const val, Stype t) : val(val) , type(t) {}      
        void printOn(std::ostream &out) const {
            switch(type->kind) {
            
                case TYPE_INTEGER : out << std::get<int>(val) ; break;
                case TYPE_BOOLEAN : out << std::get<bool>(val) ; break;
                case TYPE_CHAR : out << std::get<char>(val) ; break;
                case TYPE_REAL : out << std::get<double>(val) ; break;
                case TYPE_VOID : out << "Nil" ; break; 

            } 
        }
        Value* compile() {
            switch(type->kind) {

            case TYPE_INTEGER : return c_i32(std::get<int>(val));
            case TYPE_BOOLEAN : return c_i1(std::get<bool>(val));
            case TYPE_CHAR : return c_i8(std::get<char>(val));
            case TYPE_REAL : return c_r64(std::get<double>(val));;
            case TYPE_VOID : return nullptr;  

            }
            return nullptr;  
        }
};



// remember to account for both a return argument
// and no return argument
class CallFunc : public Expr {
    private:
        std::string fname;
        ASTvector<Expr*> parameters;
    public:
        CallFunc(std::string name, ASTvector<Expr*>* params) : fname(name), parameters(*params) {}
        CallFunc(std::string name) : fname(name) {}
        void printOn(std::ostream &out) const {
            out << "Call(" << fname; 
            if (!parameters.list.empty()) parameters.printOn(out); 
            out << ")";}
        Value* compile()  {
            // TODO get function from symbol table 
            // Function func = nullptr; 
            // calculate values for arguments
            // parameters->compile();
            // Value* ret = Builder.CreateCall(func,parameters->values);
            //return ret;

            return nullptr; 
            }

};

class CallProc : public Stmt {
    private:
        std::string fname;
        ASTvector<Expr*> parameters;
    public:
        CallProc(std::string name, ASTvector<Expr*>* params) : fname(name), parameters(*params) {}
        CallProc(std::string name) : fname(name){}
        void printOn(std::ostream &out) const {
            out << "Call(" << fname; 
            if (!parameters.list.empty()) parameters.printOn(out); 
            out << ")";}
        Value* compile()  {
            // TODO get function from symbol table 
            // Function func = nullptr; 
            // calculate values for arguments
            // parameters->compile();
            // Value* ret = Builder.CreateCall(func,parameters->values);
            //return ret;

            return nullptr; 
            }

};

/*  String should be handled with care,
    like an array type but you can't assign ? 
*/ 
class String : public Expr {
    private:
        std::string s; 
        // should add type array [n] of char
    public:
        String(std::string s) : s(s){}
        void printOn(std::ostream &out) const {out << '\"' << s << '\"';}
        Value* compile()  {
            /* string is just a static array of i8 (char), 
               created with alloca, this could be also be unified with the array class
               TODO 
            */ 
            return nullptr;}

};

class ArrayAccess : public Expr {
    private:
        Expr* lval; 
        Expr* pos; 
    public:
        ArrayAccess(Expr* lval, Expr* pos) : lval(lval) , pos(pos){}
        void printOn(std::ostream &out) const { lval->printOn(out); out << "[";  pos->printOn(out); out << "]";}
        Value* compile()  {
            /*
                TODO 
                probably need a GEP instruction
            */
            return nullptr;}


};

class Dereference : public Expr {
    private:
        Expr* e;
    public:
        Dereference(Expr* e) : e(e) {}
        void printOn(std::ostream &out) const { e->printOn(out) ; out << "^";}
        Value* compile()  {
            // TODO 
            // do the right GEP instruction for the Pointer
            return nullptr; 
        }

};

/* Statements */

class Block : public Stmt {
    /*
        This class implements the body of each structure

        local :
            variable definitions : VariableGroupStack( [variables]) , variable = {type,id}
            label definitions : Label( [id] ) 
            a function definition : Routine() , routine is a 
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

        void push_local(Stmt *l)
        {locals.list.push_back(l);}

        void printOn(std::ostream &out) const {
            out << "Block of: "  << std::endl; 
            out << "\t\tlocals: " ; locals.printOn(out); out << std::endl; 
            out << "\t\tbody: " ; body.printOn(out); out <<  std::endl; 
        }
        Value* compile()  {
            // we compile each locals, calling the right method for its initial class
            // one of Label(),Routine(),etc. 
            for (auto x : locals.list) x->compile();
            for (auto x : body.list) x->compile();
            return nullptr;
        }

};


/* These 2 classes can be merged but we leave it as is for now 
*/ 

class Variable : public Stmt {
    public:
    Stype t;
    std::string s; 
    Variable(std::string s, Stype t) : s(s), t(t){}
    Value * compile(){return nullptr;}
    void printOn(std::ostream &out) const { out << s << " : " << t;}
};

class VarDef : public Stmt {
    /* Class containing variable definitions
    */ 
    private:
        ASTvector<Stmt*> vars;    
    public:
        void push(std::vector<std::string>* var_ids, Stype t){
            for (std::string s : *var_ids ) {
                vars.list.push_back(new Variable(s,t));
            }
        }
        void printOn(std::ostream &out) const { out << "VarDef :"; vars.printOn(out) ;}
        Value* compile()  {
            // create an alloca for each variable
            // add to symbol table  
            return nullptr;}

};

class LabelDef : public Stmt {
    private:
        std::vector<std::string> labels;
    public:
        LabelDef(std::vector<std::string>* names) : labels(*names) {}
        void printOn(std::ostream &out) const { out << "LabelDef";  out << labels ;}
        Value* compile()  {
            // TODO 
            /*
                labels are essentially created as new BasicBlocks
                here we just declare labels , add them to symbol table 
            */
            return nullptr; 
        }

};
typedef enum{PASS_BY_REFERENCE,PASS_BY_VALUE} PassMode;
class FormalsGroup : public Stmt {
    public:
        std::vector<std::string> formals;
        Stype type;
        PassMode pass_by;

    
        FormalsGroup(std::vector<std::string>* f, Stype t, PassMode pm) : formals(*f), type(t), pass_by(pm) {}

        void printOn(std::ostream &out) const { 
            if (pass_by == PASS_BY_REFERENCE) out << "var: "; 
            out << formals;
            out <<  " : " ; out << type; 
        }
        Value* compile()  {
            /*  TODO 
                alloca for parameter variables, add to symbol table
                how to signify pass_by value or by reference? 
                use byref(<ty>), byval(<ty>) in llvm language specification ? 
            */           
            return nullptr; 
        }

};


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
        
        void printOn(std::ostream &out) const { 
            out << name ; parameters.printOn(out); out << " : " << type <<std::endl ;
            body->printOn(out);
        }
        void set_forward(){this->isForward = true;}
        void add_body(Block* theBody){this->body = theBody;}

        Value* compile()  {

            std::vector<Type*> param_types; 
            for (auto param : parameters.list){
                // for some reason passing it as is, results in const error 
                param_types.push_back(TypeConvert(param->type)) ;
            }
            // TODO : see how to handle by val and by reference params   
            FunctionType* Ftype =  FunctionType::get(TypeConvert(type),param_types,false);
            Function *routine = Function::Create(Ftype,
            Function::ExternalLinkage,name,TheModule
            );
            BasicBlock * BB = BasicBlock::Create(TheContext,"entry",routine);
            Builder.SetInsertPoint(BB);
            if (!isForward) body->compile();
            // TODO : symbol.insert(Function); 
            return nullptr; 
        }

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
        Value* compile()  {
            // TODO 
            /*  store command, check language specification, 
                and how to assign with heap structures
            */ 
            return nullptr;}

};

class IfThenElse : public Stmt {
    private:
        Expr *cond;
        Stmt *st_then, *st_else = nullptr;
        bool hasElse; 
        
    public:
        IfThenElse(Expr* c, Stmt* t) : cond(c), st_then(t) , hasElse(false) {};
        IfThenElse(Expr* c, Stmt* t, Stmt* e) : cond(c), st_then(t), st_else(e) , hasElse(true) {};
        void printOn(std::ostream &out) const { 
            out << "If("; cond->printOn(out); out << " then " << std::endl;
            st_then->printOn(out); out << std::endl ;
            if (hasElse) { out  << "else : " << std::endl;  st_else->printOn(out); }

        }         
        Value *compile() {
            Value *v = cond->compile();
            Value *cond = Builder.CreateICmpNE(v,c_i32(0), "if_cond");
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
        void printOn(std::ostream &out) const {
            out << "While("; cond->printOn(out); out << ") do" << std::endl;
            body->printOn(out);
        }        
        Value *compile() {
            // get current block 
            BasicBlock *PrevBB = Builder.GetInsertBlock();
            Function *TheFunction = PrevBB->getParent();
            BasicBlock *LoopBB = BasicBlock::Create(TheContext,"loop", TheFunction);
            BasicBlock *AfterBB = BasicBlock::Create(TheContext,"endloop", TheFunction);
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


class Label : public Stmt {
/*
    Implements actual label encounter in code
*/
    private:
        std::string lbl;
        Stmt *target;
    public:
        Label(std::string name, Stmt* st) : lbl(name), target(st) {}
        void printOn(std::ostream &out) const { out <<  lbl << " : "; target->printOn(out);  }
        Value* compile()  {
            Function *TheFunction = Builder.GetInsertBlock()->getParent();
            BasicBlock * LabelBB  = BasicBlock::Create(TheContext,lbl, TheFunction);
            Builder.SetInsertPoint(LabelBB);
            // TODO
            // update symbol table with the BasicBlock*
            // label is already added in the Label() class
            target->compile();
            return nullptr; 
        }
};

class GoTo : public Stmt {
    private:
        std::string label;
    public:
        GoTo(std::string lbl) : label(lbl) {}
        void printOn(std::ostream &out) const {out << "LABEL : " << label;}
        Value* compile()  {
            // TODO :
            // search into symbol table for label 
            //Builder.CreateBr();
            return nullptr; 
        }

};

class ReturnStmt : public Stmt {
    public: 
        ReturnStmt(){};
        void printOn(std::ostream &out) const {out << "RET" ; }
        Value* compile()  {
            // TODO 
            // search for result in symboltable 
            // if result is set return it with CreateRet()
            return nullptr;}

};


// could be merged but we leave it as 2 classes for now.
class Init : public Stmt {
    private:
        Expr *lval;
    public:
        Init(Expr* lval) {}
        void printOn(std::ostream &out) const {out << "Init"; lval->printOn(out); }
        Value* compile()  {
            // TODO
            /*  Create call to malloc_gc ,
                for concrete type pointers, e.g. everything except dynamic arrays
                need to have type of Expr
            */
            return nullptr;}

};

class InitArray : public Stmt {
    private:
        Expr *lval;
    public:
        InitArray(Expr* lval, Expr* sz) {}
        void printOn(std::ostream &out) const {out << "InitArr"; lval->printOn(out); }
        Value* compile()  {
            // TODO 
            /* add a conrete type array [n] of t, to an l-value of ^array of t
            */
            return nullptr; 
        }

};

// we merged the DisposeArray class
class Dispose : public Stmt {
    private:
        Expr *lval;
            
    public:
        Dispose(Expr *lval) : lval(lval) {} 
        void printOn(std::ostream &out) const {out << "Destroy"; lval->printOn(out); }
        Value* compile()  {
            // TODO 
            /* Dispose should be some function call from gc library 
            */ 
            return nullptr;}

};

