#include "asm.h"
#include "parser.h"
#include "stdio.h"
#include "brain.h"

/*

MY BRAIN HURTS AFTER IMPLEMENTING POINTERS
AHHHHHHHHHHHHHHHHHHHHHH
I REALLY HOPE IT WORKS FIRST TRY CAUSE OTHERWISE IDK WHAT I'M GOING TO DO
CROSSING MY FINGERS

i was supposed to change smth in the code before running it, but I forgot what...
hope it still works

OOOH. I remember, it was removing MFP,PIN, OUTP, and MTP from the instruction list.

*/

int bfi = 0;
char bf[99999999];

// tells us how many registers are used for things
// address 0 is reserved to find our way back to the start
// all the reserved registers
int regA = 1;
int regB = 2;
int regC = 3;
int buffer = 4;
int ZERO = 5; // constant zero address. used for STP

int buffer2 = 6;

int programCounter = 7;
int currentInstruction = 8;
int instructionCheck = 9;

int shadowAddressBuffer = 10;

int dataPointer = 0;

// allows global variables to take up 255 registers

int adrOffset = 12; // we have to add the stack pointer and stack size

int stackPointer = 200;

// the shadow offset
int shadowOffset = 256;

int lineLength = 0;
int noCommentLength = 60;

void moveToInAddress(int adr, int isPtr);

void addCommand(char cmd)
{
    lineLength++;
    bf[bfi] = cmd;
    bfi++;
    if (bfi % noCommentLength == noCommentLength - 1)
        addCommand('\n');
}

void addCommands(char* cmds)
{
    int idx = 0;
    while (cmds[idx] !='\0')
    {
        lineLength++;
        addCommand(cmds[idx]);
        idx++;
    }
}

void repeatCommand(char cmd, int times)
{
    for (int i = 0; i < times; i++)
    {
        addCommand(cmd);
    }
}

void moveBy(int num)
{
    if (num > 0)
        repeatCommand('>', num);
    else
        repeatCommand('<', -num);

    dataPointer += num;
}

void moveFromTo(int from, int to)
{
    int toAdrTo = to - from;
    moveBy(toAdrTo);
}

void moveTo(int adr, int isPtr)
{
    if (isPtr)
    {
        moveToInAddress(adr, 0);
    }
    else
    {
        moveFromTo(dataPointer, adr);
    }
}

void toStart()
{
    //+[-<+]-
    addCommands("+[-<+]-"); // what?
    dataPointer = 0;
}

void put(int adr, int val, int isPtr)
{
    moveTo(adr, isPtr);
    addCommand('[');
        addCommand('-');
    addCommand(']');
    repeatCommand('+', val);
    if (isPtr)
        toStart();
}

void transfer(int adrFrom, int adrTo, int isAdrFromPointer, int isAdrToPointer)
{
    put(adrTo, 0, isAdrToPointer);
    moveTo(adrFrom, isAdrFromPointer);
    addCommand('[');
        addCommand('-');

        if (isAdrFromPointer) toStart();

        moveTo(adrTo, isAdrToPointer);
        addCommand('+');

        if (isAdrToPointer) toStart();
        
        moveTo(adrFrom, isAdrFromPointer);
    addCommand(']');

    if (isAdrFromPointer) toStart();
}

void copy(int adrFrom, int adrTo, int isAdrFromPtr, int isAdrToPtr)
{
    put(buffer, 0, 0);
    put(adrTo, 0, isAdrToPtr);

    moveTo(adrFrom, isAdrFromPtr);
    addCommand('[');
        addCommand('-');

        if (isAdrFromPtr) toStart();

        moveTo(adrTo, isAdrToPtr);
        addCommand('+');

        if (isAdrToPtr) toStart();
        
        moveTo(buffer, 0);
        addCommand('+');

        moveTo(adrFrom, isAdrFromPtr);
    addCommand(']');

    if (isAdrFromPtr) toStart();

    transfer(buffer, adrFrom, 0, isAdrFromPtr);
}

void input(int adr, int isPtr)
{
    moveTo(adr, isPtr);
    addCommand(',');
}

void output(int adr, int isPtr)
{
    moveTo(adr, isPtr);
    addCommand('.');
}

void add(int adr1, int adr2, int isAdr1Ptr, int isAdr2Ptr)
{   
    copy(adr1, regC, isAdr1Ptr, 0);
    copy(adr2, regB, isAdr2Ptr, 0);

    moveTo(regB, 0);
    addCommand('[');
        addCommand('-');
        moveTo(regC, 0);
        addCommand('+');
        moveTo(regB, 0);
    addCommand(']');
    toStart();
}

void sub(int adr1, int adr2, int isAdr1Ptr, int isAdr2Ptr)
{
    copy(adr1, regC, isAdr1Ptr, 0);
    copy(adr2, regB, isAdr2Ptr, 0);

    moveTo(regB, 0);
    addCommand('[');
        addCommand('-');
        moveTo(regC, 0);
        addCommand('-');
        moveTo(regB, 0);
    addCommand(']');
    toStart();
}

void addn(int adr, int num, int isPtr)
{
    moveTo(adr, isPtr);
    repeatCommand('+', num);
}

void subn(int adr, int num, int isPtr)
{
    moveTo(adr, isPtr);
    repeatCommand('-', num);
}

void compareIfGreater(int adr1, int adr2, int isAdr1Ptr, int isAdr2Ptr)
{
    copy(adr1, regA, isAdr1Ptr, 0);
    copy(adr2, regB, isAdr2Ptr, 0);
    transfer(regA, regC, 0, 0);
    put(buffer, 1, 0);
    moveTo(regC, 0);
    addCommand('+'); // in case of zeros
    moveTo(regB, 0);
    addCommand('+'); // in case of zeros...again

    // magic alg: [>-[<]>-]
    addCommand('[');
        addCommand('>');
        addCommand('-');
        addCommand('[');
            addCommand('<');
        addCommand(']');
        addCommand('>');
        addCommand('-');
    addCommand(']');
    // now we need to put the data pointer back to a known position
    /*
    this is old code but it was smart so im keeping it
    // we do this by abusing the ZERO data cell, by walking right until we hit it.
    addCommand('>');
    addCommand('[');
        addCommand('>');
    addCommand(']');
    addCommand('<');
    // we've made it back to the buffer yay!
    */
    // this is even smarter tho. lol
    toStart();
}

void invert(int adr, int isPtr)
{
    const int y = 2; // lowest value is 2
    moveTo(adr, isPtr);
    addCommand('[');
        addCommands("[-]");
        repeatCommand('-', y);
        toStart();
        moveTo(ZERO, 0);
    addCommand(']');

    toStart();
    moveTo(adr, isPtr);
    repeatCommand('+', y);
    toStart();
}

void equals(int adr1, int adr2, int isAdr1Ptr, int isAdr2Ptr)
{
    sub(adr1, adr2, isAdr1Ptr, isAdr2Ptr);
    invert(regC, 0);
}

void equalsn(int adr, int num, int isPtr)
{
    copy(adr, regC, isPtr, 0);
    subn(regC, num, 0);
    invert(regC, 0);
}

void moveToInAddress(int adr, int isPtr)
{
    // same as copy code but using shadowAddressBuffer as the buffer
    put(shadowAddressBuffer, 0, 0);
    put(shadowOffset, 0, 0);

    moveTo(adr, isPtr);
    addCommand('[');
        addCommand('-');

        if (isPtr) toStart();

        moveTo(shadowOffset, 0);
        addCommand('+');
        
        moveTo(shadowAddressBuffer, 0);
        addCommand('+');

        moveTo(adr, isPtr);
    addCommand(']');

    if (isPtr) toStart();

    transfer(shadowAddressBuffer, adr, 0, isPtr);

    //copy(adr, shadowOffset, isPtr, 0);
    moveTo(shadowOffset, 0);
    addCommands("[->[-]<[>+<-]>]");
    moveBy(adrOffset-shadowOffset);
}

void moveFromPtr(int ptr, int adrCopyTo, int isPtrPtr, int isAdrPtr)
{
    put(adrCopyTo, 0, isAdrPtr);
    put(buffer, 0, 0);
    moveToInAddress(ptr, isPtrPtr); // after this call, dataPointer is no longer valid

    addCommand('[');
        addCommand('-');
        toStart(); // now data pointer is valid again
        moveTo(buffer, 0);
        addCommand('+');
        moveTo(adrCopyTo, isAdrPtr);
        addCommand('+');
        if (isAdrPtr) toStart();
        moveToInAddress(ptr, isPtrPtr);
    addCommand(']');

    toStart();

    moveTo(buffer, 0);
    addCommand('[');
        addCommand('-');
        moveToInAddress(ptr, isPtrPtr);
        addCommand('+');
        toStart();
        moveTo(buffer, 0);
    addCommand(']');
}

void moveIntoPtr(int adr, int adrPtr, int isAdrPtr, int isPtrPtr)
{
    put(buffer2, 0, 0);
    moveToInAddress(adrPtr, isPtrPtr);
    addCommands("[-]");
    toStart();

    moveTo(adr, isAdrPtr);
    addCommand('[');
        addCommand('-');
        if (isAdrPtr) toStart();
        moveToInAddress(adrPtr, isPtrPtr);
        addCommand('+');
        toStart();
        moveTo(buffer2, 0);
        addCommand('+');
        moveTo(adr, isAdrPtr);
    addCommand(']');

    if (isAdrPtr) toStart();

    transfer(buffer2, adr, 0, isAdrPtr);
}

void jump(int ins)
{
    // do jump logic
    put(programCounter, ins, 0);
}

void pjump(int reg, int isPtr)
{
    copy(reg, programCounter, isPtr, 0);
}

void jumpIfZero(int ins, int adr, int isPtr)
{
    copy(adr, buffer2, isPtr, 0);
    invert(adr, isPtr); // we invert it // this is a sub comment, commenting on how bad my comment was lol
    moveTo(adr, isPtr);
    addCommand('[');
        jump(ins);
        moveTo(ZERO, 0);
    addCommand(']');
    toStart();
    transfer(buffer2, adr, 0, isPtr);
}

void jumpNotZero(int ins, int adr, int isPtr)
{
    moveTo(adr, isPtr);
    addCommand('[');
        if (isPtr) toStart();
        jump(ins);
        moveTo(ZERO, 0);
    addCommand(']');
    toStart(); // when in doubt, call toStart // im reading this comment in the future and its funny lol
}

void push(int* adrs, int* isPtr)
{
    int i = 0;
    while (adrs[i] != -1)
    {
        addn(stackPointer, 1, 0);
        // moveIntoPtr(adrs[i] + adrOffset, stackPointer, isPtr[i], 0);
        copy(adrs[i] + adrOffset, stackPointer, isPtr[i], 1);
        i++;
    }
}

void pull(int* adrs, int* isPtr)
{
    int i = 0;
    while (adrs[i] != -1)
    {
        //moveFromPtr(stackPointer, adrs[i] + adrOffset, 0, isPtr[i]);
        copy(stackPointer, adrs[i] + adrOffset, 1, isPtr[i]);
        subn(stackPointer, 1, 0);
        i++;
    }
}

void compareIfGreaterN(int adr1, int num, int isPtr)
{
    copy(adr1, regA, isPtr, 0);
    put (regB, num, 0);
    transfer(regA, regC, 0, 0);
    put(buffer, 1, 0);
    moveTo(regC, 0);
    addCommand('+'); // in case of zeros
    moveTo(regB, 0);
    addCommand('+'); // in case of zeros...again
    // magic alg: [>-[<]>-]
    addCommand('[');
        addCommand('>');
        addCommand('-');
        addCommand('[');
            addCommand('<');
        addCommand(']');
        addCommand('>');
        addCommand('-');
    addCommand(']');
    // now we need to put the data pointer back to a known position
    /*
    this is old code but it was smart so im keeping it
    // we do this by abusing the ZERO data cell, by walking right until we hit it.
    addCommand('>');
    addCommand('[');
        addCommand('>');
    addCommand(']');
    addCommand('<');
    // we've made it back to the buffer yay!
    */
    // this is even smarter tho. lol
    toStart();
}

void ret()
{
    int adrs[2] = { programCounter - adrOffset,-1 };
    int isPtr = 0;
    pull(adrs, &isPtr);
    sub(programCounter, buffer, 0, 0);
    
    compareIfGreaterN(programCounter, 127, 0);
    moveTo(regC, 0);
    addCommand('[');
        // add number of instructions
        addn(programCounter, numberOfInstructions, 0);
        moveTo(ZERO, 0);
    addCommand(']');
}

void call(int jumpToIns)
{
    int args[2] = { currentInstruction - adrOffset, -1 };
    int isPtr = 0;
    addn(currentInstruction, 1, 0);
    push(args, &isPtr);
    subn(currentInstruction, 1, 0);
    jump(jumpToIns);
}

void pcall(int reg, int isPtr)
{
    int args[2] = { currentInstruction - adrOffset, -1 };
    int isPtr2 = 0;
    addn(currentInstruction, 1, 0);
    push(args, &isPtr2);
    subn(currentInstruction, 1, 0);
    pjump(reg, isPtr);
}

char BRAINFUCK_INSTRUCTIONS[9] = "+-><[].,";
void raw(int* args)
{
    int i = 0;
    while (args[i] != -1)
    {
        addCommand(BRAINFUCK_INSTRUCTIONS[args[i]]);
        i++;
    }
}

void addBoilerPlateHeader()
{
    moveTo(currentInstruction, 0);
    addCommand('+');

    // check if currentInstruction equals programCounter
    copy(currentInstruction, instructionCheck, 0, 0);

    copy(programCounter, buffer2, 0, 0);
    moveTo(programCounter, 0);
    addCommand('[');
        addCommand('-');
        moveTo(instructionCheck, 0);
        addCommand('-');
        moveTo(programCounter, 0);
    addCommand(']');
    // we dont have to move to start because our loop starts where we end off
    transfer(buffer2, programCounter, 0, 0);
    
    invert(instructionCheck, 0);
    moveTo(instructionCheck, 0);
    addCommand('[');
        // increment program counter
        moveTo(programCounter, 0);
        addCommand('+');

}

void addBoilerPlateFooter(int inc)
{
    toStart();
    moveTo(ZERO, 0);
    addCommand(']');
    toStart();


    //moveTo(currentInstruction);
    //repeatCommand('+', inc);
}

void assemble()
{
    int i = 1;
    int wasPreviousJumpedTo = 1;
    int compoundStack = 0;

    addCommand('-'); // set first cell to 255, so that we can identify it

    // first instruction is: start at ...
    // this is stored as a jump command, so we set the program counter to the first arg
    // of the first instruction
    jump(instructions[0].args[0]); 
    put(stackPointer, stackPointer - adrOffset, 0);
    addCommand('[');

    toStart();
    put(currentInstruction, 0, 0);

    while (instructions[i].op != INS_NULL)
    {
        /*
        if (!wasPreviousJumpedTo && instructions[i].jumpedTo)
        {
            addBoilerPlateFooter(compoundStack);
            wasPreviousJumpedTo = 1;
            compoundStack = 0;
            addBoilerPlateHeader();
        }
        else
        {
            wasPreviousJumpedTo = 0;
            compoundStack++;
        }*/

        addBoilerPlateHeader();

        int* args = instructions[i].args;
        int* isPtr = instructions[i].isPtr;

        switch (instructions[i].op)
        {
            case INS_PUT:
                put(args[0] + adrOffset, args[1], isPtr[0]);
                break;
            case INS_MOV:
                copy(args[0] + adrOffset, args[1] + adrOffset, isPtr[0], isPtr[1]);
                break;
            case INS_MOO:
                copy(regC, args[0] + adrOffset, 0, isPtr[0]);
                break;
            case INS_ADD:
                add(args[0] + adrOffset, args[1] + adrOffset, isPtr[0], isPtr[1]);
                if (args[2] != -1)
                {
                    copy(regC, args[2] + adrOffset, 0, isPtr[2]);
                }
                break;
            case INS_ADDN:
                addn(args[0] + adrOffset, args[1], isPtr[0]);
                break;
            case INS_SUB:
                sub(args[0] + adrOffset, args[1] + adrOffset, isPtr[0], isPtr[1]);
                if (args[2] != -1)
                {
                    copy(regC, args[2] + adrOffset, 0, isPtr[2]);
                }
                break;
            case INS_SUBN:
                subn(args[0] + adrOffset, args[1], isPtr[0]);
                break;
            case INS_CMP:
                compareIfGreater(args[0] + adrOffset, args[1] + adrOffset, isPtr[0], isPtr[1]);
                break;
            case INS_CMPN:
                compareIfGreaterN(args[0] + adrOffset, args[1], isPtr[0]);
                break;
            case INS_EQU:
                equals(args[0] + adrOffset, args[1] + adrOffset, isPtr[0], isPtr[1]);
                break;
            case INS_EQUN:
                equalsn(args[0] + adrOffset, args[1], isPtr[0]);
                break;
            case INS_INV:
                if (args[0] == -1)
                {
                    args[0] = regC - adrOffset;
                }
                invert(args[0] + adrOffset, isPtr[0]);
                break;
            case INS_JMP:
                jump(args[0]);
                break;
            case INS_PJUMP:
                pjump(args[0] + adrOffset, isPtr[0]);
                break;
            case INS_JFZ:
                if (args[1] == -1)
                {
                    args[1] = regC - adrOffset;
                    isPtr[1] = 0;
                }
                jumpIfZero(args[0], args[1] + adrOffset, isPtr[1]);
                break;
            case INS_JNZ:
                if (args[1] == -1)
                {
                    args[1] = regC - adrOffset;
                    isPtr[1] = 0;
                }
                jumpNotZero(args[0], args[1] + adrOffset, isPtr[1]);
                break;
            case INS_HLT:
                jump(0);
                break;
            case INS_CALL:
                call(args[0]);
                break;
             case INS_PCALL:
                pcall(args[0] + adrOffset, isPtr[0]);
                break;
            case INS_RET:
                ret();
                break;
            case INS_PUSH:
                push(args, isPtr);
                break;
            case INS_PULL:
                pull(args, isPtr);
                break;
            case INS_PRT:
                output(args[0] + adrOffset, isPtr[0]);
                break;
            case INS_INP:
                input(args[0] + adrOffset, isPtr[0]);
                break;
            case INS_RAW:
                raw(args);
                break;
            case INS_GUAD:
                moveTo(ZERO, 0);
                addCommands("+[]");
                break;
        }
        i++;

        addCommand('#');
        addBoilerPlateFooter(compoundStack);
    }

    moveTo(programCounter, 0);
    addCommand(']');
    addCommand('\0');
    FILE* out = fopen(outPath, "w");
    fwrite(bf, 1, bfi - 1, out);
    fclose(out);
}