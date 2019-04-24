//
// Created by will on 23/04/19.
//

#include "Send.c"

ssize_t verification_receive(MESSAGE* file, size_t len) {
    if (file == NULL) {
        perror("file inutilisable");
        return -1;
    }

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
}

ssize_t reception(MESSAGE* file, char *msg, size_t len) {
    int indice;

    if (sem_wait(file->mp->sem_last) == -1) {
        perror("error semaphore wait last reception");
        return -1;
    }
    indice = file->mp->last;
    file->mp->last = (file->mp->last+1)%file->mp->capacite;
    if (sem_post(file->mp->sem_last) == -1) {
        perror("error semaphore release last reception");
        return -1;
    }

    if (sem_wait(file->mp->sem_first) == -1) {
        perror("error semaphore wait first reception");
        return -1;
    }
    if (file->mp->last == file->mp->first) {
        file->mp->first = -1;
    }
    if (sem_post(file->mp->sem_first) == -1) {
        perror("error semaphore release first reception");
        return -1;
    }

    if (msg == NULL) {
        msg = malloc(len);
    } else {
        msg = realloc(msg, len);
    }
    memcpy(msg, file->mp->liste[indice], len);

    free(file->mp->liste[indice]);
    return sizeof(char)*strlen(msg);
}

ssize_t msg_receive(MESSAGE *file, char *msg, size_t len) {

    verification_receive(file, len);

    while (file->mp->first == -1);

    return reception(file, msg, len);
}


ssize_t msg_tryreceive(MESSAGE *file, char *msg, size_t len) {

    verification_receive(file, len);

    if (file->mp->first == -1) {
        perror("file empty");
        return -1;
    }

    return reception(file, msg, len);
}

