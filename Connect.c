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

#include "STRUCTURE.c"

MESSAGE* msg_connect(const char* nom, int options, size_t nb_message, size_t len_max) {
    MESSAGE* m = malloc(sizeof(MESSAGE));
    if (m == NULL) {
        perror("malloc error");
        return NULL;
    }

    if ((options & O_RDONLY) == O_RDONLY) {
        m->permission = O_RDONLY;
    }
    else if ((options & O_WRONLY) == O_WRONLY) {
        m->permission = O_WRONLY;
    }
    else if ((options & O_RDWR) == O_RDWR) {
        m->permission = O_RDWR;
    }
    else {
        perror("aucune permission accorde");
        return NULL;
    }

    int fd = open(nom, options, 0600);
    if (fd == -1) {
        perror("erreur open");
        return NULL;
    }

    if ((options & O_CREAT) != O_CREAT) {
        struct stat st;
        if (fstat(fd, &st) == -1) {
            perror("fstat");
            return NULL;
        }
        int len = st.st_size;
        m->mp = malloc(len);
        m->mp = mmap(NULL, len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
        if (m->mp == MAP_FAILED) {
            perror("echec mmap");
            return NULL;
        }
        return m;
    }

    int len = sizeof(Memoire_Partage) + nb_message*len_max;

    if (ftruncate(fd, len) == -1) {
        perror("ftruncate error");
        return NULL;
    }

    //m->mp = malloc(sizeof(Memoire_Partage));

    m->mp = mmap(0, len, PROT_WRITE|PROT_READ, MAP_SHARED, fd, 0);

    if (m->mp == MAP_FAILED) {
        perror("erreur mmap");
        return NULL;
    }
    m->mp->longueur = len_max;
    m->mp->capacite = nb_message;
    m->mp->first = -1;
    m->mp->last = 0;
    m->mp->liste[nb_message];
    msync(m->mp, len, MS_SYNC);
    return m;

}