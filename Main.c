 #include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "STRUCTURE.c"


 int main(int argc, char *argv[]) {
    Memoire_Partage mp;
    mp.liste[34];
    printf("%zu\n", sizeof(mp));
     printf("%zu\n", 2* sizeof(int) + 2* sizeof(size_t));
    return 0;
}