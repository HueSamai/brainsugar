#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

int strtoint(const char* c)
{
    int n,i;n=i=0;
    while (c[i] != '\0')
        n = n * 10 + c[i++] - 48;
        
    return n;
}

int b = 0;
char bf[99999];
void addChar(char c)
{
    bf[b++] = c;
}

void addChars(const char* str)
{
    int i = 0;
    while (str[i] != '\0')
    {
        addChar(str[i++]);
    }
}

FILE* file;
int getChar = 1;
char c;
int countChar(char a)
{
    int j = 1;
    c = fgetc(file);
    while (c == a)
    {
        c = fgetc(file);
        j++;
    }
    getChar = 0;
    return j;
}

void addThing(const char* format, int num)
{
    char z[20];
    int i,j; i=j=0;
    while (format[i] != '\0') 
    {
        z[i] = format[i];
        i++;
    }

    int k = log10(num);
    k = ceil(pow(10, k));
    while (k >= 1)
    {
        int y = floor(num / k);
        z[i++] = y + 48;
        num -= y * k;
        k /= 10;
    }
    z[i] = '\0';
    addChars(z);
    getChar = 0;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("ERROR: no input file given.");
        return 1;
    }
    if (argc < 3)
    {
        printf("ERROR: no output file given.");
        return 1;
    }

    file = fopen(argv[1], "r");

    addChars("#include <stdio.h>\n#include <stdlib.h>\n#include <stdint.h>\nint main(){");
    addChars("int ptr=0;");
    addChars("int8_t*mem=malloc(1000*sizeof(int8_t));");
    addChars("for(int i=0;i<1000;i++)mem[i]=0;");
    addChars("int inputBufferIndex=-1;char inputBuffer[50];");

    c = fgetc(file);
    while (c != EOF)
    {
        getChar = 1;
        switch (c)
        {
            case '+':
                addChars("inputBufferIndex=-1;");
                addThing("mem[ptr]+=", countChar('+'));
                addChar(';');
                break;
            case '-':
                addChars("inputBufferIndex=-1;");
                addThing("mem[ptr]-=", countChar('-'));
                addChar(';');
                break;
            case '>':
                addThing("ptr+=", countChar('>'));
                addChar(';');
                break;
            case '<':
                addThing("ptr-=", countChar('<'));
                addChar(';');
                break;
            case '.':
                addChars("printf(\"\%c\",mem[ptr]);");
                break;
            case '[':
                addChars("while(mem[ptr]){");
                break;
            case ']':
                addChar('}');
                break;
            case ',':
                addChars("if(inputBuffer[inputBufferIndex]=='\\n'||inputBufferIndex==-1){printf(\"reached here\\n\");scanf(\"%50s\",&inputBuffer);inputBufferIndex=0;}mem[ptr]=inputBuffer[inputBufferIndex];inputBufferIndex++;");
                break;
            default:
                break;
        }

        if (getChar)
            c = fgetc(file);
    }

    addChar('}');
    FILE* output;
    output = fopen(argv[2], "w");

    fwrite(bf, 1, b, output);
    fclose(output);
    fclose(file);

    return 0;
}