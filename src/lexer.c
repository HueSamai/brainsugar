#include "lexer.h"
#include "stdio.h"
#include "ctype.h"
#include "error.h"
#include "strfuncs.h"
#include "vector.h"

FILE* file;
char c;
char next;

int line = 1;
int chi = 1;

int tki = 0;

void lexnext();
void addtoken(int, char*);

void lex(char* path)
{
    tki = 0;
    line = 1;
    chi = 1;

    file = fopen(path, "r");
    if (file == NULL)
    {
        error(ERRTYP_ERROR, 0, 0, ERR_INVALIDFILE);
    }
    c = fgetc(file);
    while (c != EOF)
    {
        lexnext();
    }
    addtoken(TOKEN_NULL, NULL);
    fclose(file);
}

char nextc()
{
    chi++;
    return fgetc(file);
}

int isws(char c)
{
    return c == ' ' || c == '\n' || c == '\t' || c == '\r';
}

int ishex(char c)
{
    return c >= 'a' && c <= 'f';
}

void addtoken(int op, char* lexeme)
{
    tokens[tki].op = op;
    tokens[tki].lexeme = lexeme;
    tokens[tki].chi = chi;
    tokens[tki].line = line;
    tki++;
}

int isstopchar(char c)
{
    return isws(c) || c == ',' || c == EOF || c == ']';
}

void lexnum(int step)
{
    if (step)
    {
        c = next;
        next = nextc();
    }
    int bidx = 0;
    char buffer[8];

    int isHex = c == 'x';
    
    buffer[bidx++] = c;
    c = next;

    while (!isstopchar(c))
    {
        if (isdigit(c) || (isHex && ishex(c)))
        {
            buffer[bidx++] = c;
            c = nextc();
        }
        else
        {
            error(ERRTYP_LEX, line, chi, ERR_INVALIDNUM);
        }
    }

    buffer[bidx] = '\0';
    addtoken(TOKEN_NUM, permptr(buffer));
}

void lexid()
{
    int bidx = 0;
    char buffer[55];

    buffer[bidx++] = c;
    c = next;

    if (c != ':')
    {
        while (!isstopchar(c))
        {
            buffer[bidx++] = c;
            c = nextc();

            if (c == ':')
            {
                break;
            }
        }
    }

    buffer[bidx] = '\0';
    addtoken(TOKEN_ID, permptr(buffer));
}

void lexnext()
{
    if (c == '\n')
    {
        chi = 1;
        line++;
        if (tokens[tki - 1].op != TOKEN_NEWLINE)
            addtoken(TOKEN_NEWLINE, NULL);
        c = nextc();
        return;
    }

    if (isws(c))
    {
        c = nextc();
        return;
    }

    if (c == ':')
    {
        addtoken(TOKEN_COLON, NULL);
        c = nextc();
        return;
    }

    if (c == ',')
    {
        addtoken(TOKEN_COMMA, NULL);
        c = nextc();
        return;
    }

    if (c == '=')
    {
        addtoken(TOKEN_EQUALS, NULL);
        c = nextc();
        return;
    }

    if (c == '~')
    {
        addtoken(TOKEN_TILDA, NULL);
        c = nextc();
        return;
    }

    if (c == '[')
    {
        addtoken(TOKEN_SQR_OPEN, NULL);
        c = nextc();
        return;
    }

    if (c == ']')
    {
        addtoken(TOKEN_SQR_CLOSE, NULL);
        c = nextc();
        return;
    }

    if (c == ';')
    {
        while (c != '\n' && c != EOF)
        {
            c = nextc();
        }
        return;
    }

    next = nextc();

    // checking for just numbers
    if (isdigit(c))
    {
        lexnum(0);
        return;
    }

    // cases checking for: 
    // r0 (or any other letter in front), and rxff (or any other hex with a char in front)
    if (isdigit(next) || next == 'x')
    {
        lexnum(c != 'x');
        return;
    }

    lexid();
}