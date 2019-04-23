#include "Connect.c"

int verification(MESSAGE* file, size_t len) {
    if (file == NULL) {
        perror("message inutilisable");
        return -1;
    }

    if (file->mp == NULL) {
        perror("file detruite/inutilisable");
        return -1;
    }

    if (len > file->mp->longueur) {
        perror("longueur trop grande");
        errno = EMSGSIZE;
        return -1;
    }

    if (!((file->permission & O_WRONLY) == O_WRONLY ||
        (file->permission & O_RDWR) == O_RDWR)) {
        perror("pas les permissions");
        return -1;
    }
    return 0;
}

int envoie (MESSAGE* file, const char* msg, size_t len) {
    int indice;

    if (file->mp->first == -1) {
        indice = file->mp->last;
        file->mp->first = (file->mp->last+1)%file->mp->capacite;
    }
    else  {
        indice = file->mp->first;
        file->mp->first = (file->mp->first+1)%file->mp->capacite;
    }


    if (file->mp->liste[indice] == NULL) {
        file->mp->liste[indice] = malloc(len);
        if (file->mp->liste[indice] == NULL) {
            perror("error malloc");
            return -1;
        }
    }
    else {
        file->mp->liste[indice] = realloc(file->mp->liste[indice], len);
        if (file->mp->liste[indice] == NULL) {
            perror("error realloc");
            return -1;
        }
    }
    memcpy(file->mp->liste[indice], msg, len);
    return 0;
}

int msg_send(MESSAGE* file, const char* msg, size_t len) {

    if (verification(file, len) == -1) {
        return -1;
    }

    while(file->mp->first == file->mp->last);

    if (envoie(file, msg, len) == -1) {
        return -1;
    }

    return 0;
}

int msg_trysend(MESSAGE* file, const char* msg, size_t len) {

    if (verification(file, len) == -1) {
        return -1;
    }

    if (file->mp->first == file->mp->last) {
        perror("file pleine");
        return -1;
    }

    if (envoie(file, msg, len) == -1) {
        return -1;
    }

    return 0;
}