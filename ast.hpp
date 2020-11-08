#pragma once
#include <stdlib.h> 
#include <string>
#include <iostream>
#include "symbol/symbol.h"


class AST {
public:
    virtual ~AST() {}
    virtual void printOn(std::ostream &out) const = 0;
    // Cannot change the value of this object. Can be called by const objects.
    virtual void sem() {}
};
// Operator << on AST
inline std::ostream& operator<<(std::ostream &out, const AST &t) {
    t.printOn(out);
    return out;
}

class Expr: public AST {
    void type_check(std::string t) {
        sem(); // Get type (and maybe some other things)
        if (type != t) {
            std::cerr << "Type mismatch" << std::endl;
            exit(1);
        
        }
    }
    private:
        std::string type;
        
};


class Op : public Expr{
    public:
    Op(Expr* left, std::string op,Expr *right): left(left),right(right),op(op){
        printOn(std::cout);
    }
    void sem(){
        if (!op.compare("+")){
            
        }
    } 
    virtual void printOn(std::ostream &out) const override {
         out << op << "(" << *left << ", " << *right << ")";
    }

    private: 
        Expr* left;
        Expr* right;
        std::string op;
          
};

class Dereferecne : public Expr{
};

class Call : public Expr{
};

class Const : public Expr{
};

