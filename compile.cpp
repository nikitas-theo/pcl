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

bool BBended;


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

inline void Program::add_func_llvm(FunctionType *type, std::string name, 
    std::vector<PassMode> args, std::vector<Stype> types)
{
    ct.insert(name, Function::Create(type, Function::ExternalLinkage, name, TheModule));
    CodeGenEntry* e = ct.lookup(name);
    e->arguments = args;
    e->types = types; 
};

void Program::add_libs_llvm()
{
    // PREDEFINED LIBRARY FUNCTIONS 

    // WRITE UTILS 

    add_func_llvm(FunctionType::get(voidTy,{i32},false),"writeInteger",{PASS_BY_VALUE}, {typeInteger});
    add_func_llvm(FunctionType::get(voidTy,{i1},false), "writeBoolean",{PASS_BY_VALUE}, {typeBoolean});
    add_func_llvm(FunctionType::get(voidTy,{i8},false), "writeChar",{PASS_BY_VALUE}, {typeChar});
    add_func_llvm(FunctionType::get(voidTy,{r64},false), "writeReal",{PASS_BY_VALUE}, {typeReal});
    add_func_llvm(FunctionType::get(voidTy,{PointerType::get(i8, 0)},false),"writeString",{PASS_BY_REFERENCE}, {typeChar});

    // READ UTILS

    add_func_llvm(FunctionType::get(i32,{},false),"readInteger",{}, {});
    add_func_llvm(FunctionType::get(i1,{},false), "readBoolean",{}, {});
    add_func_llvm(FunctionType::get(i8,{},false), "readChar",{}, {});
    add_func_llvm(FunctionType::get(r64,{},false), "readReal",{}, {});
    add_func_llvm(FunctionType::get(PointerType::get(i8, 0),{},false),"readString",{}, {});


    // MATH UTILS 

    add_func_llvm(FunctionType::get(i32,{i32},false),"abs",{PASS_BY_VALUE}, {typeInteger});
    add_func_llvm(FunctionType::get(r64,{r64},false),"fabs",{PASS_BY_VALUE}, {typeReal});
    add_func_llvm(FunctionType::get(r64,{r64},false),"sqrt",{PASS_BY_VALUE}, {typeReal});
    add_func_llvm(FunctionType::get(r64,{r64},false),"sin",{PASS_BY_VALUE}, {typeReal});
    add_func_llvm(FunctionType::get(r64,{r64},false),"cos",{PASS_BY_VALUE}, {typeReal});
    add_func_llvm(FunctionType::get(r64,{r64},false),"tan",{PASS_BY_VALUE}, {typeReal});
    add_func_llvm(FunctionType::get(r64,{r64},false),"arctan",{PASS_BY_VALUE}, {typeReal});
    add_func_llvm(FunctionType::get(r64,{r64},false),"exp",{PASS_BY_VALUE}, {typeReal});
    add_func_llvm(FunctionType::get(r64,{r64},false),"ln",{PASS_BY_VALUE}, {typeReal});
    add_func_llvm(FunctionType::get(r64,{},false),"pi",{PASS_BY_VALUE}, {typeReal});

    // CHAR UTILS
    add_func_llvm(FunctionType::get(i8,{i32},false),"ord",{PASS_BY_VALUE}, {typeInteger});
    add_func_llvm(FunctionType::get(i32,{i8},false),"chr",{PASS_BY_VALUE}, {typeChar});

    // ROUND UTILS

    Function *trunc = Function::Create(
        FunctionType::get(i32,{r64},false), 
        Function::ExternalLinkage, "truncFunc", TheModule);
    ct.insert("trunc",trunc);
    CodeGenEntry* e = ct.lookup("trunc");
    e->arguments = {PASS_BY_VALUE};
    e->types = {typeReal};     

    Function *round = Function::Create(
        FunctionType::get(i32,{r64},false), 
        Function::ExternalLinkage, "roundFunc", TheModule);
    ct.insert("round",round);
    e = ct.lookup("round");
    e->arguments = {PASS_BY_VALUE};
    e->types = {typeReal}; 

};

void Program::compile_initalize()
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
    
    BBended = false; 
    // open global scope 
    ct.openScope();

    // add external libraries 
    add_libs_llvm();

    // add main 
    main = Function::Create(
        FunctionType::get(i32,{}, false), 
        Function::ExternalLinkage,"main",TheModule
        );
    BasicBlock * BB = BasicBlock::Create(TheContext,"entry",main);
    Builder.SetInsertPoint(BB);
    Builder.CreateCall(GC_Init, {});
}

void Program::compile_run()
{
    rootNode->compile();
}

void Program::compile_finalize()
{
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
        std::string cmd = "clang " + program_name + ".ll libs.c -lm -lgc -o " +  program_name + ".out" + " -g";
        system(cmd.c_str());
    }
}


Value* ASTnodeCollection :: compile()
{
    for (AST* n : nodes){
        n->compile();
        if (BBended) break; 
    }
    return nullptr;
}

Value* EmptyStmt::compile()
{
    return nullptr;
}

Value* BinOp::compile() /* override */
{
    Value *l = left->compile();
    Value *r = nullptr; 
    Value * res ;
    // need to implement short-circuit
    if (op != "and" && op != "or") r = right->compile();
    bool real_ops = false; 

    if (left->lvalue) l = Builder.CreateLoad(l);
    if (right->lvalue) r = Builder.CreateLoad(r);
    

    // sign exted to real if necessary
    if (check_type(left->type,typeReal) || check_type(right->type,typeReal)) {
        // can also use Value type here for cmp 
        if (! check_type(left->type, typeReal))   
            l = Builder.CreateCast(Instruction::SIToFP, l, r64);
        if (! check_type(right->type, typeReal))  
            r = Builder.CreateCast(Instruction::SIToFP, r, r64);
        real_ops = true ;
    }
    switch( hashf(op.c_str()) ){
        case "+"_ :
            if (real_ops)   return Builder.CreateFAdd(l,r,"addtmp");
            else            return Builder.CreateAdd(l,r,"addtmp");
        case "-"_ : 
            if (real_ops)   return Builder.CreateSub(l,r,"addtmp");
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
            PHINode *phi = Builder.CreatePHI(i1, 2, "phi_and");
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
            PHINode *phi = Builder.CreatePHI(i1, 2, "phi_or");
            phi->addIncoming(c_i1(1),shortBB);
            phi->addIncoming(OP,CurrBB);

            return phi;
            }
        case "="_ : 
            if (real_ops)   res = Builder.CreateFCmpOEQ(l,r,"eqtmp");
            else            res =  Builder.CreateICmpEQ(l,r,"eqtmp");
            return res;

        case "<>"_ : 
            if (real_ops)   res = Builder.CreateFCmpONE(l,r,"neqtmp");
            else            res =  Builder.CreateICmpNE(l,r,"neqtmp");
            return res;

            
        /* "=,<>" :  results are boolean, 
            * if operants are both arithmetic then we compare values, 
                we have taken care of type casting
            * if they are of any other type we simply compare, 
                type checking takes care of type matching
        */
        case ">"_ :                     
            if (real_ops)   res =  Builder.CreateFCmpOGT(l,r);
            else            res =  Builder.CreateICmpSGT(l,r);
            return res;

        case "<"_ :
            if (real_ops)   res =  Builder.CreateFCmpOLT(l,r);
            else            res =  Builder.CreateICmpSLT(l,r);
            return res;

        case "<="_ : 
            if (real_ops)   res =  Builder.CreateFCmpOLE(l,r);
            else            res = Builder.CreateICmpSLE(l,r);
            return res;

        case ">="_ : 
            if (real_ops)   res = Builder.CreateFCmpOGE(l,r);
            else            res =  Builder.CreateICmpSGE(l,r);
            return res;


        // results must be arithmetic, some typecasting req
    }
    return nullptr; 
}

Value* UnOp::compile() /* override */
{
    Value* val = e->compile();
    switch( hashf(op.c_str()) ){
        case "+"_ : return val;
        case "-"_ : {
            if (e->lvalue) val = Builder.CreateLoad(val);
            if (check_type(e->type, typeReal))
                return Builder.CreateFSub(c_r64(0),val);
            else 
                return Builder.CreateSub(c_i32(0),val);

        }
        case "not"_ : return Builder.CreateNot(val,"nottmp");
        case "@"_ : 
            // not sure how to implement this 
            return Builder.CreateGEP(val, {0},"@");
             
    }
    return nullptr;
}

Value* Id::compile() /* override */
{
    CodeGenEntry* entry = ct.lookup(name);
    Value * val = entry->value;
    if (entry->pass_by == PASS_BY_REFERENCE)
        val = Builder.CreateLoad(val);
    return val; 
}

Value* Result::compile()
{
    return ct.lookup("result")->value;
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



Value* create_call(std::string fname, ASTnodeCollection *parameters){

    CodeGenEntry* f = (CodeGenEntry *) ct.lookup(fname); 
    Value* func = f->value;
    
    std::vector<PassMode> arguments = f->arguments;
    std::vector<Stype> sem_types = f->types;
    std::vector<Value*> param_values;
    
    if (parameters != nullptr) {

        auto param_iter = (parameters->nodes).begin();     

        for (size_t i = 0 ; i < arguments.size() ; i++){

            // cast to an Expr 
            Expr* p = (Expr*) *param_iter;
            Value* par_val  = p->compile();

            bool lval = p->lvalue;
            
            PassMode pass_by = arguments[i];
            Stype arg_type = sem_types[i];

            if (lval) par_val = Builder.CreateLoad(par_val);
            if (pass_by == PASS_BY_VALUE){
                if (arg_type->pointer_special_case(p->type))
                    par_val = Builder.CreatePointerCast(par_val, p->TypeConvert(arg_type), "bitcast");  
                else if (arg_type->kind == TYPE_REAL && p->type->kind == TYPE_INTEGER)
                    par_val = Builder.CreateCast(Instruction::SIToFP, par_val, r64);
            }
            else {
                if (typePointer(arg_type)->pointer_special_case(typePointer(p->type)))
                     par_val = Builder.CreatePointerCast(par_val, p->TypeConvert(arg_type), "bitcast");  
            }
            param_values.push_back(par_val);
            param_iter++; 
        }
    }    
    return Builder.CreateCall(func,param_values);
}

Value* CallFunc::compile() /* override */
{
    Value* ret = create_call(fname, parameters);
    return  ret;
}


Value* CallProc::compile() /* override */
{
    create_call(fname, parameters);
    return  nullptr;
}

Value* StringValue::compile() /* override */
{
    std::string pstr(strvalue);
    std::list<std::pair<std::string,std::string>> rep = {
        {"\\n" , "\n"} , {"\\t", "\t"}, {"\\r", "\r"} , {"\\\\", "\\"} , {"\\0", "\0"}, 
        {"\\'","'"}, {"\\\"", "\""}  };
    for (auto p : rep){
        ReplaceStringInPlace(pstr,p.first, p.second);
    }
    Value *ret = Builder.CreateGlobalStringPtr(pstr.c_str(),"str",0);
    return ret;

}

void pp(Value *a){
    a->getType()->print(llvm::errs(),true);
}

Value* ArrayAccess::compile() /* override */
{
    Value *index = pos->compile();
    Value *ptr = lval->compile();
    if (pos->lvalue)
        index = Builder.CreateLoad(index);
    index = Builder.CreateSExt(index,i64);
    Value *idx = Builder.CreateGEP(ptr, {index},"arrayIdx");
    Value *ret = Builder.CreateLoad(idx);
    
    return ret;
}

Value* Dereference::compile() /* override */
{
    Value *ptr = e->compile();
    return Builder.CreateLoad(ptr);
}

Value* Block::compile() /* override */
{
    // we compile each locals, calling the right method for its initial class
    locals->compile();
    body->compile();
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
    
    for (auto var : vars){ 
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
    BasicBlock* parentBB = Builder.GetInsertBlock();
    std::vector<Type*> param_types; 
    std::vector<Stype> sem_types; 
    std::vector<PassMode> param_pass;
    
    for (ParameterGroup* param : parameters){
        // by reference passing is just adding a pointer
        Stype t = param->type;
        if (param->pmode == PASS_BY_REFERENCE)
            if (param->type->kind == TYPE_ARRAY || param->type->kind == TYPE_IARRAY)
                param->pmode = PASS_BY_VALUE;

        if (param->pmode == PASS_BY_REFERENCE)
                t = typePointer(t);            
        for (std::string name : param->names){
             param_types.push_back(TypeConvert(t)) ;
             param_pass.push_back(param->pmode);
             sem_types.push_back(t);
             std::cout << "AA";
        }
    }
    FunctionType* Ftype =  FunctionType::get(TypeConvert(type),param_types,false);
    routine = Function::Create(Ftype,
        Function::ExternalLinkage,name,TheModule
    );
    // insert function in current scope 
    ct.insert(name,routine);
    CodeGenEntry* f = ct.lookup(name);
    f->arguments = param_pass;
    f->types = sem_types; 
    //create a new basic block 
    BasicBlock * BB = BasicBlock::Create(TheContext,"entry",routine);
    Builder.SetInsertPoint(BB);
    // create a new scope
    ct.openScope();
    auto param_iter = routine->arg_begin(); 
    for (ParameterGroup* param : parameters){
        // need to create aloca here 
        for (std::string name : param->names ){
            Stype t = param->type;
            if (param->pmode == PASS_BY_REFERENCE) 
                t = typePointer(t);           
            Value* value = Builder.CreateAlloca(TypeConvert(t), 0, name.c_str());
            Builder.CreateStore(param_iter,value);
            ct.insert(name,value,param->pmode);
            param_iter++;
        }
    }
    // type void means its a procedure 
    if (! type->equals(typeVoid)) {
        Value* value = Builder.CreateAlloca(TypeConvert(type), 0, "result");
        ct.insert("result",value);
    }
    body->compile(); 
    if (! BBended){   
        if (! type->equals(typeVoid)){
            Value* ret = ct.lookup("result")->value;
            Value* l = Builder.CreateLoad(ret);
            Builder.CreateRet(l);    
        }
        else Builder.CreateRetVoid();
    }
    else BBended = false; 
    ct.closeScope();
    Builder.SetInsertPoint(parentBB);
    return nullptr; 
}


Value* Assignment::compile() /* override */
{   // := 
    Value* l = lval->compile();
    Value* r = rval->compile();
    if (rval->lvalue)
        r = Builder.CreateLoad(r);
    Value* source; 
    if (lval->type_verify(typeReal) && rval->type_verify(typeInteger)){
        source = Builder.CreateCast(Instruction::SIToFP, r, r64);
    }
    else source = r; 
    if (lval->type->pointer_special_case(rval->type))
        source = Builder.CreatePointerCast(r, TypeConvert(lval->type), "bitcast");         

    Builder.CreateStore(source,l);
    return nullptr;
}

Value* IfThenElse::compile() /* override */
{
    Value *v = cond->compile();
    Value *cond = Builder.CreateICmpNE(v,c_i1(0), "if_cond");
    Function *TheFunction = Builder.GetInsertBlock()->getParent();
    BasicBlock *ThenBB = BasicBlock::Create(TheContext,"then", TheFunction);
    BasicBlock *AfterBB = BasicBlock::Create(TheContext,"endif", TheFunction);
    BasicBlock *ElseBB = BasicBlock::Create(TheContext,"else", TheFunction);
    Builder.CreateCondBr(cond,ThenBB,ElseBB);
    Builder.SetInsertPoint(ThenBB);
    st_then->compile();
    if (! BBended){   
        Builder.CreateBr(AfterBB);
    }
    else BBended = false; 
    Builder.SetInsertPoint(ElseBB);
    if (hasElse) st_else->compile();
    if (! BBended){   
        Builder.CreateBr(AfterBB);
    }
    else BBended = false; 
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
    if (! BBended){
        Builder.CreateBr(LoopBB);
        BBended = false; 
    }
    else BBended = false; 
    Builder.SetInsertPoint(AfterBB);
    return nullptr; 
}

Value* Label::compile() /* override */
{
    Function *TheFunction = Builder.GetInsertBlock()->getParent();
    BasicBlock * LabelBB  = BasicBlock::Create(TheContext, label, TheFunction);
    Builder.SetInsertPoint(LabelBB);
    ct.insert(label ,LabelBB);
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
    Value* ret = ct.lookup("result")->value;
    Value* l = Builder.CreateLoad(ret);
    Builder.CreateRet(l);    
    BBended = true; 
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
    Value* ptr = lval->compile();
    Builder.CreateCall(GC_Malloc,{ptr});
    return nullptr; 
}

Value* Dispose::compile() /* override */
{
    Value* ptr = lval->compile();
    Builder.CreateCall(GC_Free,ptr);
    return nullptr;
}

Value* DisposeArray::compile() /* override */
{
    /* Dispose should be some function call from gc library */ 
    // Value* ptr = lval->compile();
    // Builder.CreateCall(GC_Free,ptr);
    return nullptr;

}
