#include "Connect.c"

int msg_send(MESSAGE* file, const char* msg, size_t len) {

    if (file->mp == NULL) {
        perror("file detruite/inutilisable");
        return -1;
    }

    if (len > file->mp->longueur) {
        perror("longueur trop grande");
        errno = EMSGSIZE;
        return -1;
    }

    while(file->mp->first == file->mp->last);

    int indice;

    if (file->mp->first == -1) {
        indice = file->mp->last;
        file->mp->first = (file->mp->last+1)%file->mp->capacite;
    }
    else  {
        indice = file->mp->first;
        file->mp->first++;
    }


    if (file->mp->liste[indice] == NULL) {
        file->mp->liste[indice] = malloc(len);
    }
    else {
        file->mp->liste[indice] = realloc(file->mp->liste[indice], len);
    }
    memcpy(file->mp->liste[indice], msg, len);
    return 0;
}

int msg_trysend(MESSAGE* file, const char* msg, size_t len) {

    if (file->mp == NULL) {
        perror("file detruite/inutilisable");
        return -1;
    }

    if (len > file->mp->longueur) {
        perror("longueur trop grande");
        errno = EMSGSIZE;
        return -1;
    }

    if (file->mp->first == file->mp->last) {
        perror("file pleine");
        return -1;
    }

    int indice;

    if (file->mp->first == -1) {
        indice = file->mp->last;
        file->mp->first = (file->mp->last+1)%file->mp->capacite;
    }
    else  {
        indice = file->mp->first;
        file->mp->first++;
    }


    if (file->mp->liste[indice] == NULL) {
        file->mp->liste[indice] = malloc(len);
    }
    else {
        file->mp->liste[indice] = realloc(file->mp->liste[indice], len);
    }
    memcpy(file->mp->liste[indice], msg, len);
    return 0;
}