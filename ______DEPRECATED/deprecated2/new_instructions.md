# INSTRUCTIONS

# setting memory
MEM val             ! MEMORY - set how much memory your process needs

## putting data into addresses
PUT adr,val         ! PUT - put a value into an address

## copying data
MOV adr,adr         ! MOVE - copy a value from an address to another address
MOO adr             ! MOVE OUTPUT - copy output to address

## operations
INC adr             ! INCREMENT - increment value of address by 1
DEC adr             ! DECREMENT - decrement value of address by 1
ADD adr,adr         ! ADD - add the values in the addresses
SUB adr,adr         ! SUBTRACT - subtract the value in the second address from the value in the first
CMP adr,adr         ! COMPARE - check if the first value is greater than the second in the addresses
EQU adr,adr         ! EQUALS - check if the values in the two addresses are equal

## pointer stuffies
BYT adr,adr         ! BYTE - copy the value stored at the address, which the first address points to, and copy it to the second address
BMV adr,adr         ! BYTE MOVE - copy the value in the first address, and put it into the address which the second address points to

## io
OUT adr             ! OUTPUT - output value at address
INP adr             ! INPUT - get input and store it into address

## control flow
JMP ins             ! JUMP - jump to instruction 'ins'
JFZ ins,(adr)       ! JUMP IF ZERO - If optional address or output is zero, jump to instruciton number 'ins'
JNZ ins,(adr)       ! JUMP NOT ZERO - If optional address or output is not zero, jump to instruciton number 'ins'
INV (adr)           ! INVERT - invert optional address or output. so any non zero becomes zero and zero becomes a non zero
HLT                 ! HALT - Halt the program
