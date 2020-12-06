#ifndef __SYMBOL_COMPATIBLE__
#define __SYMBOL_COMPATIBLE__

#include <stdlib.h> 
#include <string>

SymbolEntry * newVariable        (std::string name, Type type);
SymbolEntry * newConstant        (std::string name, Type type, ...);
SymbolEntry * newFunction        (std::string name);
SymbolEntry * newParameter       (std::string name, Type type,
                                  PassMode mode, SymbolEntry * f);
SymbolEntry * lookupEntry        (std::string name, LookupType type,
                                  bool err);

#endif
