#pragma once

// give lieutenant arg info to help with command-line argument handling
void lieutenant(int argc, char** argv);

// get flag count
int ltnflagc();

// get value of a flag
// returns 0 if not found
char* ltnflagv(char* flagname);

// get flag name from an index
// returns 0 if index is out of bounds
char* ltnflag(int index);

// get switch count
int ltnswitchc();

// check if a switch is present
// returns 0 if its not found
// returns 1 if it is
int ltnswitchv(char* switchname);

// get switch from an index
// returns 0 if index is out of bounds
char* ltnswitch(int index);


// get adjusted argc, which ignores flags and switches
int ltnargc();

// get the command argument at the given index, ignoring the executable path, flags and switches
// returns 0 if index is out of bounds
char* ltnarg(int index);