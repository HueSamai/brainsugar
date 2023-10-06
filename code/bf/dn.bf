>>>>>[-]++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++put 204 into address 0
>[-]                                                        put 0 into address 1
>[-]                                                        put 0 into address 2
>[-]                                                        put 0 into address 3
<<<<<[-]>>>>>>[-]<<<<[->>>>+<<<<<<+>>][-]<<[->>+<<]         copy data at address 0 into address 4
[-]<<<[-]>>>>>>>>>[-<<<<<<<<<+>>>+>>>>>>][-]<<<<<<[->>>>>>+<<<<<<]move data in address 4 into regA
<<[-]+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++put 99 into regB
>[-]<<[->>+<<]>>>[-]+<+<+[>-[<]>-]>[>]<                     compare if value in A is greater than B
<[
>[-]<<<[-]>>>>>>[-<<<<<<+>>>+>>>][-]<<<[->>>+<<<]           move data in address 1 into regA
<<[-]+                                                      put 1 into regB
>[-]<<[->>+<<]>[->+<]                                       add values in regA and B and store it in the output register
>>[-]>>>[-]<<<<[->>>>+<<<+<][-]>[-<+>]                      copy output into address 1
[-]<<<[-]>>>>>>>>>[-<<<<<<<<<+>>>+>>>>>>][-]<<<<<<[->>>>>>+<<<<<<]move data in address 4 into regA
<<[-]++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++put 100 into regB
>[-]<<[->>+<<]>[->-<]                                       subtract the values in regA and B and store it in the output register
>>[-]>>>>>>[-]<<<<<<<[->>>>>>>+<<<<<<+<][-]>[-<+>]          copy output into address 4
[-]<<<[-]>>>>>>>>>[-<<<<<<<<<+>>>+>>>>>>][-]<<<<<<[->>>>>>+<<<<<<]move data in address 4 into regA
<<[-]+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++put 99 into regB
>[-]<<[->>+<<]>>>[-]+<+<+[>-[<]>-]>[>]<                     compare if value in A is greater than B
<]                                                          jump if output is not zero
>[-]<<<[-]>>>>>>>>>[-<<<<<<<<<+>>>+>>>>>>][-]<<<<<<[->>>>>>+<<<<<<]move data in address 4 into regA
<<[-]+++++++++                                              put 9 into regB
>[-]<<[->>+<<]>>>[-]+<+<+[>-[<]>-]>[>]<                     compare if value in A is greater than B
<[
>[-]<<<[-]>>>>>>>[-<<<<<<<+>>>+>>>>][-]<<<<[->>>>+<<<<]     move data in address 2 into regA
<<[-]+                                                      put 1 into regB
>[-]<<[->>+<<]>[->+<]                                       add values in regA and B and store it in the output register
>>[-]>>>>[-]<<<<<[->>>>>+<<<<+<][-]>[-<+>]                  copy output into address 2
[-]<<<[-]>>>>>>>>>[-<<<<<<<<<+>>>+>>>>>>][-]<<<<<<[->>>>>>+<<<<<<]move data in address 4 into regA
<<[-]++++++++++                                             put 10 into regB
>[-]<<[->>+<<]>[->-<]                                       subtract the values in regA and B and store it in the output register
>>[-]>>>>>>[-]<<<<<<<[->>>>>>>+<<<<<<+<][-]>[-<+>]          copy output into address 4
[-]<<<[-]>>>>>>>>>[-<<<<<<<<<+>>>+>>>>>>][-]<<<<<<[->>>>>>+<<<<<<]move data in address 4 into regA
<<[-]+++++++++                                              put 9 into regB
>[-]<<[->>+<<]>>>[-]+<+<+[>-[<]>-]>[>]<                     compare if value in A is greater than B
<]                                                          jump if output is not zero
[
>[-]<<<[-]>>>>>>[-<<<<<<+>>>+>>>][-]<<<[->>>+<<<]           move data in address 1 into regA
<<[-]++++++++++++++++++++++++++++++++++++++++++++++++       put 48 into regB
>[-]<<[->>+<<]>[->+<]                                       add values in regA and B and store it in the output register
>>[-]>>>[-]<<<<[->>>>+<<<+<][-]>[-<+>]                      copy output into address 1
>>>.                                                        output data from address 1
<<]                                                         stop loop after one iteration
<<[
>[-]<<<[-]>>>>>>>[-<<<<<<<+>>>+>>>>][-]<<<<[->>>>+<<<<]     move data in address 2 into regA
<<[-]++++++++++++++++++++++++++++++++++++++++++++++++       put 48 into regB
>[-]<<[->>+<<]>[->+<]                                       add values in regA and B and store it in the output register
>>[-]>>>>[-]<<<<<[->>>>>+<<<<+<][-]>[-<+>]                  copy output into address 2
>>>>.                                                       output data from address 2
<<<]                                                        stop loop after one iteration
<[-]<<<[-]>>>>>>>>>[-<<<<<<<<<+>>>+>>>>>>][-]<<<<<<[->>>>>>+<<<<<<]move data in address 4 into regA
<<[-]++++++++++++++++++++++++++++++++++++++++++++++++       put 48 into regB
>[-]<<[->>+<<]>[->+<]                                       add values in regA and B and store it in the output register
>>[-]>>>>>[-]<<<<<<[->>>>>>+<<<<<+<][-]>[-<+>]              copy output into address 3
>>>>>.                                                      output data from address 3