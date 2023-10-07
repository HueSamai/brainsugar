#pragma once

/*
CARTOGRAPHER by Jayden van Zuydam

hashmap

*/

typedef struct bucketnode
{
    char* key;
    void* value;
    void* next;
} bucketnode_t;

// hasmap struct
typedef struct map
{
    int entryCount;
    int size;
    bucketnode_t** buckets;
} map_t;

// create a new map
// size is the starting size of the hashmap
// the map will be resized automatically if, it reaches a high or low enough load factor
map_t* mapcreate(int size);

// set a value in map
void mapset(map_t* map, char* key, void* value);

// get value in map
// returns NULL if failed
// returns value if succeeded
void* mapget(map_t* map, char* key);

// frees a map
// if shallow is non zero, the values of the map won't be freed
void mapfree(map_t* map, int shallow);
