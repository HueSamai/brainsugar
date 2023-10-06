#include "asm.h"
#include "parser.h"
#include "stdio.h"
#include "brain.h"

int bfi = 0;
char bf[99999];

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

int dataPointer = 0;

// allows global variables to take up 255 registers

int adrOffset = 10; // we have to add the stack pointer and stack size

int stackPointer = 25;

// the shadow offset
int shadowOffset = 200;

int lineLength = 0;
int noCommentLength = 60;

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

void moveTo(int adr)
{
    moveFromTo(dataPointer, adr);
}

void toStart()
{
    //+[-<+]-
    addCommands("+[-<+]-"); // what?
    dataPointer = 0;
}

void put(int adr, int val)
{
    moveTo(adr);
    addCommand('[');
        addCommand('-');
    addCommand(']');
    repeatCommand('+', val);
}

void transfer(int adrFrom, int adrTo)
{
    put(adrTo, 0);
    moveTo(adrFrom);
    addCommand('[');
        addCommand('-');
        moveTo(adrTo);
        addCommand('+');
        moveTo(adrFrom);
    addCommand(']');
}

void copy(int adrFrom, int adrTo)
{
    put(buffer, 0);
    put(adrTo, 0);

    moveTo(adrFrom);
    addCommand('[');
        addCommand('-');
        moveTo(adrTo);
        addCommand('+');
        moveTo(buffer);
        addCommand('+');
        moveTo(adrFrom);
    addCommand(']');

    transfer(buffer, adrFrom);
}

void input(int adr)
{
    moveTo(adr);
    addCommand(',');
}

void output(int adr)
{
    moveTo(adr);
    addCommand('.');
}

void add(int adr1, int adr2)
{   
    copy(adr1, regC);
    copy(adr2, regB);

    moveTo(regB);
    addCommand('[');
        addCommand('-');
        moveTo(regC);
        addCommand('+');
        moveTo(regB);
    addCommand(']');
    toStart();
}

void sub(int adr1, int adr2)
{
    copy(adr1, regC);
    copy(adr2, regB);

    moveTo(regB);
    addCommand('[');
        addCommand('-');
        moveTo(regC);
        addCommand('-');
        moveTo(regB);
    addCommand(']');
    toStart();
}

void addn(int adr, int num)
{
    moveTo(adr);
    repeatCommand('+', num);
}

void subn(int adr, int num)
{
    moveTo(adr);
    repeatCommand('-', num);
}

void compareIfGreater(int adr1, int adr2)
{
    copy(adr1, regA);
    copy(adr2, regB);
    transfer(regA, regC);
    put(buffer, 1);
    moveTo(regC);
    addCommand('+'); // in case of zeros
    moveTo(regB);
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

void invert(int adr)
{
    const int y = 2; // lowest value is 2
    moveTo(adr);
    addCommand('[');
        addCommands("[-]");
        repeatCommand('-', y);
        moveTo(ZERO);
    addCommand(']');

    toStart();
    moveTo(adr);
    repeatCommand('+', y);
}

void equals(int adr1, int adr2)
{
    sub(adr1, adr2);
    invert(regC);
}

void equalsn(int adr, int num)
{
    copy(adr, regC);
    subn(regC, num);
    invert(regC);
}

void moveToInAddress(int adr)
{
    copy(adr, shadowOffset);
    moveTo(shadowOffset);
    addCommands("[->[-]<[>+<-]>]");
    moveBy(-shadowOffset);
}

void moveFromPtr(int ptr, int adrCopyTo)
{
    put(adrCopyTo, 0);
    put(buffer, 0);
    moveToInAddress(ptr); // after this call, dataPointer is no longer valid

    addCommand('[');
        addCommand('-');
        toStart(); // now data pointer is valid again
        moveTo(buffer);
        addCommand('+');
        moveTo(adrCopyTo);
        addCommand('+');
        moveToInAddress(ptr);
    addCommand(']');

    toStart();

    moveTo(buffer);
    addCommand('[');
        addCommand('-');
        moveToInAddress(ptr);
        addCommand('+');
        toStart();
        moveTo(buffer);
    addCommand(']');
}

void moveIntoPtr(int adr, int adrPtr)
{
    put(buffer2, 0);
    moveToInAddress(adrPtr);
    addCommands("[-]");
    toStart();

    moveTo(adr);
    addCommand('[');
        addCommand('-');
        moveToInAddress(adrPtr);
        addCommand('+');
        toStart();
        moveTo(buffer2);
        addCommand('+');
        moveTo(adr);
    addCommand(']');

    transfer(buffer2, adr);
}

void jump(int ins)
{
    // do jump logic
    put(programCounter, ins);
}

void jumpIfZero(int ins, int adr)
{
    copy(adr, buffer2);
    invert(adr); // we invert it // this is a sub comment, commenting on how bad my comment was lol
    addCommand('[');
        jump(ins);
        moveTo(ZERO);
    addCommand(']');
    toStart();
    transfer(buffer2, adr);
}

void jumpNotZero(int ins, int adr)
{
    moveTo(adr);
    addCommand('[');
        jump(ins);
        moveTo(ZERO);
    addCommand(']');
    toStart(); // when in doubt, call toStart
}

void push(int* adrs)
{
    int i = 0;
    while (adrs[i] != -1)
    {
        addn(stackPointer, 1);
        moveIntoPtr(adrs[i] + adrOffset, stackPointer);
        i++;
    }
}

void pull(int* adrs)
{
    int i = 0;
    while (adrs[i] != -1)
    {
        moveFromPtr(stackPointer, adrs[i] + adrOffset);
        subn(stackPointer, 1);
        i++;
    }
}

void compareIfGreaterN(int adr1, int num)
{
    copy(adr1, regA);
    put (regB, num);
    transfer(regA, regC);
    put(buffer, 1);
    moveTo(regC);
    addCommand('+'); // in case of zeros
    moveTo(regB);
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
    pull(adrs);
    sub(programCounter, buffer);
    
    compareIfGreaterN(programCounter, 127);
    moveTo(regC);
    addCommand('[');
        // add number of instructions
        addn(programCounter, numberOfInstructions);
        moveTo(ZERO);
    addCommand(']');
}

void call(int jumpToIns)
{
    int args[2] = { currentInstruction - adrOffset, -1 };
    addn(currentInstruction, 1);
    push(args);
    subn(currentInstruction, 1);
    jump(jumpToIns);
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
    moveTo(currentInstruction);
    addCommand('+');

    // check if currentInstruction equals programCounter
    copy(currentInstruction, instructionCheck);

    copy(programCounter, buffer2);
    moveTo(programCounter);
    addCommand('[');
        addCommand('-');
        moveTo(instructionCheck);
        addCommand('-');
        moveTo(programCounter);
    addCommand(']');
    // we dont have to move to start because our loop starts where we end off
    transfer(buffer2, programCounter);
    
    invert(instructionCheck);
    moveTo(instructionCheck);
    addCommand('[');
        // increment program counter
        moveTo(programCounter);
        addCommand('+');

}

void addBoilerPlateFooter(int inc)
{
    moveTo(ZERO);
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
    put(programCounter, instructions[0].args[0]); 
    put(stackPointer, stackPointer);
    addCommand('[');

    toStart();
    put(currentInstruction, 0);

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
        switch (instructions[i].op)
        {
            case INS_PUT:
                put(args[0] + adrOffset, args[1]);
                break;
            case INS_MOV:
                copy(args[0] + adrOffset, args[1] + adrOffset);
                break;
            case INS_MOO:
                copy(regC, args[0] + adrOffset);
                break;
            case INS_ADD:
                add(args[0] + adrOffset, args[1] + adrOffset);
                if (args[2] != -1)
                {
                    copy(regC, args[2] + adrOffset);
                }
                break;
            case INS_ADDN:
                addn(args[0] + adrOffset, args[1]);
                break;
            case INS_SUB:
                sub(args[0] + adrOffset, args[1] + adrOffset);
                if (args[2] != -1)
                {
                    copy(regC, args[2] + adrOffset);
                }
                break;
            case INS_SUBN:
                subn(args[0] + adrOffset, args[1]);
                break;
            case INS_CMP:
                compareIfGreater(args[0] + adrOffset, args[1] + adrOffset);
                break;
            case INS_CMPN:
                compareIfGreaterN(args[0] + adrOffset, args[1]);
                break;
            case INS_EQU:
                equals(args[0] + adrOffset, args[1] + adrOffset);
                break;
            case INS_EQUN:
                equalsn(args[0] + adrOffset, args[1]);
                break;
            case INS_INV:
                if (args[0] == -1)
                {
                    args[0] = regC - adrOffset;
                }
                invert(args[0] + adrOffset);
                break;
            case INS_JMP:
                jump(args[0]);
                break;
            case INS_JFZ:
                if (args[1] == -1)
                {
                    args[1] = regC - adrOffset;
                }
                jumpIfZero(args[0], args[1] + adrOffset);
                break;
            case INS_JNZ:
                if (args[1] == -1)
                {
                    args[1] = regC - adrOffset;
                }
                jumpNotZero(args[0], args[1] + adrOffset);
                break;
            case INS_HLT:
                jump(0);
                break;
            case INS_CALL:
                call(args[0]);
                break;
            case INS_RET:
                ret();
                break;
            case INS_PUSH:
                push(args);
                break;
            case INS_PULL:
                pull(args);
                break;
            case INS_MTP:
                moveIntoPtr(args[0] + adrOffset, args[1] + adrOffset);
                break;
            case INS_MFP:
                moveFromPtr(args[0] + adrOffset, args[1] + adrOffset);
                break;
            case INS_PRT:
                output(args[0] + adrOffset);
                break;
            case INS_INP:
                input(args[0] + adrOffset);
                break;
            case INS_PRTP:
                moveToInAddress(args[0] + adrOffset);
                addCommand('.');
                break;
            case INS_PIN:
                moveToInAddress(args[0] + adrOffset);
                addCommand(',');
                break;
            case INS_RAW:
                raw(args);
                break;
            case INS_GUAD:
                moveTo(ZERO);
                addCommands("+[]");
                break;
        }
        i++;

        addBoilerPlateFooter(compoundStack);
    }

    moveTo(programCounter);
    addCommand(']');
    addCommand('\0');
    FILE* out = fopen(outPath, "w");
    fwrite(bf, 1, bfi - 1, out);
    fclose(out);
}