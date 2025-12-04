#ifndef TYPECHECKER_H
#define TYPECHECKER_H

#include <unordered_map>
#include <string>
#include "ast.h"
#include "environment.h"
#include "semantic_types.h"

using namespace std;

class BinaryExp;
class NumberExp;
class Program;
class PrintStm;
class AssignStm;
class IfStm;
class whileStm;
//class ForStm; // nuevo
class FunDec;
class ReturnStm;
class Body;
class VarDec;
class FcallExp;
class BoolExp;

class TypeVisitor {
public:

    // --- Nodos de nivel superior ---
    virtual void visit(Program* p) = 0;
    virtual void visit(Body* b) = 0;
    virtual void visit(VarDec* v) = 0;
    virtual void visit(FunDec* f) = 0;

    // --- Sentencias ---
    virtual void visit(PrintStm* stm) = 0;
    virtual void visit(AssignStm* stm) = 0;
    virtual void visit(ReturnStm* stm) = 0;
    virtual void visit(IfStm* stm) = 0; // agregado
    virtual void visit(WhileStm* stm) = 0; // agregado
    //virtual void visit(ForStm* stm) = 0; // nuevo


    // --- Expresiones ---
    virtual Type* visit(BinaryExp* e) = 0;
    virtual Type* visit(NumberExp* e) = 0;
    virtual Type* visit(IdExp* e) = 0;
    virtual Type* visit(BoolExp* e) = 0;
    virtual Type* visit(FcallExp* e) = 0;
};



// ──────────────────────────────────────────────
//   CLASE TYPECHECKER
// ──────────────────────────────────────────────

class TypeChecker : public TypeVisitor {
private:
    Environment<Type*> env;                 // Entorno de variables y sus tipos
    unordered_map<string, Type*> functions; // Entorno de funciones

    // Tipos básicos
    Type* intType;
    Type* boolType;
    Type* voidType;
    Type* unsignedType;
    Type* floatType;
    Type* longType;

    // Registro de funciones
    void add_function(FunDec* fd);

public:
    TypeChecker();

    // Método principal de verificación
    void typecheck(Program* program);

    // --- Visitas de alto nivel ---
    void visit(Program* p) override;
    void visit(Body* b) override;
    void visit(VarDec* v) override;
    void visit(FunDec* f) override;

    // --- Sentencias ---
    void visit(PrintStm* stm) override;
    void visit(AssignStm* stm) override;
    void visit(ReturnStm* stm) override;
    void visit(IfStm* stm) override; // agregado
    void visit(WhileStm* stm) override; // agregado
    //void visit(ForStm* stm) override; // nuevo

    // --- Expresiones ---
    Type* visit(BinaryExp* e) override;
    Type* visit(NumberExp* e) override;
    Type* visit(IdExp* e) override;
    Type* visit(FcallExp* e) override;
    Type* visit(BoolExp* e) override;
};

#endif // TYPECHECKER_H
