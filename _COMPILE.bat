gcc -c src/*.c -I quark/include -I src/headers
gcc *.o -o brain.exe quark/libquark.a