#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <semaphore.h>

typedef struct Memoire_Partage Memoire_Partage;
struct Memoire_Partage {

    size_t longueur;
    size_t capacite;
    int first;
    int last;
    int taille_fichier;
    sem_t* sem_first;
    sem_t* sem_last;
    char liste[];
};

typedef struct MESSAGE MESSAGE;

struct MESSAGE {
    int permission;
    Memoire_Partage* mp;
};