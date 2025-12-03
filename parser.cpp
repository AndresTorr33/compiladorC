#include<iostream>
#include "token.h"
#include "scanner.h"
#include "ast.h"
#include "parser.h"

using namespace std;

// =============================
// Métodos de la clase Parser
// =============================

Parser::Parser(Scanner* sc) : scanner(sc) {
    previous = nullptr;
    current = scanner->nextToken();
    if (current->type == Token::ERR) {
        throw runtime_error("Error léxico");
    }
}

bool Parser::match(Token::Type ttype) {
    if (check(ttype)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(Token::Type ttype) {
    if (isAtEnd()) return false;
    return current->type == ttype;
}

bool Parser::advance() {
    if (!isAtEnd()) {
        Token* temp = current;
        if (previous) delete previous;
        current = scanner->nextToken();
        previous = temp;

        if (check(Token::ERR)) {
            throw runtime_error("Error lexico");
        }
        return true;
    }
    return false;
}

bool Parser::isAtEnd() {
    return (current->type == Token::END);
}


// =============================
// Reglas gramaticales
// =============================

Program* Parser::parseProgram() {
    Program* p = new Program();
    // para Auto
    if (check(Token::AUTO)) {
        VarDec* vd = parseAutoDec();
        p->vdlist.push_back(vd);
        return;
    }

    if(check(Token::ID)) {
        if(check(Token::ID)) {
            if(check(Token::LPAREN)){
                p->fdlist.push_back(parseFunDec());
                while(check(Token::FUN)){
                    p->fdlist.push_back(parseFunDec());
                }
            }
            else{
                p->vdlist.push_back(parseVarDec());
                while(match(Token::SEMICOL)) {
                    if(check(Token::ID)) {
                        p->vdlist.push_back(parseVarDec());
                    }
                }
            }
        }
        
    }

    if(check(Token::FUN)) {
        
        }
    cout << "Parser exitoso" << endl;
    return p;
}

VarDec* Parser::parseAutoDec() {
    VarDec* vd = new VarDec();
    // auto x = CExp, y = CExp...
    match(Token::AUTO); 
    match(Token::ID);
    vd->variables.push_back(previous->text);
    match(Token::ASSIGN);
    vd->inicializadores.push_back(parseCE());

    while (match(Token::COMA)) {
        match(Token::ID);
        vd->variables.push_back(previous->text);
        match(Token::ASSIGN);
        vd->inicializadores.push_back(parseCE());
    }

    match(Token::SEMICOL);

    return vd;
}

VarDec* Parser::parseVarDec(){
    VarDec* vd = new VarDec();
    
    match(Token::ID);
    vd->tipo = previous->text;
    match(Token::ID);
    vd->variables.push_back(previous->text);
    while(match(Token::COMA)) {
        match(Token::ID);
        vd->variables.push_back(previous->text);
    }
    return vd;
}

FunDec *Parser::parseFunDec() {
    FunDec* fd = new FunDec();

    match(Token::ID);
    fd->tipo = previous->text;
    match(Token::ID);
    fd->nombre = previous->text;
    match(Token::LPAREN);
    if(check(Token::ID)) { // parametros
        while(match(Token::ID)) {
            fd->Tparametros.push_back(previous->text);
            match(Token::ID);
            fd->Nparametros.push_back(previous->text);
            match(Token::COMA);
        }
    }
    match(Token::RPAREN);
    match(Token::LBRACE);
    fd->cuerpo = parseBody();
    match(Token::RBRACE);

    return fd;
}



Body* Parser::parseBody(){
    Body* b = new Body();

    if(check(Token::ID)) { // si hay declaraciones de variables
        b->vdlist.push_back(parseVarDec());
        while(match(Token::SEMICOL)) {
            if(check(Token::ID)) {
                b->vdlist.push_back(parseVarDec());
            }
        }
    }
    b->stmlist.push_back(parseStm()); // minimo una sentencia
    while(match(Token::SEMICOL)) {
        b->stmlist.push_back(parseStm());
    }

    return b;
}

Stm* Parser::parseStm() {
    Stm* a;
    Exp* e;
    string variable;
    Body* tb = nullptr;
    Body* fb = nullptr;
    if(match(Token::ID)){
        variable = previous->text;
        match(Token::ASSIGN);
        e = parseCE();
        return new AssignStm(variable,e);
    }
    else if(match(Token::PRINT)){
        match(Token::LPAREN);
        match(Token::COMILLAS);
        match(Token::PORCENTAJE);
        match(Token::ID);
        match(Token::COMILLAS);
        match(Token::COMA);
        e = parseCE();
        match(Token::RPAREN);
        return new PrintStm(e);
    }
    else if(match(Token::RETURN)) { // dejado intacto
        ReturnStm* r  = new ReturnStm();
        match(Token::LPAREN);
        r->e = parseCE();
        match(Token::RPAREN);
        return r;
    }
else if (match(Token::IF)) {
        match(Token::LPAREN);
        e = parseCE();
        match(Token::RPAREN);
        match(Token::LBRACE);
        tb = parseBody();
        match(Token::RBRACE);
        if (match(Token::ELSE)) {
            match(Token::LBRACE);
            fb = parseBody();
            match(Token::RBRACE);
        }

        a = new IfStm(e, tb, fb);
    }
    else if (match(Token::WHILE)) {
        match(Token::LPAREN);
        e = parseCE();
        match(Token::RPAREN);
        match(Token::LBRACE);
        tb = parseBody();
        match(Token::RBRACE);

        a = new WhileStm(e, tb);
    }
    else if (match(Token::FOR)) {
        // for(int i = 0; i <10, i=i+1)
        match(Token::LPAREN);
        match(Token::ID); // tipo
        string tipo = previous->text;
        match(Token::ID); // Inicializacion
        string i = previous->text; // guardando el i
        match(Token::ASSIGN);
        Exp* e1 = parseCE(); // parsea el 0
        match(Token::SEMICOL);
        Exp* e2 = parseCE(); // Condicion
        match(Token::SEMICOL);
        Stm* s = parseStm(); // parsea el i = i+1
        match(Token::RPAREN);
        match(Token::LBRACE);
        Body* body = parseBody();
        match(Token::RBRACE);

        a = new ForStm(i, e1, e2, s, tb);
    }
    else{
        throw runtime_error("Error sintáctico");
    }
    return a;
}

Exp* Parser::parseCE() {
    Exp* l = parseE();
    if (match(Token::LT)) {
        BinaryOp op = LT_OP;
        Exp* r = parseE();
        l = new BinaryExp(l, r, op);
    }
    else if (match(Token::GT)) {
        BinaryOp op = GT_OP;
        Exp* r = parseE();
        l = new BinaryExp(l, r, op);
        throw runtime_error("Operador '>' no implementado");
    }
    return l;
}


Exp* Parser::parseE() {
    Exp* l = parseT();
    while (match(Token::PLUS) || match(Token::MINUS)) {
        BinaryOp op;
        if (previous->type == Token::PLUS){
            op = PLUS_OP;
        }
        else{
            op = MINUS_OP;
        }
        Exp* r = parseT();
        l = new BinaryExp(l, r, op);
    }
    return l;
}


Exp* Parser::parseT() {
    Exp* l = parseF();
    while (match(Token::MUL) || match(Token::DIV)) {
        BinaryOp op;
        if (previous->type == Token::MUL){
            op = MUL_OP;
        }
        else{
            op = DIV_OP;
        }
        Exp* r = parseF();
        l = new BinaryExp(l, r, op);
    }
    return l;
}

/*
Exp* Parser::parseT() {
    Exp* l = parseF();
    if (match(Token::POW)) {
        BinaryOp op = POW_OP;
        Exp* r = parseF();
        l = new BinaryExp(l, r, op);
    }
    
    return l;
}
*/

Exp* Parser::parseF() {
    Exp* e;
    string nom;
    if (match(Token::NUM)) {
        return new NumberExp(stoi(previous->text)); //stoi: string to int
    }
    else if (match(Token::TRUE)) {
        return new NumberExp(1);
    }
    else if (match(Token::FALSE)) {
        return new NumberExp(0);
    }
    else if (match(Token::LPAREN)){
        e = parseCE();
        match(Token::RPAREN);
        return e;
    }
    else if (match(Token::ID)) {
        nom = previous->text;
        if(check(Token::LPAREN)) {
            match(Token::LPAREN);
            FcallExp* fcall = new FcallExp();
            fcall->nombre = nom;
            fcall->argumentos.push_back(parseCE());
            while(match(Token::COMA)) {
                fcall->argumentos.push_back(parseCE());
            }
            match(Token::RPAREN);
            return fcall;
        }
        else {
            return new IdExp(nom);
            }
    }
    else {
        throw runtime_error("Error sintáctico");
    }
}
