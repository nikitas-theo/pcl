#include "ast.hpp"
#include "symbol.hpp"
void EmptyStmt::printOn(std::ostream &out) const /* override */
{
    out << "empty";
}

void BinOp::printOn(std::ostream &out) const /* override */
{
    left->printOn(out); 
    out << " " << op;  
    out << " ";
    right->printOn(out); 
}

void UnOp::printOn(std::ostream &out) const /* override */
{
    out << "(" << op ;
    e->printOn(out); 
    out << ")";
}

void Id::printOn(std::ostream &out) const /* override */
{
    out << name;
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
    if (!parameters.list.empty()) parameters.printOn(out); 
    out << ")";
}

void CallProc::printOn(std::ostream &out) const /* override */
{
    out << fname << "("; 
    if (!parameters.list.empty()) parameters.printOn(out); 
    out << ")";
}

void String::printOn(std::ostream &out) const /* override */
{
    out << '\"' << s << '\"';
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
    out << "Block of: "  << std::endl; 
    out << "\t\tlocals: " ; locals.printOn(out); out << std::endl; 
    out << "\t\tbody: " ; body.printOn(out); out <<  std::endl; 
}

void Variable::printOn(std::ostream &out) const /* override */
{
    out << name << " : " << type;
}

void VarDef::printOn(std::ostream &out) const /* override */
{
    out << "VarDef :"; vars.printOn(out);
}

void LabelDef::printOn(std::ostream &out) const /* override */
{
    out << "LabelDef";  out << labels;
}

void FormalsGroup::printOn(std::ostream &out) const /* override */
{ 
    if (pass_by == PASS_BY_REFERENCE) out << "var: "; 
    out << formals;
    out <<  " : " ; out << type; 
}

void FunctionDef::printOn(std::ostream &out) const /* override */
{ 
    out << name ; parameters.printOn(out); out << " : " << type <<std::endl ;
    body->printOn(out);
}

void Declaration::printOn(std::ostream &out) const /* override */
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
    out <<  lbl << " : "; target->printOn(out);
}

void GoTo::printOn(std::ostream &out) const /* override */
{
    out << "LABEL : " << label;
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
