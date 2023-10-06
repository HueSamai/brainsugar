#pragma once

// vector definition
typedef struct vector
{
    void** items;
    int capacity;
    int size;
} vector_t;

// create a new vector
// capacity is the starting capacity
vector_t* vecmake(int capacity);

// push a value onto a vector
void vecpush(vector_t* vec, void* value);

// get a value from a vector
// returns 0 if index was out of bounds
// returns pointer to value if succeeded
void* vecat(vector_t* vec, int index);

// free a vector, so all objects the vector points to, WILL also be freed
void vecfreedeep(vector_t* vec);

// free a vector, but all the objects the vector points to, WON'T be freed
void vecfreeshallow(vector_t* vec);