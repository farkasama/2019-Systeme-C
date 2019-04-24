#include "Connect.c"

int verification_send(MESSAGE* file, size_t len) {
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

    if (sem_wait(file->mp->sem_first) == -1) {
        perror("error semaphore wait send");
        return -1;
    }
    if (file->mp->first == -1) {
        indice = file->mp->last;
        file->mp->first = (file->mp->last+1)%file->mp->capacite;
        //printf("%d\n", indice);
    }
    else  {
        indice = file->mp->first;
        file->mp->first = (file->mp->first+1)%file->mp->capacite;
    }
    if (sem_post(file->mp->sem_first) == -1) {
        perror("error semaphore release send");
        return -1;
    }


    if (file->mp->liste[indice] == NULL) {
        file->mp->liste[indice] = malloc(len);
        if (file->mp->liste[indice] == NULL) {
            perror("error malloc");
            return -1;
        }
    }
    memcpy(file->mp->liste[indice], msg, len);
    return 0;
}

int msg_send(MESSAGE* file, const char* msg, size_t len) {

    if (verification_send(file, len) == -1) {
        return -1;
    }

    while(file->mp->first == file->mp->last);

    return envoie(file, msg, len);
}

int msg_trysend(MESSAGE* file, const char* msg, size_t len) {

    if (verification_send(file, len) == -1) {
        return -1;
    }

    if (file->mp->first == file->mp->last) {
        perror("file pleine");
        return -1;
    }

    return envoie(file, msg, len);
}