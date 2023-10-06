#pragma once
#include "stdio.h"
#include "stdlib.h"

#define ERRTYP_LEX "LexError(L%dC%d): %s"
#define ERRTYP_PARSE "ParseError(L%dC%d): %s"
#define ERRTYP_INSTRUCTIONS "InstructionError(L%d): "
#define ERRTYP_ERROR "Error(L%dC%d): %s"

#define ERR_INVALIDNUM "invalid number!\npotential fixes:\n\t- make sure identifiers and numbers are spaced apart\n\t- your hex value is valid and only uses lowercase letters\n\t- suffix your hex values with 'x'"
#define ERR_INVALIDFILE "invalid file!\npotential fixes:\n\t- make sure your file path is correct."
#define ERR_UNEXPECTEDTOKEN "unexpected token! expected '%s' got '%s'!\npotential fixes:\n\t- make sure your file starts with 'start at ...'\n\t- make sure you have a comma between each macro/section/number for the args of an instruction\n\t- make sure you put colons after section names\n\t- make sure instructions are separated with newlines\n\t- make sure your macros only expand to numbers"
#define ERR_MACROEXISTS "a macro with the name '%s' already exists!\npotential fixes:\n\t- give your macros unique names\n\t- make sure a macro and a section don't have the same name"
#define ERR_UNKNOWNCOMMAND "unknow instruction '%s'!\npotential fixes:\n\t- make sure you have a ':' after each section name\n\t- make sure you typed the name of the instruction correctly\n\t- make sure the instruction you want to use goes by a different name than you remember"
#define ERR_MACRONOTFOUND "use of an undefined macro/section '%s'!\npotential fixes:\n\t- make sure you spelled the macro/section name correctly\n\t- make sure you defined the macro/section."

void error(char* type, int line, int chi, char* description);
void errorSpecial(char* type, int line, int chi);
void errorInstructions(int line, char* description, char* otherInfo);

// custom
void errorMacroExists(int line, int chi, char* macroName);