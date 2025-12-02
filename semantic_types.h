#ifndef SEMANTIC_TYPES_H
#define SEMANTIC_TYPES_H

#include <iostream>
#include <string>
using namespace std;

// ===========================================================
//  Representación de tipos básicos del lenguaje
// ===========================================================

class Type {
public:
    enum TType { NOTYPE, VOID, INT, BOOL, UNSIGNED, FLOAT, LONG, AUTO };
    static const char* type_names[8]; // antes eran solo 4

    TType ttype;

    Type() : ttype(NOTYPE) {}
    Type(TType tt) : ttype(tt) {}

    // Comparación de tipos
    bool match(Type* t) const {
        return this->ttype == t->ttype;
    }

    // Asignación de tipo básico desde string
    bool set_basic_type(const string& s) {
        TType tt = string_to_type(s);
        if (tt == NOTYPE) return false;
        ttype = tt;
        return true;
    }

    // Conversión string 
    static TType string_to_type(const string& s) {
        if (s == "int") return INT;
        if (s == "bool") return BOOL;
        if (s == "void") return VOID;
        // Nuevos
        if (s == "unsigned") return UNSIGNED;
        if (s == "float") return FLOAT;
        if (s == "long") return LONG;
        if (s == "auto") return AUTO;
        return NOTYPE;
    }


};

inline const char* Type::type_names[8] = { "notype", "void", "int", "bool", "unsigned", "float", "long", "auto" };
// antes eran solo 4

#endif // SEMANTIC_TYPES_H
