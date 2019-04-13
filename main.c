 #include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "STRUCTURE.c"


 int main(int argc, char *argv[]) {
    Memoire_Partage mp;
    mp.longueur = 5;
    printf("%zu\n", mp.longueur);
    printf("%d\n", O_CREAT & (O_RDONLY|O_CREAT));
    return 0;
}