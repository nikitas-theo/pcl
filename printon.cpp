#include "ast.hpp"
#include "symbol.hpp"
#include "helpers.hpp"


void ParameterGroup :: printOn(std::ostream &out) const /* override */
{ 
    if (pmode == PASS_BY_REFERENCE)
        out << "var:";
    out << names << " : " << type;
}


void ASTnodeCollection :: printOn(std::ostream &out) const
{
    out << nodes;
}

void EmptyStmt::printOn(std::ostream &out) const /* override */
{
    out << "NOOP";
}

void BinOp::printOn(std::ostream &out) const /* override */
{
    left->printOn(out); 
    out << " " << op << " ";
    right->printOn(out); 
}

void UnOp::printOn(std::ostream &out) const /* override */
{
    out << "(" << op << ")";
    e->printOn(out);
}

void Id::printOn(std::ostream &out) const /* override */
{
    out << "ID(" << name << ")";
}

void Result::printOn(std::ostream &out) const
{
    out << "result";
}

void Const::printOn(std::ostream &out) const /* override */
{
    switch(type->kind) {
        case TYPE_INTEGER : out << std::get<int>(val) ; break;
        case TYPE_BOOLEAN : out << std::get<bool>(val) ; break;
        case TYPE_CHAR : out << std::get<char>(val) ; break;
        case TYPE_REAL : out << std::get<double>(val) ; break;
        case TYPE_VOID : out << "Nil" ; break; 
    } 
}

void CallFunc::printOn(std::ostream &out) const /* override */
{
    out << fname << "(";
    if (parameters != nullptr) out << parameters->nodes;
    out  << ")";
}

void CallProc::printOn(std::ostream &out) const /* override */
{
    out << fname << "(";
    if (parameters != nullptr) out << parameters->nodes;
    out  << ")";
}

void StringValue::printOn(std::ostream &out) const /* override */
{
    out << "\"" << strvalue << "\"";
}

void ArrayAccess::printOn(std::ostream &out) const /* override */
{
    lval->printOn(out); out << "[";  pos->printOn(out); out << "]";
}

void Dereference::printOn(std::ostream &out) const /* override */
{
    e->printOn(out) ; out << "^";
}

void Block::printOn(std::ostream &out) const /* override */
{
    out << "BLOCK: "  << std::endl; 
    out << "LOCALS: " << locals->nodes << std::endl; 
    out << "BODY: " << body->nodes <<  std::endl; 
}

void Variable::printOn(std::ostream &out) const /* override */
{
    out << name << " : " << type;
}

void VarDef::printOn(std::ostream &out) const /* override */
{
    out << "VarDef(" << vars << ")";
}

void LabelDef::printOn(std::ostream &out) const /* override */
{
    out << "LabelDef(" << labels << ")";
}


void FunctionDef::printOn(std::ostream &out) const /* override */
{ 
    out << name << "(" << parameters << ") : " << type << std::endl ;
    // special case only forward defined 
    if (body != nullptr)
        body->printOn(out);
}

void Assignment::printOn(std::ostream &out) const /* override */
{
    lval->printOn(out);
    out << " := " ; 
    rval->printOn(out);
}

void IfThenElse::printOn(std::ostream &out) const /* override */
{ 
    out << "If("; cond->printOn(out); out << " then " << std::endl;
    st_then->printOn(out); out << std::endl ;
    if (hasElse) { out  << "else : " << std::endl;  st_else->printOn(out); }
}

void While::printOn(std::ostream &out) const /* override */
{
    out << "While("; cond->printOn(out); out << ") do" << std::endl;
    body->printOn(out);
} 

void Label::printOn(std::ostream &out) const /* override */
{
    out <<  label << ": "; target->printOn(out);
}

void GoTo::printOn(std::ostream &out) const /* override */
{
    out << "GOTO: " << label;
}

void ReturnStmt::printOn(std::ostream &out) const /* override */
{
    out << "RET";
}

void Init::printOn(std::ostream &out) const /* override */
{
    out << "Init"; lval->printOn(out);
}

void InitArray::printOn(std::ostream &out) const /* override */
{
    out << "InitArr"; lval->printOn(out);
}

void Dispose::printOn(std::ostream &out) const /* override */
{
    out << "Destroy";
    lval->printOn(out);
}

void DisposeArray::printOn(std::ostream &out) const /* override */
{
    out << "Destroy";
    lval->printOn(out);
}
