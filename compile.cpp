#include  <cstdlib>
#include "ast.hpp"
#include "symbol.hpp"

// for optimization 
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Verifier.h>

#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include <llvm/Transforms/Utils.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>

using namespace llvm;

LLVMContext TheContext; 
IRBuilder<> Builder(TheContext); 
Module* TheModule;

Type* i1; 
Type* i8; 
Type* i32;
Type* i64;
Type* r64;
Type* voidTy;

Function* GC_Malloc;
Function* GC_Init;
Function* GC_Free;

CodeGenTable ct = CodeGenTable();

ConstantInt* c_i32(int n)
{
    return ConstantInt::get(TheContext,APInt(32,n,true));
}

ConstantInt* c_i8(char c)
{
    return ConstantInt::get(TheContext,APInt(8,c,false));
}

ConstantInt* c_i1(int n)
{
    return ConstantInt::get(TheContext,APInt(1,n,false));
}

ConstantFP* c_r64(double d)
{
    return ConstantFP::get(TheContext,APFloat(d));
}

inline void AST::add_func(FunctionType *type, std::string name)
{
    Function *func = Function::Create(
            type, 
            Function::ExternalLinkage, name, TheModule
        );
        ct.insert(name,func);
};

inline void AST::add_libs()
{
    // PREDEFINED LIBRARY FUNCTIONS 

    // WRITE UTILS 

    add_func(FunctionType::get(voidTy,{i32},false),"writeInteger");
    add_func(FunctionType::get(voidTy,{i1},false), "writeBoolean");
    add_func(FunctionType::get(voidTy,{i8},false), "writeChar");
    add_func(FunctionType::get(voidTy,{r64},false), "writeReal");
    add_func(FunctionType::get(voidTy,{PointerType::get(i8, 0)},false),"writeString");

    // READ UTILS

    add_func(FunctionType::get(r64,{},false),"readInteger");
    add_func(FunctionType::get(i1,{},false), "readBoolean");
    add_func(FunctionType::get(i8,{},false), "readChar");
    add_func(FunctionType::get(r64,{},false), "readReal");
    add_func(FunctionType::get(PointerType::get(i8, 0),{},false),"readString");


    // MATH UTILS 

    add_func(FunctionType::get(i32,{i32},false),"abs");
    add_func(FunctionType::get(r64,{r64},false),"fabs");
    add_func(FunctionType::get(r64,{r64},false),"sqrt");
    add_func(FunctionType::get(r64,{r64},false),"sin");
    add_func(FunctionType::get(r64,{r64},false),"cos");
    add_func(FunctionType::get(r64,{r64},false),"tan");
    add_func(FunctionType::get(r64,{r64},false),"arctan");
    add_func(FunctionType::get(r64,{r64},false),"exp");
    add_func(FunctionType::get(r64,{r64},false),"ln");
    add_func(FunctionType::get(r64,{},false),"pi");

};

void AST::compile_llvm(std::string program_name, bool optimize,bool imm_stdout)
{
    // Initialize Module
    TheModule = new Module(program_name,TheContext);

    // useful types
    i1 = IntegerType::get(TheContext,1);
    i8 = IntegerType::get(TheContext,8);
    i32 = IntegerType::get(TheContext,32);
    i64 = IntegerType::get(TheContext,64);
    r64 =  Type::getDoubleTy(TheContext);
    voidTy = Type::getVoidTy(TheContext);

    // garbage collection and dynamic heap alloc
    GC_Malloc = Function::Create(
        FunctionType::get( PointerType::get(i8,0), {i64}, false),
        Function::ExternalLinkage, "GC_malloc",TheModule
    );
    
    GC_Init = Function::Create(
        FunctionType::get(voidTy, {}, false),
        Function::ExternalLinkage, "GC_init",TheModule
    );

    GC_Free = Function::Create(
        FunctionType::get(voidTy ,{PointerType::get(i8,0)}, false), 
        Function::ExternalLinkage,"GC_free",TheModule
        );
    
    
    // open global scope 
    ct.openScope();

    // add external libraries 
    add_libs();

    // add main 
    Function *main = Function::Create(
        FunctionType::get(i32,{}, false), 
        Function::ExternalLinkage,"main",TheModule
        );
    BasicBlock * BB = BasicBlock::Create(TheContext,"entry",main);
    Builder.SetInsertPoint(BB);
    Builder.CreateCall(GC_Init, {});

    // compile program 
    compile();

    Builder.CreateRet(c_i32(0));
    ct.closeScope();

    // verify module for badly formed IR
    bool failed = verifyModule(*TheModule,&errs());
    if (failed) { 
        TheModule->print(errs(), nullptr);
        std::cerr << "Problem in the IR" << std::endl; 
        std::exit(1);
    } 

    // do optimizations if necessary 
    if (optimize) {
        /*  This is legacy optimization 
            TODO : add newer version 
        */
        legacy::FunctionPassManager TheFPM = legacy::FunctionPassManager(TheModule);
        TheFPM.add(createPromoteMemoryToRegisterPass());
        TheFPM.add(createInstructionCombiningPass());
        TheFPM.add(createReassociatePass());
        TheFPM.add(createGVNPass());
        TheFPM.add(createCFGSimplificationPass());
        TheFPM.doInitialization();
        TheFPM.run(*main);
    }



    if (imm_stdout){

        // print IR to stdout 
        TheModule->print(outs(),nullptr);
    }
    else {

        // print IR to file 
        std::error_code EC;
        raw_ostream * out = new raw_fd_ostream(program_name + ".ll",EC);
        TheModule->print(*out,nullptr);

        // print bin to file 
        std::string cmd = "clang " + program_name + ".ll libPCL.a -lgc -o " +  program_name + ".out" + " -g";
        system(cmd.c_str());
    }
}

Value* EmptyStmt::compile() {return nullptr;}

Value* BinOp::compile() /* override */
{
    Value *l = left->compile();
    Value *r = nullptr; 
    // need to implement short-circuit
    if (op != "and" && op != "or") r = right->compile();
    bool real_ops = false; 
    // sign exted to real if necessary
    if (check_type(left->type,typeReal) || check_type(right->type,typeReal)) {
        // can also use Value type here for cmp 
        if (! check_type(left->type, typeReal))  Builder.CreateSExt(l,r64);
        if (! check_type(right->type, typeReal)) Builder.CreateSExt(r,r64);
        real_ops = true ;
    }
    switch( hashf(op.c_str()) ){
        case "+"_ :
            if (real_ops)   return Builder.CreateFAdd(l,r,"addtmp");
            else            return Builder.CreateAdd(l,r,"addtmp");
        case "-"_ : 
            if (real_ops)   return Builder.CreateFSub(l,r,"addtmp");
            else            return Builder.CreateSub(l,r,"addtmp");
        case "*"_ : 
            if (real_ops)   return Builder.CreateFMul(l,r,"addtmp");
            else            return Builder.CreateMul(l,r,"addtmp");
        case "/"_ : 
            if (check_type(left->type,typeInteger) || check_type(right->type,typeInteger)) {
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

Value* UnOp::compile() /* override */
{
    Value* val = e->compile();
    switch( hashf(op.c_str()) ){
        case "+"_ : return val;
        case "-"_ : return Builder.CreateNeg(val,"negtmp");
        case "not"_ : return Builder.CreateNot(val,"nottmp");
        case "@"_ : 
            // not sure how to implement this 
            std::cerr << "NOT_IMPLEMENTED";
            return nullptr; 
    }
    return nullptr;
}

Value* Id::compile() /* override */
{
    return ct.lookup(name)->value;
}

Value* Const::compile() /* override */
{
    switch(type->kind)
    {
        case TYPE_INTEGER : return c_i32(std::get<int>(val));
        case TYPE_BOOLEAN : return c_i1(std::get<bool>(val));
        case TYPE_CHAR : return c_i8(std::get<char>(val));
        case TYPE_REAL : return c_r64(std::get<double>(val));;
        case TYPE_VOID : return nullptr;  
    }
    return nullptr;  
}

Value* CallFunc::compile() /* override */
{
    Value* func = ct.lookup(fname)->value;
    std::vector<Value*> param_values;
    for (auto p : parameters.list){
        param_values.push_back(p->compile());
    }
    Value* ret = Builder.CreateCall(func,param_values);
    return ret;
}

Value* CallProc::compile() /* override */
{
    Value* func = ct.lookup(fname)->value;
    std::vector<Value*> param_values;
    for (auto p : parameters.list){
        param_values.push_back(p->compile());
    }
    Builder.CreateCall(func,param_values);
    return nullptr; 
}

Value* String::compile() /* override */
{
    std::string pstr(s);
    std::vector<std::pair<std::string,std::string>> rep = {
        {"\\n" , "\n"} , {"\\t", "\t"}, {"\\r", "\r"} , {"\\\\", "\\"} , {"\\0", "\0"}, 
        {"\\'","'"}, {"\\\"", "\""}  };
    for (auto p : rep){
        ReplaceStringInPlace(pstr,p.first, p.second);
    }
    Value *ret = Builder.CreateGlobalStringPtr(pstr.c_str(),"str",0);
    return ret;

}

Value* ArrayAccess::compile() /* override */
{
    Value *index = pos->compile();
    Value *ptr = lval->compile();
    return Builder.CreateGEP(ptr, {0, index});
}

Value* Dereference::compile() /* override */
{
    Value *ptr = e->compile();
    return Builder.CreateGEP(ptr, {0});
}

Value* Block::compile() /* override */
{
    // we compile each locals, calling the right method for its initial class
    for (auto x : locals.list) x->compile();
    for (auto x : body.list) x->compile();
    return nullptr;

}

Value* Variable::compile() /* override */
{
    Value* value = Builder.CreateAlloca(TypeConvert(type), 0, name.c_str());
    ct.insert(name,value);
    return nullptr; 
}

Value* VarDef::compile() /* override */
{
    // get entry block of current function for allocas to work properly 
    Function *TheFunction = Builder.GetInsertBlock()->getParent();
    BasicBlock* entryBlock =&TheFunction->getEntryBlock();
    IRBuilder<> TemporalBuilder(entryBlock, entryBlock->begin());
    
    for (auto var : vars.list){ 
        var->compile();
    }                    
    return nullptr;
}

Value* LabelDef::compile() /* override */
{
    //no need to do anything, labels are basic blocks
    return nullptr;
}

Value* FormalsGroup::compile() /* override */
{
    return nullptr;
}

Value* FunctionDef::compile() /* override */
{
        
    Function *routine;
    std::vector<Type*> param_types; 
    for (FormalsGroup* param : parameters.list){
        Stype t = param->type;
        // by reference passing is just adding a pointer
        if (param->pass_by == PASS_BY_REFERENCE) 
            t = typePointer(t);            
        for (std::string name : param->formals){
             param_types.push_back(TypeConvert(param->type)) ;
        }
    }
    FunctionType* Ftype =  FunctionType::get(TypeConvert(type),param_types,false);
    routine = Function::Create(Ftype,
        Function::ExternalLinkage,name,TheModule
    );
    // insert function in current scope 
    ct.insert(name,routine);

    //create a new basic block 
    BasicBlock * BB = BasicBlock::Create(TheContext,"entry",routine);
    Builder.SetInsertPoint(BB);
    // create a new scope
    ct.openScope();
    auto param_iter = routine->arg_begin(); 
    for (FormalsGroup* param : parameters.list){
        for (std::string name : param->formals ){
            ct.insert(name,param_iter);
            param_iter++;
        }
    }
    body->compile(); 
    ct.closeScope();
    return nullptr; 
}

Value* Declaration::compile() /* override */
{   // := 
    Value* l = lval->compile();
    Value* r = rval->compile();
    //Builder.CreateLoad(r);
    Value* source; 
    if (lval->type_verify(typeReal) && rval->type_verify(typeInteger)){
        source = Builder.CreateSExt(r,r64);
    }
    else source = r; 
    Builder.CreateStore(source,l);
    return nullptr;
}

Value* IfThenElse::compile() /* override */
{
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

Value* While::compile() /* override */
{
    // get current block 
    Function *TheFunction = Builder.GetInsertBlock()->getParent();
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

Value* Label::compile() /* override */
{
    Function *TheFunction = Builder.GetInsertBlock()->getParent();
    BasicBlock * LabelBB  = BasicBlock::Create(TheContext,lbl, TheFunction);
    Builder.SetInsertPoint(LabelBB);
    ct.insert(lbl,LabelBB);
    target->compile();            
    return nullptr; 
}

Value* GoTo::compile() /* override */
{
    BasicBlock* val = (BasicBlock*)ct.lookup(label)->value;
    Builder.CreateBr(val);
    return nullptr; 
}

Value* ReturnStmt::compile() /* override */
{
    Value* ret = ct.lookup("return")->value;
    Builder.CreateRet(ret);
    return nullptr;
}

Value* Init::compile() /* override */
{
    Value* ptr = lval->compile();
    Builder.CreateCall(GC_Malloc,{ptr});
    return nullptr;
}

Value* InitArray::compile() /* override */
{
    /* add a conrete type array [n] of t, to an l-value of ^array of t */
    Value* ptr = lval->compile();
    Builder.CreateCall(GC_Malloc,{ptr});
    return nullptr; 
}

Value* Dispose::compile() /* override */
{
    /* Dispose should be some function call from gc library */ 
    Value* ptr = lval->compile();
    Builder.CreateCall(GC_Free,ptr);
    return nullptr;
}
