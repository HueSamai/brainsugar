#include  "brain.h"

#define VERSION "0.0.1"

void printTokens();
void printInstructions();

int main(int argc, char** argv)
{
    lieutenant(argc, argv);

    outPath = ltnflagv("-o");
    if (outPath == NULL)
    {
        outPath = "temp.bf";
    }

    int debugMode = ltnswitchv("--debug");
    char* inputFile = ltnarg(0);
    if (inputFile == NULL)
    {
        printf("BRAIN %s INSTALLED\n\t- assemble a program with: brain `filepath`\n\t- use swithc `--debug` for debug info\n\t- use `-o` flag to specify output file destination (default output path is `temp.bf`) ", VERSION);
        return 0;
    }

    lex(inputFile);
    if (debugMode)
        printTokens();

    parse();
    if (debugMode)
        printInstructions();

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