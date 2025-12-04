#ifndef AST_H
#define AST_H

#include <string>
#include <list>
#include <ostream>
#include <vector>
#include "semantic_types.h"

using namespace std;

class Visitor;
class VarDec;
class TypeVisitor;

// Operadores binarios soportados
enum BinaryOp { 
    PLUS_OP, 
    MINUS_OP, 
    MUL_OP, 
    DIV_OP,
    // POW_OP,
    LT_OP,
    GT_OP
};

// Clase abstracta Exp
class Exp {
public:
    virtual int  accept(Visitor* visitor) = 0;
    virtual ~Exp() = 0;  // Destructor puro → clase abstracta
    static string binopToChar(BinaryOp op);  // Conversión operador → string

    //Typechecker
    virtual Type* accept(TypeVisitor* visitor) = 0; // Para verificador de tipos
};

// Expresión binaria
class BinaryExp : public Exp {
public:
    Exp* left;
    Exp* right;
    BinaryOp op;
    int accept(Visitor* visitor);
    BinaryExp(Exp* l, Exp* r, BinaryOp op);
    ~BinaryExp();

    //Typechecker
    Type* accept(TypeVisitor* visitor);

};

// Expresión numérica
class NumberExp : public Exp {
public:
    int value;
    int accept(Visitor* visitor);
    NumberExp(int v);
    ~NumberExp();

    //Typechecker
    Type* accept(TypeVisitor* visitor);
};

// Expresión numérica
class IdExp : public Exp {
public:
    string value;
    int accept(Visitor* visitor);
    IdExp(string v);
    ~IdExp();

    //Typechecker
    Type* accept(TypeVisitor* visitor);
};

// Nuevo
class BoolExp : public Exp {
public:
    int valor;

    BoolExp(){};
    ~BoolExp(){};

    int accept(Visitor* visitor);

    //Typechecker
    Type* accept(TypeVisitor* visitor);
};
// Nuevo


// Base para sentencias
class Stm{
public:
    virtual int accept(Visitor* visitor) = 0;
    virtual ~Stm() = 0;

    //Typechecker
    virtual void accept(TypeVisitor* visitor) = 0;
};

class VarDec{
public:
    string tipo;
    vector<string> variables;
    vector<Exp*> inicializadores; // para soportar int x = 1;
    VarDec();
    int accept(Visitor* visitor);
    ~VarDec();

    //Typechecker
    void accept(TypeVisitor* visitor);
};


class Body{
public:
    list<Stm*> stmlist;
    list<VarDec*> vdlist;
    int accept(Visitor* visitor);
    Body();
    ~Body();

    //Typechecker
    void accept(TypeVisitor* visitor);
};


// Sentencias

class IfStm: public Stm {
public:
    Exp* condition;
    Body* then;
    Body* els;
    IfStm(Exp* condition, Body* then, Body* els);
    int accept(Visitor* visitor);
    ~IfStm(){};

    //Typechecker
    void accept(TypeVisitor* visitor);
};

class WhileStm: public Stm {
public:
    Exp* condition;
    Body* b;
    WhileStm(Exp* condition, Body* b);
    int accept(Visitor* visitor);
    ~WhileStm(){};

    //Typechecker
    void accept(TypeVisitor* visitor);
};



class AssignStm: public Stm {
public:
    string id;
    Exp* e;
    AssignStm(string, Exp*);
    ~AssignStm();
    int accept(Visitor* visitor);

    //Typechecker
    void accept(TypeVisitor* visitor);
};

class PrintStm: public Stm {
public:
    Exp* e;
    PrintStm(Exp*);
    ~PrintStm();
    int accept(Visitor* visitor);

    //Typechecker
    void accept(TypeVisitor* visitor);
};

// Nuevo
class ForStm: public Stm {
public:
    string id; // el i
    Exp* inicializacion;
    Exp* condicion;
    Stm* actualizacion;
    Body* cuerpo;
    ForStm(string id, Exp* inicializacion, Exp* condicion, Stm* actualizacion, Body* cuerpo);
    ~ForStm();
    int accept(Visitor* visitor);

    //Typechecker
    void accept(TypeVisitor* visitor);
};

// Nuevo

class FcallStm: public Stm {
public:
    Exp* fcall;
    FcallStm(Exp*);
    ~FcallStm();
    int accept(Visitor* visitor);

    //TypeChecker
    void accept(TypeVisitor* visitor);
};



class ReturnStm: public Stm {
public:
    Exp* e;
    ReturnStm(){};
    ~ReturnStm(){};
    int accept(Visitor* visitor);

    //Typechecker
    void accept(TypeVisitor* visitor);
};

class FcallExp: public Exp {
public:
    string nombre;
    vector<Exp*> argumentos;
    int accept(Visitor* visitor);
    FcallExp(){};
    ~FcallExp(){};

    //Typechecker
    Type* accept(TypeVisitor* visitor);
};




class FunDec{
public:
    string nombre;
    string tipo;
    Body* cuerpo;
    vector<string> Tparametros;
    vector<string> Nparametros;
    int accept(Visitor* visitor);
    FunDec(){};
    ~FunDec(){};

    //Typechecker
    void accept(TypeVisitor* visitor);
};

class Program{
public:
    list<VarDec*> vdlist;
    list<FunDec*> fdlist;
    Program(){};
    ~Program(){};
    int accept(Visitor* visitor);

    //Typechecker
    void accept(TypeVisitor* visitor);
};


#endif // AST_H
