#include "typechecker.h"
#include <iostream>
#include <stdexcept>
using namespace std;


Type* NumberExp::accept(TypeVisitor* v) { return v->visit(this); }
Type* IdExp::accept(TypeVisitor* v) { return v->visit(this); }
Type* BinaryExp::accept(TypeVisitor* v) { return v->visit(this); }
Type* FcallExp::accept(TypeVisitor* v) { return v->visit(this); }
Type* BoolExp::accept(TypeVisitor* v) { return v->visit(this); }

void AssignStm::accept(TypeVisitor* v) { v->visit(this); }
void PrintStm::accept(TypeVisitor* v) { v->visit(this); }
void ReturnStm::accept(TypeVisitor* v) { v->visit(this); }
void IfStm::accept(TypeVisitor* v) { v->visit(this); } // agregado
void WhileStm::accept(TypeVisitor* v) { v->visit(this); } // agregado
void ForStm::accept(TypeVisitor* v) { v->visit(this); } // nuevo

void VarDec::accept(TypeVisitor* v) { v->visit(this); }
void FunDec::accept(TypeVisitor* v) { v->visit(this); }
void Body::accept(TypeVisitor* v) { v->visit(this); }
void Program::accept(TypeVisitor* v) { v->visit(this); }

// ===========================================================
//   Constructor del TypeChecker
// ===========================================================

TypeChecker::TypeChecker() {
    intType = new Type(Type::INT);
    boolType = new Type(Type::BOOL);
    voidType = new Type(Type::VOID);
    unsignedType = new Type(Type::UNSIGNED);
    floatType = new Type(Type::FLOAT);
    longType = new Type(Type::LONG);
}

// ===========================================================
//   Registrar funciones globales
// ===========================================================

void TypeChecker::add_function(FunDec* fd) {
    if (functions.find(fd->nombre) != functions.end()) {
        cerr << "Error: función '" << fd->nombre << "' ya fue declarada." << endl;
        exit(0);
    }

    Type* returnType = new Type();
    if (!returnType->set_basic_type(fd->tipo)) {
        cerr << "Error: tipo de retorno no válido en función '" << fd->nombre << "'." << endl;
        exit(0);
    }

    functions[fd->nombre] = returnType;
}

// ===========================================================
//   Método principal de verificación
// ===========================================================

void TypeChecker::typecheck(Program* program) {
    if (program) program->accept(this);
    cout << "Revisión exitosa" << endl;
}

// ===========================================================
//   Nivel superior: Programa y Bloque
// ===========================================================

void TypeChecker::visit(Program* p) {
    // Primero registrar funciones
    for (auto f : p->fdlist)
        add_function(f);

    env.add_level(); // para el nivel global
    // y ahora procesarlas
    for (auto v : p->vdlist)
        v->accept(this);    // se procesan las variables globales
    for (auto f : p->fdlist)
        f->accept(this);   // se procesan las funciones
    env.remove_level();
}

void TypeChecker::visit(Body* b) {
    env.add_level();
    for (auto v : b->vdlist) // declaraciones locales
        v->accept(this); 
    for (auto s : b->stmlist) // sentencias
        s->accept(this); 
    env.remove_level();
}

// ===========================================================
//   Declaraciones
// ===========================================================

void TypeChecker::visit(VarDec* v) {
    Type* t = new Type();
    if (!t->set_basic_type(v->tipo)) { // para validar que exista el tipo
        cerr << "Error: tipo de variable no válido." << endl;
        exit(0);
    }

    for (const auto& id : v->variables) {
        if (env.check(id)) { // error, ya declarada en este nivel
            cerr << "Error: variable '" << id << "' ya declarada." << endl;
            exit(0);
        }
        env.add_var(id, t); // cada variable de este VarDec se guarda con el mismo tipo (puntero Type*)
    }
}

void TypeChecker::visit(FunDec* f) { // chequeo de parametros y cuerpo
    env.add_level();
    for (size_t i = 0; i < f->Nparametros.size(); ++i) {
        Type* pt = new Type(); // por cada parametro se crea un Type nuevo
        if (!pt->set_basic_type(f->Tparametros[i])) { // valida si el tipo existe
            cerr << "Error: tipo de parámetro inválido en función '" << f->nombre << "'." << endl;
            exit(0);
        }
        env.add_var(f->Nparametros[i], pt); // Si no falla, se guarda
    }
    f->cuerpo->accept(this); // llama a TypeChecker de Body.
    // Alli se crean nuevos niveles para variables locales. Si no encuentra, pasa a buscar
    // en este nivel de parametros. Asi va expandiendo la busqueda hasta llegar al nivel global.
    env.remove_level();
}

// ===========================================================
//   Sentencias
// ===========================================================

void TypeChecker::visit(PrintStm* stm) {
    Type* t = stm->e->accept(this); // obtiene el tipo de la expresion a imprimir
    if (!(t->match(intType) || t->match(boolType))) { // valida que sea int o bool
        cerr << "Error: tipo inválido en print (solo int o bool)." << endl;
        exit(0);
    }
}

void TypeChecker::visit(AssignStm* stm) {
    if (!env.check(stm->id)) { // verifica que la variable exista en algun nivel
        cerr << "Error: variable '" << stm->id << "' no declarada." << endl;
        exit(0);
    }

    Type* varType = env.lookup(stm->id); // obtiene su tipo
    Type* expType = stm->e->accept(this); // obtiene el tipo de la expresion asignada

    if (!varType->match(expType)) { // valida que ambos tipos sean iguales
        cerr << "Error: tipos incompatibles en asignación a '" << stm->id << "'." << endl;
        exit(0);
    }
}

void TypeChecker::visit(ReturnStm* stm) {
    if (stm->e) { // si hay expresion de retorno, (si no existe, es void)
        Type* t = stm->e->accept(this); // obtiene su tipo
        if (!(t->match(intType) || t->match(boolType) || t->match(voidType))) { // valida que sea int, bool o void
            cerr << "Error: tipo inválido en return." << endl;
            exit(0);
        }
    }
}

// ===========================================================
//   Expresiones
// ===========================================================

Type* TypeChecker::visit(BinaryExp* e) {
    Type* left = e->left->accept(this); // obtiene tipo del operando izquierdo
    Type* right = e->right->accept(this); // obtiene tipo del operando derecho

    switch (e->op) {
        case PLUS_OP: 
        case MINUS_OP: 
        case MUL_OP: 
        case DIV_OP: 
            if (!(left->match(intType) && right->match(intType))) {
                cerr << "Error: operación aritmética requiere operandos int." << endl;
                exit(0);
            }
            return intType;
        /*case POW_OP:
            if (!(left->match(intType) && right->match(intType))) {
                cerr << "Error: operación aritmética requiere operandos int." << endl;
                exit(0);
            }
            return intType;
        */
        case GT_OP:
        case LT_OP:
            if (!(left->match(intType) && right->match(intType))) {
                cerr << "Error: operación aritmética requiere operandos int." << endl;
                exit(0);
            }
            return boolType;

        /*case AND_OP:
            if (!(left->match(boolType) && right->match(boolType))) {
                cerr << "Error: operación logica requiere operandos bool." << endl;
                exit(0);
            }
        return boolType;
        */

        default:
            cerr << "Error: operador binario no soportado." << endl;
            exit(0);
    }
}

Type* TypeChecker::visit(NumberExp* e) { return intType; }

Type* TypeChecker::visit(BoolExp* e) { return boolType; }

Type* TypeChecker::visit(IdExp* e) {
    if (!env.check(e->value)) { // verifica que la variable exista en algun nivel
        cerr << "Error: variable '" << e->value << "' no declarada." << endl;
        exit(0);
    }
    return env.lookup(e->value); // obtiene y devuelve su tipo al nodo padre
}

Type* TypeChecker::visit(FcallExp* e) {
    auto it = functions.find(e->nombre); // busca la funcion en el entorno de funciones
    if (it == functions.end()) {
        cerr << "Error: llamada a función no declarada '" << e->nombre << "'." << endl;
        exit(0);
    }
    return it->second; // devuelve el tipo de retorno de la funcion al nodo padre
}
