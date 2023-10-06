#include "stdlib.h"
#include "stdio.h"
#include "error.h"

void error(char* type, int line, int chi, char* description)
{
    printf(type, line, chi, description);
    exit(1);
}

void errorSpecial(char* type, int line, int chi)
{
    printf(type, line, chi, "");
}

void errorInstructions(int line, char* description, char* otherInfo)
{
    printf(ERRTYP_INSTRUCTIONS, line);
    printf(description, otherInfo);
    exit(1);
}

void errorMacroExists(int line, int chi, char* macroName)
{
    errorSpecial(ERRTYP_PARSE, line, chi);
    printf(ERR_MACROEXISTS, macroName);
    exit(1);
}