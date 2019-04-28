#include "Connect.c"

int verification_send(MESSAGE *file, size_t len) {
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

int envoie(MESSAGE *file, const char *msg, size_t len) {
    int indice;
    int i;

    if (sem_wait(file->mp->sem_first) == -1) {
        perror("error semaphore wait send");
        return -1;
    }

    if (file->mp->first + len > file->mp->capacite * file->mp->longueur) {
        i = (file->mp->first + len) - (file->mp->capacite * file->mp->longueur);
    } else
        i = -1;

    if (file->mp->first == -1) {
        indice = file->mp->last;
        file->mp->first = (file->mp->last + len + 1) % (file->mp->capacite * file->mp->longueur);

    } else {
        indice = file->mp->first;
        file->mp->first = (file->mp->first + len + 1) % (file->mp->capacite * file->mp->longueur);
    }

    if (sem_post(file->mp->sem_first) == -1) {
        perror("error semaphore release send");
        return -1;
    }

    if (i != -1)
        memmove(file->mp->liste + indice, msg, len + 1);
    else {
        memmove(file->mp->liste + indice, msg, len - i);
        memmove(file->mp->liste, msg + len - i, i + 1);
    }
    if (msync(file->mp, file->mp->taille_fichier, MS_SYNC) == -1) {
        perror("error synchronisation envoie memoire partage");
        return -1;
    }

    return 0;
}

int msg_send(MESSAGE *file, const char *msg, size_t len) {

    if (verification_send(file, len) == -1) {
        return -1;
    }

    if (file->mp->first != -1) {
        while ((file->mp->first + len + 1) % (file->mp->capacite * file->mp->longueur) >= file->mp->last);
    }

    return envoie(file, msg, len);
}

int msg_trysend(MESSAGE *file, const char *msg, size_t len) {

    if (verification_send(file, len) == -1) {
        return -1;
    }

    if (file->mp->first != -1 &&
        (file->mp->first + len + 1) % (file->mp->capacite * file->mp->longueur) >= file->mp->last) {
        perror("file pleine");
        return -1;
    }

    return envoie(file, msg, len);
}


ssize_t msg_writev(MESSAGE *message, struct iovec *iov, int iovcnt) {
    if (message == NULL) {

        perror("file non utilisable");
        return -1;
    }
    if (message->mp == NULL) {
        perror("memoire partagée inutilisable");
        return -1;
    }
    int taille_msg = iovcnt - 1;

    for (int i = 0; i < iovcnt; ++i) {

        if (message->mp->longueur < iov[i].iov_len) {
            perror("longueur du message trop grande");
            return -1;
        }
        taille_msg += iov[i].iov_len;
    }

    char *str = malloc(taille_msg * sizeof(char));
    if (str == NULL) {
        perror("erreur malloc");
        return -1;

    }
    int res = 0;

    for (int j = 0; j < iovcnt - 1; ++j) {
        memcpy(str + res, iov[j].iov_base, iov[j].iov_len + 1);
        res += iov[j].iov_len;
    }
    memcpy(str + res, iov[iovcnt - 1].iov_base, iov[iovcnt - 1].iov_len);


    return msg_send(message, str, taille_msg);


}