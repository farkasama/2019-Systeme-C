//
// Created by etienne on 15/04/19.
//
#include "STRUCTURE.c"

MESSAGE* msg_connect(const char* nom, int options, size_t nb_message, size_t len_max) {
    MESSAGE* m;
    int perm;
    if ((options & O_RDONLY) == O_RDONLY) {
        m->permission = O_RDONLY;
        perm = PROT_READ;
    }
    else if ((options & O_WRONLY) == O_WRONLY) {
        m->permission = O_WRONLY;
        perm = PROT_WRITE;
    }
    else if ((options & O_RDWR) == O_RDWR) {
        m->permission = O_RDWR;
        perm = PROT_READ | PROT_WRITE;
    }
    else {
        perror("aucune permission accorde");
        return NULL;
    }

    int fd = open(nom, options);
    if (fd == -1) {
        perror("erreur open");
        return NULL;
    }

    if ((options & O_CREAT) != O_CREAT) {
        struct stat st;
        if (fstat(s, &st) == -1) {
            perror("fstat");
            return NULL;
        }
        int len = st.st_size;
        m->mp = mmap(NULL, len, perm, fd, 0);
        if (m->mp == MAP_FAILED) {
            perror("echec mmap");
            return NULL;
        }
        return m;
    }

    m->mp = mmap(NULL, sizeof(Memoire_Partage) + nb_message*len_max, perm, MAP_SHARED, 0);
    if (m->mp == MAP_FAILED) {
        perror("erreur mmap");
        return NULL;
    }
    m->mp->longueur = len_max;
    m->mp->capacite = nb_message;
    m->mp->first = -1;
    m->mp->last = 0;
    return m;

}