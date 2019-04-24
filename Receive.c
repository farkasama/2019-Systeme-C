//
// Created by will on 23/04/19.
//

#include "Connect.c"

ssize_t msg_receive(MESSAGE *file, void *msg, size_t len) {

    int indice;

    if (file->mp == NULL) {
        perror("pas d'acces a la file");
        return -1;
    }

    if (len < file->mp->longueur) {

        perror("taille maximale");
        errno = EMSGSIZE;
        return -1;
    }


    if ((file->permission & O_RDONLY) != O_RDONLY ||
        (file->permission & O_RDWR) != O_RDWR) {
        perror("il n'y a pas de permissions");
        return -1;
    }

    while (file->mp->last == file->mp->first);

    if (file->mp->last == -1) {

        indice = file->mp->first;
        file->mp->last = (file->mp->first + 1) % file->mp->capacite;

    } else {

        indice = file->mp->last;
        file->mp->last++;
    }

    if (file->mp->liste[indice] == NULL) {
        file->mp->liste[indice] = malloc(len);
    } else {

        file->mp->liste[indice] = realloc(file->mp->liste[indice], len);
    }
    memcpy(file->mp->liste[indice], msg, len);
    return 0;
}


ssize_t msg_tryreceive(MESSAGE *file, void *msg, size_t len) {
    int indice;

    if (file->mp == NULL) {
        perror("pas d'acces a la file");
        return -1;
    }

    if (len < file->mp->longueur) {
        perror("taille maximale");
        errno = EMSGSIZE;
        return -1;

    }


    if ((file->permission & O_RDONLY) != O_RDONLY ||
        (file->permission & O_RDWR) != O_RDWR) {
        perror("il n'y a pas de permissions");
        return -1;
    }

    if (file->mp->last == file->mp->first) {
        perror("file deja rempli");
        return -1;
    }

    if (file->mp->last == -1) {
        indice = file->mp->first;
        file->mp->last = (file->mp->first + 1) % file->mp->capacite;
    } else {

        indice = file->mp->last;
        file->mp->last++;
    }

    if (file->mp->liste[indice] == NULL) {
        file->mp->liste[indice] = malloc(len);
    } else {

        file->mp->liste[indice] = realloc(file->mp->liste[indice], len);
    }
    memcpy(file->mp->liste[indice], msg, len);

    return 0;
}

