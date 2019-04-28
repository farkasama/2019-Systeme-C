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
    int i = 0;

    if (sem_wait(file->mp->sem_last) == -1) {
        perror("error semaphore wait last reception");
        return -1;
    }

    indice = file->mp->last;

    for (int j = 0; j < file->mp->capacite*file->mp->longueur; j++) {
        if (*(file->mp->liste+indice+j) == '\0') {
            i = -1;
            break;
        }
        else
            i++;
    }

    if (i == -1) {
        file->mp->last = file->mp->last+strlen(file->mp->liste)+1;
    }
    else {
        file->mp->last = file->mp->last+strlen(file->mp->liste+indice)+1;
    }

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
        msg = malloc(len*sizeof(char));
    } else {
        msg = realloc(msg, len*sizeof(char));
    }

    if (i == -1) {
        memmove(msg, file->mp->liste+indice, len-i);
        memmove(msg+len-i, file->mp->liste, i+1);
    }
    else {
        memmove(msg, file->mp->liste+indice, len+1);
    }

    if (msync(file->mp, file->mp->taille_fichier, MS_SYNC) == -1) {
        perror("error synchronisation reception memoire partage");
        return -1;
    }

    return strlen(msg)+1;
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

ssize_t msg_readv(MESSAGE *message, struct *iov, int iovcnt){



}