#pragma once

// this is all we need lol
#define TOKEN_NULL -1

#define TOKEN_ID 0
#define TOKEN_NUM 1
#define TOKEN_COLON 2
#define TOKEN_COMMA 3
#define TOKEN_NEWLINE 4
#define TOKEN_EQUALS 5
#define TOKEN_TILDA 6

static char tokenTypes[5][20] = {
    "identifier",
    "number",
    "colon",
    "comma",
    "newline"
};

typedef struct token
{
    int op;
    char* lexeme;
    int line;
    int chi;
} token_t;

void lex(char* path);
token_t tokens[99999];
int ishex(char c);