#pragma once
#include "vector.h"

#define OPCODE_COUNT 28

#define INS_NULL -1

#define INS_PUT 0
#define INS_MOV 1
#define INS_MOO 2
#define INS_ADD 3
#define INS_ADDN 4
#define INS_SUB 5
#define INS_SUBN 6
#define INS_CMP 7
#define INS_CMPN 8
#define INS_EQU 9
#define INS_EQUN 10
#define INS_INV 11
#define INS_JMP 12
#define INS_JFZ 13
#define INS_JNZ 14
#define INS_HLT 15
#define INS_CALL 16
#define INS_RET 17
#define INS_PUSH 18
#define INS_PULL 19
#define INS_MTP 20
#define INS_MFP 21
#define INS_PRT 22
#define INS_PRTP 23
#define INS_INP 24
#define INS_PIN 25
#define INS_RAW 26
#define INS_GUAD 27

static char opcodes[OPCODE_COUNT][5] = {
    "put",
    "mov",
    "moo",
    "add",
    "addn",
    "sub",
    "subn",
    "cmp",
    "cmpn",
    "equ",
    "equn",
    "inv",
    "jmp",
    "jfz",
    "jnz",
    "hlt",
    "call",
    "ret",
    "push",
    "pull",
    "mtp",
    "mfp",
    "prt",
    "prtp",
    "inp",
    "pin",
    "raw",
    "guad",
};

typedef struct pseudoins
{
    int op;
    char* args;
    int line;
} pseudoins_t;

typedef struct ins
{
    int op;
    int args[99];
    int jumpedTo; // for compounding
} ins_t;

ins_t instructions[9999];
int numberOfInstructions;
void parse();


// ----------------------PRIVATE-------------------------
vector_t* jumpedToInstructions;

void parseStartAt();
void parseMacros();
void parseSections();

// just a bunch of helper functions
void validateOp(int op, int thenNext);
void validateLexeme(char* lexeme, int thenNext);
void validateStrict(int op, char* lexeme, int thenNext);
void getToken();
int peekNext();
void addPseudoIns(int op, char* args);
int* permintptr(int);
int strToInt(char* str);
int getInsOp();
void collapsePseudos();
int evalForwardOrBackward(char*);