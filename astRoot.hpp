
#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>

using namespace llvm;

class AST {
    public:
        virtual ~AST() {}
        virtual void semantic() {}
        virtual void printOn(std::ostream &out) const = 0;
        virtual Value* compile() = 0 ;
        void compile_llvm(){
            // Initialize Module
            TheModule = std::make_unique<Module>("add program name here?",TheContext);

            // TODO : find how to make optimizations passes in new LLVM versions. 

            // USEFUL TYPES
            i1 = IntegerType::get(TheContext,1);
            i8 = IntegerType::get(TheContext,8);
            i32 = IntegerType::get(TheContext,32);
            i64 = IntegerType::get(TheContext,64);
            r64 =  Type::getDoubleTy(TheContext);
            voidTy = Type::getVoidTy(TheContext);

            // DEFINE LIB FUNCTIONS
            define_external();
            /*
                These functions should be in the global namespcae
                unless shadowed by other variable nabes, so they should be added the symbol table
            */

            // GARBAGE COLLECTION
            TheMalloc = Function::Create(
                FunctionType::get( PointerType::get(i8,0), {i64}, false),
                Function::ExternalLinkage, "GC_malloc",TheModule.get()
            );
            TheInit = Function::Create(
                FunctionType::get(voidTy, {}, false),
                Function::ExternalLinkage, "GC_init",TheModule.get()
            );

            // MAIN FUNCTION
            Function *main = Function::Create(
                FunctionType::get(i32,{}, false), 
                Function::ExternalLinkage,"main",TheModule.get() 
                );
            BasicBlock * BB = BasicBlock::Create(TheContext,"entry",main);
            Builder.SetInsertPoint(BB);
            Builder.CreateCall(TheInit, {});

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

        
    protected:

        static LLVMContext TheContext; 
        static IRBuilder<>  Builder; 
        static std::unique_ptr<Module> TheModule;
        
        static ConstantInt* c_i32(int n){ return ConstantInt::get(TheContext,APInt(32,n,true)); }
        static ConstantInt* c_i8(char c){ return ConstantInt::get(TheContext,APInt(8,c,false)); }
        static ConstantInt* c_i1(int n){ return ConstantInt::get(TheContext,APInt(1,n,false)); }
        static ConstantFP* c_r64(double d) {return ConstantFP::get(TheContext,APFloat(d));}

        static Type *i1; 
        static Type *i8; 
        static Type *i32; 
        static Type *i64; 
        static Type *r64; 
        static Type *voidTy;

        /* Should add this to the global symboltable
           scope, instead of keeping them at the AST node  
        */


        static Function *WriteInteger;
        static Function *WriteBoolean;
        static Function *WriteChar;
        static Function *WriteReal;
        static Function *WriteString;

        static Function *ReadInteger;
        static Function *ReadBoolean;
        static Function *ReadChar;
        static Function *ReadReal;
        static Function *ReadString;

        static Function *Abs;
        static Function *FAbs;
        static Function *Sqrt;
        static Function *Sin;
        static Function *Cos;
        static Function *Tan;
        static Function *Arctan;
        static Function *Exp;
        static Function *Ln;
        static Function *Pi;

        static Function *TheMalloc;
        static Function *TheInit;
        
        Type* TypeConvert  (SymType t) {
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
        void define_external(){
            // PREDEFINED LIBRARY FUNCTIONS 

            // WRITE UTILS 

            WriteInteger = Function::Create(
                FunctionType::get(voidTy,{i32},false), 
                Function::ExternalLinkage, "writeInteger", TheModule.get()
            );
            WriteBoolean = Function::Create(
                FunctionType::get(voidTy,{i1},false), 
                Function::ExternalLinkage, "writeString", TheModule.get()
            );
            WriteChar = Function::Create(
                FunctionType::get(voidTy,{i8},false), 
                Function::ExternalLinkage, "writeInteger", TheModule.get()
            );
            WriteReal = Function::Create(
                FunctionType::get(voidTy,{r64},false), 
                Function::ExternalLinkage, "writeInteger", TheModule.get()
            );
            WriteString = Function::Create(
                FunctionType::get(voidTy,{PointerType::get(i8, 0)},false), 
                Function::ExternalLinkage, "writeInteger", TheModule.get()
            );

            // READ UTILS

            ReadInteger = Function::Create(
                FunctionType::get(r64,voidTy,false), 
                Function::ExternalLinkage, "writeInteger", TheModule.get()
            );
            ReadBoolean = Function::Create(
                FunctionType::get(i1,voidTy,false), 
                Function::ExternalLinkage, "writeInteger", TheModule.get()
            );
            ReadChar = Function::Create(
                FunctionType::get(i8,voidTy,false), 
                Function::ExternalLinkage, "writeInteger", TheModule.get()
            );
            ReadReal = Function::Create(
                FunctionType::get(r64,voidTy,false), 
                Function::ExternalLinkage, "writeInteger", TheModule.get()
            );
            ReadString = Function::Create(
                FunctionType::get(PointerType::get(i8, 0),voidTy,false), 
                Function::ExternalLinkage, "writeInteger", TheModule.get()
            );

            // MATH UTILS 

            Abs = Function::Create(
                FunctionType::get(i32,{i32},false), 
                Function::ExternalLinkage, "Abs", TheModule.get()
            );
            FAbs = Function::Create(
                FunctionType::get(r64,{r64},false), 
                Function::ExternalLinkage, "FAbs", TheModule.get()
            );
            Sqrt = Function::Create(
                FunctionType::get(r64,{r64},false), 
                Function::ExternalLinkage, "Sqrt", TheModule.get()
            );
            Sin = Function::Create(
                FunctionType::get(r64,{r64},false), 
                Function::ExternalLinkage, "Sin", TheModule.get()
            );
            Cos = Function::Create(
                FunctionType::get(r64,{r64},false), 
                Function::ExternalLinkage, "Cos", TheModule.get()
            );
            Tan = Function::Create(
                FunctionType::get(r64,{r64},false), 
                Function::ExternalLinkage, "Tan", TheModule.get()
            );
            Arctan = Function::Create(
                FunctionType::get(r64,{r64},false), 
                Function::ExternalLinkage, "Arctan", TheModule.get()
            );
            Exp = Function::Create(
                FunctionType::get(r64,{r64},false), 
                Function::ExternalLinkage, "Exp", TheModule.get()
            );
            Ln = Function::Create(
                FunctionType::get(r64,{r64},false), 
                Function::ExternalLinkage, "Ln", TheModule.get()
            );
            Pi = Function::Create(
                FunctionType::get(r64,voidTy,false), 
                Function::ExternalLinkage, "Pi", TheModule.get()
            );


        }

}   
;

// Operator << on AST
inline std::ostream& operator<<(std::ostream &out, const AST &t) {
  t.printOn(out);
  return out;
}
