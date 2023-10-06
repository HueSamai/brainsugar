>>>>[-]++++++    put 6 into address 0
>[-]++++++++    put 8 into address 1
<<[-]>>>[-]<<[->>+<<<+>][-]<[->+<]    copy data at address 0 into address 2
[-]<<<[-]>>>>>[-<<<<<+>>>+>>][-]<<[->>+<<]    move data in address 1 into regA
<<[-]+    put 1 into regB
>[-]<<[->>+<<]>[->-<]    subtract the values in regA and B and store it in the output register
>[
>[-]>>[-]<<<[->>>+<<+<][-]>[-<+>]    copy output into address 1
[-]<<<[-]>>>>[-<<<<+>>>+>][-]<[->+<]    move data in address 0 into regA
[-]<<[-]>>>>>[-<<<<<+>>+>>>][-]<<<[->>>+<<<]    move data in address 2 into regB
<[-]<<[->>+<<]>[->+<]    add values in regA and B and store it in the output register
>>[-]>>>[-]<<<<[->>>>+<<<+<][-]>[-<+>]    copy output into address 2
[-]<<<[-]>>>>>[-<<<<<+>>>+>>][-]<<[->>+<<]    move data in address 1 into regA
<<[-]+    put 1 into regB
>[-]<<[->>+<<]>[->-<]    subtract the values in regA and B and store it in the output register
>>[-]>>[-]<<<[->>>+<<+<][-]>[-<+>]    copy output into address 1
<]    jump if output is not zero
>[-]>>>>>>>>[-<<<<<<<<+>>>>>>>>]<<<<<[.>]>>>>>[-]<<<<<<<<[->>>>>>>>+<<<<<<<<]    write data from adress 2 to 1