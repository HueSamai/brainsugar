# What the brainfuck is SUGAR?

SUGAR is an assembly language. it is assembled with the 'brain' compiler into brainfuck code. This is a document detailing the syntax and instruction list of SUGAR, so that you can use it.

## why the name sugar?
I have also made a programming language called FUDGE, which compiles into SUGAR. SUGAR, comes from the name of language FUDGE, since SUGAR is the building blocks of FUDGE, in baking and in the programming languages.

# Instruction list

## some notes
reg is short for register
ins is short for instruction
ptr is short for pointer

NOTE: ptr is a reg which stores the address of another reg

the output reg or regC, is a register which comparisons and jumps are by default based off of. this is different to the operations 'out' argument. that is just a normal register, which the result of the output is copied to.

## moving and setting data
```put reg1,num                    - put: put 'num' into reg1```
<br>

```mov reg1,reg2                   - move: copy value from reg1 to reg2```
<br>

```moo reg1                        - move output: copy output to reg1```

## operations
```add reg1,reg2,(out)             - add: adds two regs. output is moved also to out reg```
<br>

```addn adr1,num                   - add number: increments address by num. DOESN'T CHANGE OUTPUT REG```
<br>

```sub reg1,reg2,(out)             - subtract: subtract reg2 from reg1. output is moved to out reg```
<br>

```subn reg1,num                   - subtract number: subtract num from reg1```
<br>

```cmp reg1,reg2                   - compare: stores result of (reg1 > reg2) in output reg```
<br>

```cmpn reg1,num                   - compare number: stores result of (reg1 > num) in output reg```
<br>

```equ reg1,reg2                   - equals: stores result of (reg1 == reg2) in output reg```
<br>

```equn reg1,num                   - equals number: stores result of (reg1 == num) in output reg```
<br>

NOTE: sub can be used to check if two numbers are not equal.

```inv (reg1)                      - invert: turns a non-zero value to zero, and vice versa. reg1 is the output reg if it is not set```

## control flow

```jmp ins                         - jump: jump to instruction```
<br>

```jfz ins,(reg)                   - jump if zero: jump to instruction 'ins' if the reg, or the output reg, if reg1 wasn't given, is zero```
<br>

```jnz ins,(reg)                   - jump not zero: same as jfz, but if the value is not zero```
<br>
NOTE: jnz is more performant that jfz
<br>

```hlt                             - halt: halts the program```
<br>
NOTE: hlt is equivalent to jmp 0
<br>

```call ins                        - call: add current instruction to stack and jump to instruction```
<br>

```ret                             - return: return to last instruction on the stack```

## stacking
```push reg...                     - push: takes 1 or more regs as arguments, and pushes each of them in order onto the stack.```
<br>

```pull reg...                     - pull: takes 1 or more regs as arguments, and pulls each of them in order from the stack```
<br>

NOTE: this means if you push before calling, you need to pull in the reverse order you pushed.

## pointers
```mtp reg1,ptr                    - move into pointer: move contents of reg1 into reg which ptr points to```
<br>

```mfp ptr,reg1                    - move from pointer: move contents of reg which ptr points to into reg1```
<br>

## io
```prt reg                         - print: print value at reg into the console```
<br>

```prtp ptr                        - print pointer: print the value at reg, which ptr points to```
<br>

```inp reg                         - input: store next char of input into reg```
<br>

```pin ptr                         - pointer input: store next char of input into reg, which ptr points to```

## special
```raw bf,...                      - raw: run raw brainfuck code given. takes any number of args. check RAW section for info.```
<br>

NOTE: this is for obvious reasons unsafe. only reason this is here, is to implement more performant code, but then just use a proper programming language lol. there is a section later on discussing, what to not do when using raw.
<br>

```guad                            - give up and die: enter an infinite loop of nothingness```
<br>
NOTE: don't use guad.

# Syntax

## comments
semi-colons are used for comments. if the parser reaches a semi-colon, the rest of that line is ignored.

## some notes

you can write numbers in hex, by prefacing the number with an 'x' for example:
254 can be written as 254, or xfe

numbers can also be prefaced by any lowercase letters (other than x) in the alphabet to make code more readable. for instance, to make it apparent that a value is referring to a register, a number can be prefaced with r.

for example: rxfe
The assembler will parse 'rxfe', by first getting rid of the first letter, becoming 'xfe', then parsing this as a number.

the assembler WON'T do this for values beginning with x, as they are parsed as hex.
for example: xfe

The assembler will check if the first letter is a lowercase letter other than x, since it is x it won't remove it, and the 'xfe' will just be directly parsed as a hex number.

Here are some other character prefixes for values:
 - r: registers
    - use case: add r10,r11,r10
 - c: characters
    - use case: addn r10,c48
 - p: pointers
    - use case: mtp r10,p10
 - i: instruction
    - use case (although rare): jfz i10 

## sections

sections make jumps easier. your code MUST have an entry point section, which is specified at the beginning of the file by: start at

sections are declared by typing a name and then following them with a semi-colon (:).

for example: 
'start:'

then our file will look something like this:

```
start at start

start:
    ; code goes in here

```

NOTE: indentation in sections isn't required, but helps with visibility.

A section ends when a new section starts. by this definition you can't have sections within other sections, which makes sense.

we can use sections to make our code more maintable, because calls and jumps don't have to change if we just add one instruction.

NOTE: sections are not kept when assembling into bf. they merely provide a marker, so that the assembler knows which line to jump to. every call to a section, gets converted to a call to an instruction. this means, you can write code without sections, but that is just not maintainable, as every time you add a line of code, the instruction index will change.

## special macros

There are some special macros to help with loops. 

These are:
 - FWD(n)
 - BWD(n)

after each of these macros, a number must be supplied.
the FWD(n) will evaluate to the instruction 'n' instructions forward from it.
the BWD(n) will evaluate to the instruction 'n' instructions backward from it.
use case:

jfz FWD3,r1
    inc r1
jmp BWD2

NOTE: FWD(n) and BWD(n), DON'T consider lines with just comments and empty lines as instructions. This is because those are ignored when parsing.

## user defined macros

after the first line, 'start at', you can define your own custom macros, with the format: macro_name = macro_value
OR
macro_name macro_value (the equal sign is not necessary)
macro_values can only be numbers.

this can be useful to keep track of certain registers, for instance, registers used for function arguments, so that you don't have to remember all of them.

macros can't have the same names as sections.

## using raw
here is a list of what NOT to do when using raw:
 - change the first data cell to anything other than 2^n-1, where n is the size of a data cell.
 - change the ZERO data cell, to anything other than zero. the zero data cell's location can be found in the source code.
 - forcefully change values in the shadow realm.
 - modify any stack pointers, and stack data.

if you don't fully understand all of these things, it means you shouldn't be using raw. also just don't use raw. it will make your code hard to debug.

if you do any of these things, your code will most certainly break.

RUNNING RAW:
 - 0 means +
 - 1 means -
 - 2 means >
 - 3 means <
 - 4 means \[
 - 5 means \]
 - 6 means .
 - 7 means ,

EXAMPLE: raw 2,2,2,0,0,0,3,3,3
OUTPUT: >>>+++<<<

# Running SUGAR code

Install the 'brain' assembler from this repo, then assemble a program by running 'brain' and passing the input sugar file path and the output file path, as the first and second arguments, respectively. 

the output file will contain the assembled brainfuck code, which then can be compiled or interpretered with a brainfuck compiler or interpreter. there is a great brainfuck interpreter online called: el brainfuck. which can be found <a href='https://copy.sh/brainfuck/'>here</a>