#ifndef AST_H
#define AST_H

#include <string>
#include <list>
#include <ostream>
#include <vector>
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
    POW_OP,
    LT_OP
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
    string type;
    list<string> vars;
    VarDec();
    int accept(Visitor* visitor);
    ~VarDec();

    //Typechecker
    void accept(TypeVisitor* visitor);
};


class Body{
public:
    list<Stm*> StmList;
    list<VarDec*> declarations;
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
};

class WhileStm: public Stm {
public:
    Exp* condition;
    Body* b;
    WhileStm(Exp* condition, Body* b);
    int accept(Visitor* visitor);
    ~WhileStm(){};
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
    ForStm(string, Exp*, Exp*, Stm*, Body*);
    ~ForStm();
    int accept(Visitor* visitor);
};
// Nuevo




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
    vector<string> Ptipos;
    vector<string> Pnombres;
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
