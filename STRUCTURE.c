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
#include <sys/uio.h>

typedef struct Memoire_Partage Memoire_Partage;
struct Memoire_Partage {

    //longueur d'un message
    size_t longueur;

    //le nombre de message maximal qu'une file peut contenir
    size_t nb_message_max;

    //le nombre reel de message que contient la file
    size_t nb_message;

    //le nombre de processus en attente d'un message
    size_t nb_proc_att;

    //l'emplacement du premier octet libre
    int first;

    //l'emplacment du premier octet du premier message
    int last;

    //la taille total de la memoire partage
    int taille_fichier;

    //le pid enregistrer pour les signaux
    int pid;

    //le signal a recevoir
    int sig;

    //le semaphore pour l'indice first
    sem_t *sem_first;

    //le semaphore pour l'indice last
    sem_t *sem_last;

    //la file de memoire partage
    //nous avons choisi de un tableau de char car un char est code sur 1 octet ce qui simplifie enormement les indices first et last
    char liste[];
};


typedef struct MESSAGE MESSAGE;
struct MESSAGE {

    //les permissions d'ecriture
    int permission;

    //un pointeur vers la structure de la memoire partage
    Memoire_Partage *mp;
};