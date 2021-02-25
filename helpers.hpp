#pragma once
#include <iostream>
#include <list>
#include <string>

#include "types.hpp"

inline std::ostream& operator<<(std::ostream& os, const std::list<std::string>& l) {
    os << "[";
    for (auto t : l) {
        os << t;
        if (t != *(--l.end()))
            os << ",";
    }
    os << "]";
    return os;
}

template<typename T>
inline std::ostream& operator<<(std::ostream& os, const std::list<T>& l) {
    os << "{";
    for (T t : l) {
        if (std::is_pointer<T>::value) 
            os << *t ; 
        else 
            os << t;
        
        if (t != *(--l.end()))
            os << ", ";
    }
    os << "}";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const Stype t) {
    switch (t->kind)
    {
        case TYPE_ARRAY:
            os << "array[" << t->size << "] of " << t->refType;
            break;
        case TYPE_BOOLEAN:
            os << "bool";
            break;
        case TYPE_CHAR:
            os << "char";
            break;
        case TYPE_IARRAY:
            os << "array of " << t->refType;
            break;
        case TYPE_INTEGER:
            os << "int";
            break;
        case TYPE_POINTER:
            os << "pointer on " << t->refType;
            break;
        case TYPE_REAL:
            os << "real";
            break;
        case TYPE_VOID:
            os << "void";
            break;
    }
    return os;
}

inline std::ostream& operator<<(std::ostream& os, ParameterGroup& p) {
    if (p.pmode == PASS_BY_REFERENCE)
        os << "var:";
    os << p.names << " : " << p.type;
    return os;
}

template<typename T>
std::ostream & _error(std::ostream & o, T&& arg) { 
   return o << std::forward<T>(arg); 
}

template<typename T, typename ...Ts>
std::ostream & _error(std::ostream & o, T&& arg, Ts&&... args)
{
   o << std::forward<T>(arg);
   return _error(o, std::forward<Ts>(args)...);
}
