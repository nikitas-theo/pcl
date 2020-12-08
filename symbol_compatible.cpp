#include <stdarg.h>
#include "symbol/symbol.h"
#include "symbol_compatible.hpp"
#include <iostream>




SymbolEntry * newVariable        (std::string name, Type type)
{
    return newVariable(name.c_str(), type);
}

SymbolEntry * newConstant        (std::string name, Type type, ...)
{
    va_list args;
    va_start(args, type);
    return newConstant(name.c_str(), type, args);
}

SymbolEntry * newFunction        (std::string name)
{
    return newFunction(name.c_str());
}

SymbolEntry * newParameter       (std::string name, Type type, PassMode mode, SymbolEntry * f)
{
    return newParameter(name.c_str(), type, mode, f);
}

SymbolEntry * lookupEntry        (std::string name, LookupType type, bool err)
{
    return lookupEntry(name.c_str(), type, err);
}
