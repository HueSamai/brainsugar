#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#define EOC -1

#define PUT 0
#define MOV 1
#define MOO 2

#define INC 3
#define DEC 4
#define ADD 5
#define SUB 6
#define CMP 7
#define EQU 8

#define BYT 9
#define BMV 10

#define OUT 11
#define INP 12

#define JFZ 13
#define JNZ 14
#define JMP 15
#define INV 16
#define HLT 17

#define MEM 18

char* cmds[19] = {
    "PUT",
    "MOV",
    "MOO",
    "INC",
    "DEC",
    "ADD",
    "SUB",
    "CMP",
    "EQU",
    "BYT",
    "BMV",
    "OUT",
    "INP",
    "JFZ",
    "JNZ",
    "JMP",
    "INV",
    "HLT",
    "MEM"
};

typedef struct command
{
    int command;
    int value1;
    int value2;
} command_t;

command_t *parseFile(char *fileName);
void compile(command_t* commands, const char*);

int shouldComment = 0;
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("No input file was given!");
        return 1;
    }

    if (argc < 3)
    {
        printf("No output file was given!");
        return 1;
    }

    if (argc > 3)
    {
        shouldComment = !strcmp(argv[3],"on");
    }

    command_t *commands = parseFile(argv[1]);
    compile(commands, argv[2]);

    return 0;
}

int ishex(char c)
{
    return c == 'a' || c == 'b' || c == 'c' || c == 'd' || c == 'e' || c == 'f';
}

char c;
int parseValue(FILE *file, int currentLine)
{
    int value = 0;

    int isHex = 0;
    if (c == 'x')
    {
        isHex = 1;
        c = fgetc(file);
    }

    if (isdigit(c) || (ishex(c) && isHex))
    {
        while (isdigit(c) || (ishex(c) && isHex))
        {
            int v = c - 48;
            if (ishex(c))
                v = c - 87;
            if (isHex)
                value = value * 16 + v;
            else
                value = value * 10 + v;    
            c = fgetc(file);
        }
    }
    else
    {
        printf("Expected number or 'abcdef', if hex, after command at line %d!", currentLine);
        exit(1);
    }

    return value;
}

void parseCommand(FILE* file, command_t* commands, int commandIndex, int currentLine)
{
    int commandCharIndex = 0;
    char command[4];
    command[3] = 0;

    while (isalpha(c))
    {
        if (commandCharIndex > 3)
        {
            printf("INVALID COMMAND AT %d!", currentLine);
            exit(1);
        }
        command[commandCharIndex++] = c;
        c = fgetc(file);
    }

    int commandCode = 0;
    while (strcmp(command, cmds[commandCode]))
    {   
        commandCode++;
        if (commandCode > 100)
        {
            printf("INVALID COMMAND");
            exit(1);
        }
    }

    int shouldParseValue = 0;
    if (c == ' ')
    {
        shouldParseValue = 1;
    }
    c = fgetc(file);

    int val1, val2;
    val1 = -1;
    val2 = -1;

    if (shouldParseValue)
    {
        val1 = parseValue(file, currentLine);

        if (c == ',')
        {
            c = fgetc(file);
            val2 = parseValue(file, currentLine);
        }
    }

    printf("%s (%d): %d %d\n", command, commandCode, val1, val2);

    commands[commandIndex].command = commandCode;
    commands[commandIndex].value1 = val1;
    commands[commandIndex].value2 = val2;
}

command_t *parseFile(char *fileName)
{
    FILE *file;
    file = fopen(fileName, "r");

    int commandIndex = 0;
    command_t *commands = malloc(sizeof(command_t) * 999);

    int currentLine = 1;

    c = fgetc(file);

    while (c != EOF)
    {
        if (c == ';')
        {
            do
            {
                c = fgetc(file);
            } while (c != '\n');
        }

        if (c == '\n')
        {
            currentLine++;
        }
        else if (isalpha(c))
        {
            parseCommand(file, commands, commandIndex++, currentLine);
            // haha this code needs to be here but im too lazy to explain why :P
            if (commands[commandIndex - 1].value1 == -1) continue;
        }

        c = fgetc(file);
    }

    commands[commandIndex].command = EOC;
    fclose(file);
    return commands;
}

int bfi = 0;
char bf[99999];
int adrOffset = 10;
// address 0 is reserved to find our way back to the start
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

int shadowOffset = -1;

int lineLength = 0;
int noCommentLength = 60;
void addCommand(char cmd)
{
    lineLength++;
    bf[bfi] = cmd;
    bfi++;
    if ((!shouldComment) && bfi % noCommentLength == noCommentLength - 1)
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

void addComment(const char* comment, int a, int b)
{
    if (!shouldComment)
    {
        return;
    }

    char buffer[200];
    sprintf(buffer, comment, a, b);

    int f = noCommentLength - lineLength;
    for (int j = 0; j < f; j++)
    {
        addCommand(' ');
    }

    int i = 0;
    while (buffer[i] != '\0')
    {
        addCommand(buffer[i++]);
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

void jump(int adr)
{
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

void compareIfGreater()
{
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

void moveToInAddress(int adr)
{
    if (shadowOffset == -1)
    {
        printf("ERROR: before you can use BYT or BMV, you need to set a memory size with MEM");
        exit(1);
    }

    copy(adr, shadowOffset);
    moveTo(shadowOffset);
    addCommands("[->[-]<[>+<-]>]");
    moveBy(adrOffset - shadowOffset);
}

void byte(int adr, int adrCopyTo)
{
    put(adrCopyTo, 0);
    put(buffer, 0);
    moveToInAddress(adr); // after this call, dataPointer is no longer valid

    addCommand('[');
        addCommand('-');
        toStart(); // now data pointer is valid again
        moveTo(buffer);
        addCommand('+');
        moveTo(adrCopyTo);
        addCommand('+');
        moveToInAddress(adr);
    addCommand(']');

    toStart();

    moveTo(buffer);
    addCommand('[');
        addCommand('-');
        moveToInAddress(adr);
        addCommand('+');
        toStart();
        moveTo(buffer);
    addCommand(']');
}

void byteMove(int adr, int adrPtr)
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

void addBoilerPlateFooter()
{
    // this line musn't loop
    moveTo(ZERO);
    addCommand(']');
    toStart();
}

void compile(command_t* commands, const char* outputFileName)
{
    int i = 0;
    addCommand('-'); // set first cell to 255, so that we can identify it
    put(programCounter, 1);
    
    addCommand('[');
    toStart();
    put(currentInstruction, 0);

    do
    {
        lineLength = 0;
        int val1,val2;
        val1 = commands[i].value1;
        val2 = commands[i].value2;

        addBoilerPlateHeader(); // do some stuff
        
        switch(commands[i].command)
        {   
            case PUT:
                put(val1 + adrOffset, val2);
                addComment("put %d into address %d", val2, val1);
                break;
            case MOV:
                copy(val1 + adrOffset, val2 + adrOffset);
                addComment("copy data at address %d into address %d", val1, val2);
                break;
            case MOO:
                copy(regC, val1 + adrOffset);
                addComment("copy output into address %d", val1, 0);
                break;
            case INC:
                moveTo(val1 + adrOffset);
                addCommand('+');
                break;
            case DEC:
                moveTo(val1 + adrOffset);
                addCommand('-');
                break;
            case OUT:
                output(val1 + adrOffset);
                addComment("output data from address %d", val1, 0);
                break;
            case INP:
                input(val1 + adrOffset);
                addComment("get input and store it in address %d", val1, 0);
                break;
            case ADD:
                add(val1 + adrOffset, val2 + adrOffset);
                addComment("add values in regA and B and store it in the output register", 0,0);
                break;
            case SUB:
                sub(val1 + adrOffset, val2 + adrOffset);
                addComment("subtract the values in regA and B and store it in the output register", 0,0);
                break;
            case CMP:
                copy(val1 + adrOffset, regA);
                copy(val2 + adrOffset, regB);
                compareIfGreater();
                addComment("compare if value in %d is greater than value in %d", val1, val2);
                break;
            case EQU:
                // bruh
                equals(val1 + adrOffset, val2 + adrOffset);
                addComment("check if value in %d equals value in %d", val1, val2);
                break;
            case BYT:
                byte(val1 + adrOffset, val2 + adrOffset);
                addComment("copy data to %d from where %d points to", val2, val1);
                break;
            case BMV:
                byteMove(val1 + adrOffset, val2 + adrOffset);
                addComment("copy data from %d to where %d points to", val1, val2);
                break;
            case JMP:
                put(programCounter, val1);
                break;
            case JNZ:
                if (val2 == -1)
                {
                    val2 = regC - adrOffset;
                }

                moveTo(val2 + adrOffset);
                addCommand('[');
                    put(programCounter, val1);
                    moveTo(ZERO);
                addCommand(']');
                toStart(); // when in doubt, call toStart
                break;
            case INV:
                if (val1 == -1)
                {
                    val1 = regC;
                }

                invert(val1);
                break;
            case JFZ:
                if (val2 == -1)
                {
                    val2 = regC - adrOffset;
                }

                copy(val2 + adrOffset, buffer2);
                invert(val2 + adrOffset); // we invert it // this is a sub comment, commenting on how bad my comment was lol
                addCommand('[');
                    put(programCounter, val1);
                    moveTo(ZERO);
                addCommand(']');
                toStart();
                transfer(buffer2, val2 + adrOffset);
                break;
            case HLT:
                put(programCounter, 0);
                break;
            case MEM:
                if (shadowOffset != -1)
                {
                    printf("ERROR: you can't use MEM more than once per file.");
                    exit(1);
                }
                shadowOffset = val1 + adrOffset;
                break;
        }

        addBoilerPlateFooter(); // more boiler plate
        if (shouldComment)
            addCommand('\n');
    } 
    while (commands[++i].command != EOC);

    moveTo(programCounter);
    addCommand(']');

    FILE* output;
    output = fopen(outputFileName, "w");
    fwrite(bf, 1, bfi, output);
    fclose(output);
}