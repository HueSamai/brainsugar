-
>
>+
>++++
>++++++++++++++++++++++++
>+++++++++++++
>++++++++++++++++
<<<<<<


>>>>>>>>> this is a constant offset

setup values :P
<<
+++++ this is the cell which stores the cell we want to copy the value from
>>

empty the cell we are going to copy into

move back
<<<<<<<<< this is the same constant offset as before

empty the cell we're copying to
>>
[-]
<<

now the loop must begin
>>>>>>>>> this is a constant offset

move to address stored in the first cell
<<[->+>+<<]>>
<[-<+>]>

[->[-]<[>+<-]>] move to whatever cell index which is in the first cell
<<<<<<<<< this is the same constant offset as before

[
    - we minus one from that address

    +[-<+]- go back to beginning

    increase buffer
    >
    +
    increase cell we want to copy into
    >
    +
    back to the beginning
    <<

    >>>>>>>>> this is a constant offset

    move to address stored in the first cell to check if it is 0
    <<[->+>+<<]>>
    <[-<+>]>

    [->[-]<[>+<-]>] move to whatever cell index which is in the first cell
    <<<<<<<<< this is the same constant offset as before
]

back to the beginning
+[-<+]-

>
[
    -
    <

    >>>>>>>>> this is a constant offset

    move to address stored in the first cell to check if it is 0
    <<[->+>+<<]>>
    <[-<+>]>

    [->[-]<[>+<-]>] move to whatever cell index which is in the first cell
    <<<<<<<<< this is the same constant offset as before

    +

    +[-<+]- go back to beginning

    >
]

and finally move back to the beginning
+[-<+]-