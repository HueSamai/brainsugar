#include "parser.h"
#include "lexer.h"
#include "error.h"
#include "string.h"
#include <stdarg.h>
#include "strfuncs.h"
#include "ctype.h"
#include "cartographer.h"

#define ARG_SEPARATION 4
#define ARGS_END 3

#define MACRO_SIZE 103

int insIndex = 0;
int tokenIndex = 0;
int relativeMacroOffset = 0;
int storedRelativeMacroOffset = 0;
token_t current;
pseudoins_t pseudoInstructions[9999];

map_t* macroMap;

void parseFirst()
{
    macroMap = mapcreate(MACRO_SIZE);

    // does this really needa  comment like...
    parseStartAt();

    // get macro values
    parseMacros();
    // the actual girth of parsing
    
    parseSections();

    relativeMacroOffset += storedRelativeMacroOffset;
}

void removeWhitespace()
{
    while (current.op == TOKEN_NEWLINE)
    {
        getToken();
    }
}

void parseOther()
{
    // resetting values
    storedRelativeMacroOffset = 0;
    tokenIndex = 0;

    getToken();
    removeWhitespace();

    // get macro values
    parseMacros();
    // the actual girth of parsing
    
    parseSections();

    relativeMacroOffset += storedRelativeMacroOffset;
}

void parseStartAt()
{
    getToken();
    removeWhitespace();

    validateStrict(TOKEN_ID, "start", 1);
    validateStrict(TOKEN_ID, "at", 1);

    // setup jump in correct format
    char buffer[20];
    int k = 0;
    while (current.lexeme[k] != '\0')
    {   
        buffer[k] = current.lexeme[k];
        k++;
    }
    buffer[k] = ARGS_END;
    buffer[k + 1] = '\0';

    addPseudoIns(INS_JMP, permintptr(0), permptr(buffer));
    getToken();
    validateOp(TOKEN_NEWLINE, 1);
}

void parseMacros()
{
    while (peekNext() != TOKEN_COLON)
    {
        validateOp(TOKEN_ID, 0);
        char* macroName = current.lexeme;

        int addOffset = 0;
        getToken();

        if (current.op == TOKEN_TILDA)
        {
            addOffset = 1;
        }
        else if (current.op != TOKEN_EQUALS)
        {
            errorSpecial(ERRTYP_PARSE, current.line, current.chi);
            printf(ERR_NOEQUALS, tokenTypes[current.op]);
            exit(1);
        }

        getToken();
        validateOp(TOKEN_NUM, 0);
        char* macroValue = current.lexeme;

        // check if macro exists
        if (mapget(macroMap, macroName) != NULL)
        {
            errorMacroExists(current.line, current.chi, macroName);
        }
        int macroValueInt = strToInt(macroValue);
        if (addOffset)
        {
            if (macroValueInt > storedRelativeMacroOffset)
            {
                storedRelativeMacroOffset = macroValueInt + 1;
            }
            macroValueInt += relativeMacroOffset;
        }
        
        mapset(macroMap, macroName, permintptr(macroValueInt));

        getToken();
        validateOp(TOKEN_NEWLINE, 1);
    }
}

void parseIns()
{
    int* pointers = malloc(sizeof(int) * 9);

    validateOp(TOKEN_ID, 0);
    int op = getInsOp();
    getToken();
    int bidx = 0;
    int i = 0;
    int argIndex = 0;

    // get args
    char buffer[999];
    if (current.op != TOKEN_NEWLINE && current.op != TOKEN_NULL)
    {
        while (1)
        {
            int isPtr = 0;
            if (current.op == TOKEN_SQR_OPEN)
            {
                isPtr = 1;
                getToken();
            }
            i = 0;
            do
            {
                buffer[bidx++] = current.lexeme[i];
            }
            while (current.lexeme[i++] != '\0');
            buffer[bidx - 1] = ARG_SEPARATION;
            getToken();

            if (isPtr)
            {
                validateOp(TOKEN_SQR_CLOSE, 1);
            }
            pointers[argIndex++] = isPtr;

            if (current.op == TOKEN_NEWLINE || current.op == TOKEN_NULL)
            {
                break;
            }

            validateOp(TOKEN_COMMA, 1);
        }
    }
    else
    {
        bidx++;
    }
    buffer[bidx - 1] = ARGS_END;
    buffer[bidx] = '\0';

    addPseudoIns(op, pointers, permptr(buffer));

    if (current.op != TOKEN_NULL)
        validateOp(TOKEN_NEWLINE, 1);
}

void parseSections()
{
    // checking if there is a next section to parse
    while (peekNext() == TOKEN_COLON)
    {
        validateOp(TOKEN_ID, 0);
        char* sectionName = current.lexeme;

        // raise error with section already exists
        if (mapget(macroMap, sectionName))
            errorMacroExists(current.line, current.chi, sectionName);
        
        mapset(macroMap, sectionName, permintptr(insIndex));
        // get token twice. we already know the next token is a colon
        getToken();
        getToken();
        validateOp(TOKEN_NEWLINE, 1);
        
        while (peekNext() != TOKEN_COLON && current.op != TOKEN_NULL)
        {
            parseIns();
        }
    }
}

void collapsePseudo()
{
    pseudoins_t currentPseudo = pseudoInstructions[insIndex];
    instructions[insIndex].op = currentPseudo.op;
    instructions[insIndex].isPtr = currentPseudo.isPtr;
    int p = 0; // stores current char index in args
    int k = 0; // stores current arg index
    while (currentPseudo.args[p] != ARGS_END)
    {
        int j = 0; // stores current char index in buffer

        // read off of args
        char buffer[33];
        do 
        {
            buffer[j++] = currentPseudo.args[p];
            if (currentPseudo.args[p] == ARGS_END)
            {
                buffer[j-1] = '\0';
                break;
            }
        }
        while (currentPseudo.args[p++] != ARG_SEPARATION);
        buffer[j-1] = '\0';
        int* macro = mapget(macroMap, buffer);
        if (macro == NULL)
        {
            int number = strToInt(buffer);
            if (number == -1)
            {
                number = evalForwardOrBackward(buffer);
                if (number == -1)
                    errorInstructions(currentPseudo.line, ERR_MACRONOTFOUND, buffer);
            }
            instructions[insIndex].args[k] = number;
        }
        else
        {
            instructions[insIndex].args[k] = *macro;
        }
        k++;
    }
    /*
    if (currentPseudo.op == INS_JFZ || currentPseudo.op == INS_JMP || currentPseudo.op == INS_JNZ)
    {
        vecpush(jumpedToInstructions, permintptr(instructions[insIndex].args[0]));
        instructions[insIndex].jumpedTo = 1;
    }
    else if (currentPseudo.op == INS_CALL || currentPseudo.op == INS_RET)
    {
        instructions[insIndex].jumpedTo = 1;
    }
    else
    {
        instructions[insIndex].jumpedTo = 0;
    }*/
    instructions[insIndex].args[k] = -1;
}

int evalForwardOrBackward(char* str)
{
    // check if macro
    if (str[1] == 'W' && str[2] == 'D')
    {
        if (str[0] != 'F' && str[0] != 'B') return -1;
        int i = 3;
        int result = 0;
        while (str[i] != '\0')
        {
            if (!isdigit(str[i])) return -1;
            result = result * 10 + str[i] - '0';
            i++;
        }

        if (str[0] == 'F')
        {
            result = insIndex + result;
        }
        else
        {
            result = insIndex - result;
        }
        
        return result;
    }

    return -1;
}

void collapsePseudos()
{
    jumpedToInstructions = vecmake(5);
    insIndex = 0;
    while (pseudoInstructions[insIndex].op != INS_NULL)
    {
        collapsePseudo();
        insIndex++;
    }
    instructions[insIndex].op = INS_NULL;
    numberOfInstructions = insIndex - 1;
}

void getToken()
{
    current = tokens[tokenIndex++];
}

void validateOp(int op, int thenNext)
{
    if (current.op != op)
    {
        errorSpecial(ERRTYP_PARSE, current.line, current.chi);
        if (current.op == -1)
        {
            printf(ERR_UNEXPECTEDTOKEN, tokenTypes[op], "end of file");
        exit(1);
        }
        printf(ERR_UNEXPECTEDTOKEN, tokenTypes[op], tokenTypes[current.op]);
        exit(1);
    }

    if (thenNext) getToken();
}

void validateLexeme(char* lexeme, int thenNext)
{
    if (strcmp(lexeme, current.lexeme))
    {
        errorSpecial(ERRTYP_PARSE, current.line, current.chi);
        printf(ERR_UNEXPECTEDTOKEN, lexeme, current.lexeme);
        exit(1);
    }

    if (thenNext) getToken();
}

void validateStrict(int op, char* lexeme, int thenNext)
{
    validateOp(op, 0);
    validateLexeme(lexeme, 0);

    if (thenNext) getToken();
}

void addPseudoIns(int op, int* arePointers, char* args)
{
    pseudoInstructions[insIndex].op = op;
    pseudoInstructions[insIndex].isPtr = arePointers;
    pseudoInstructions[insIndex].args = args;
    pseudoInstructions[insIndex++].line = current.line;
}

int peekNext()
{
    return tokens[tokenIndex].op;
}

int* permintptr(int a)
{
    int* b = malloc(sizeof(int));
    b[0] = a;
    return b;
}

int strToInt(char* str)
{
    int isHex = str[0] == 'x';
    // start index
    // this can be equal to isHex, because if it is a hex value we need to skip the 'x', if not we begin at 0
    int i = isHex;
    int result = 0;
    while (str[i] != '\0')
    {
        result *= isHex*6 + 10; // will result in 16 if isHex, else 10
        if (isdigit(str[i]))
            result += str[i] - '0';
        else if (ishex(str[i]))
            result += 10 + str[i] - 'a';
        else
            return -1;

        ++i;
    }

    return result;
}

int getInsOp()
{
    for (int i = 0; i < OPCODE_COUNT; i++)
    {
        if (!strcmp(current.lexeme, opcodes[i]))
        {
            return i;
        }
    }

    errorSpecial(ERRTYP_PARSE, current.line, current.chi);
    printf(ERR_UNKNOWNCOMMAND, current.lexeme);
    exit(1);
}