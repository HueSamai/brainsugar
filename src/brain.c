#include  "brain.h"

#define VERSION "0.0.3"

void printTokens();
void printInstructions();

int main(int argc, char** argv)
{
    lieutenant(argc, argv);

    if (ltnargc() == 0)
    {
        printf("BRAIN %s INSTALLED\n\t- assemble a program with: brain `filepath` `list of sugar includes`\n\t- use swithc `--debug` for debug info\n\t- use `-o` or `-output` flag to specify output file destination", VERSION);
        return 0;
    }

    int debugMode = ltnswitchv("--debug");

    outPath = ltnflagv("-o");
    if (outPath == NULL)
    {
        outPath = ltnflagv("-output");
        if (outPath == NULL)
        {
            printf("BRAIN RUN ERROR: no `-o` or `-output` flag was specified.\n");
            exit(1);
        }
    }

    lex(ltnarg(0));
    if (debugMode)
        printTokens();

    parseFirst();
    if (debugMode)
        printInstructions();

    for (int i = 1; i < ltnargc(); i++)
    {
        lex(ltnarg(i));
        if (debugMode)
            printTokens();

        parseOther();
        if (debugMode)
            printInstructions();
    }

    // add termination instruction
    addPseudoIns(INS_NULL, NULL, NULL);

    // turn pseudo instructions into real instructions, by replacing all macro and section
    // names with their corresponding values. also collapsing FWD and BWD
    collapsePseudos();

    assemble();

    return 0;
}

void printTokens()
{
    printf("-------------------TOKENS-------------------\n");
    int i = 0;
    while (tokens[i].op != TOKEN_NULL)
    {
        switch (tokens[i].op)
        {
            case TOKEN_COLON:
                printf("tok_col\n");
                break;
            case TOKEN_ID:
                printf("tok_id %s\n", tokens[i].lexeme);
                break;
            case TOKEN_NUM:
                printf("tok_num %s\n", tokens[i].lexeme);
                break;
            case TOKEN_COMMA:
                printf("tok_com\n");
                break;
            case TOKEN_NEWLINE:
                printf("tok_nl\n");
                break;
        }
        i++;
    }
}

void printInstructions()
{   
    printf("----------------INSTRUCTIONS----------------\n");
    int i = 0;
    int j;
    while (instructions[i].op != INS_NULL)
    {
        printf("%s with ", opcodes[instructions[i].op]);

        j = 0;
        while (instructions[i].args[j] != -1)
        {
            printf("%d ", instructions[i].args[j]);
            j++;
        }
        printf(". jumped to? %d\n", instructions[i].jumpedTo);
        i++;
    }
}