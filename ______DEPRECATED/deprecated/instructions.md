# NOTES

points in memory:
    from 0 - whatever the memory size is set to

2 registers:
A and B
at points 0 and 1
output C at 2

usable ram:
    from 3

hex numbers are prefixxed with 'x'

every memory address is offset by 3.

## INSTRUCTIONS

# move data between registers
MVA adr         # move value from an address into register A
MVB adr         # move value from an address into register B
MOV adr1,adr2   # move value from address 1 to address 2
MOO adr         # move output to address

# putting data into registers
PTA val         # put value into register A
PTB val         # put value into register B
PUT adr,val     # put a value into an address

# various looping and conditions
SKP             # skip to after JMP if output or optional address is zero
JMP             # jump back to SKP if output or optional address is not 0
STP             # will make a loop only loop once. useful during if statements

# operations
ADD             # will add A and B together and output the value in C
SUB             # will subract B from A and output the value in C
CML             # compare if the value in A is smaller than B
CMG             # compare if the value in A is greater than B
CME             # compare if the value in A is equal to B

NOTE: to use the compare instructions, just add a SKP-STP block after them to check if they were true

# basic io
OUT adr         # output value at address as an ASCII character
INP adr         # get one character of input from the user and store it at the address



# SIMPLE PROGRAM TO MULTIPLY TWO VALUES

MOV 0,5    # first number
MOV 1,5    # second number

MVA 0       # move zero into A register
LDB 0       # load first number into B
ADD         # add the two numbers

MOO 2       # move output to register 2

LDA 1       # load address 1 into A
MVB 1       # load value 1 into register B
SUB         # subtract them from each other

MOO 1       # move output to address 1

SKP         # if the last output was zero will skip this step. this will only happen if the second number was a 1

LDA 2       # move last output into A register
LDB 0       # load first number into B
ADD         # add the two numbers

MOO 2       # move output to register 2

LDA 1       # load address 1 into A
MVB 1       # load value 1 into register B
SUB         # subtract them from each other

MOO 1

JMP         # if the previous calculation was not zero it will go back to the skip.

OUT 2 