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

int envoie(MESSAGE *file, const void *msg, size_t len) {
    int indice;

    if (sem_wait(file->mp->sem_first) == -1) {
        perror("error semaphore wait first send");
        return -1;
    }

    if (file->mp->first + sizeof(size_t) + len > file->mp->nb_message * file->mp->longueur) {
        if (sem_wait(file->mp->sem_last) == -1) {
            perror("error semaphore wait last send");
            return -1;
        }

        if (file->mp->first == -1) {
            file->mp->last = 0;
        }
        else {
            memmove(file->mp->liste, file->mp->liste + file->mp->last, file->mp->first - file->mp->last);
            file->mp->last = 0;
            file->mp->first = file->mp->first - file->mp->last;
        }

        if (sem_post(file->mp->sem_last) == -1) {
            perror("error semaphore release last send");
            return -1;
        }
    }

    if (file->mp->first == -1) {
        indice = file->mp->last;
        file->mp->first = file->mp->last + len + sizeof(size_t);

    } else {
        indice = file->mp->first;
        file->mp->first = file->mp->first + len + sizeof(size_t);
    }

    if (sem_post(file->mp->sem_first) == -1) {
        perror("error semaphore release first send");
        return -1;
    }


    memmove(file->mp->liste + indice, &len, sizeof(size_t));
    memmove(file->mp->liste + indice + sizeof(size_t), msg, len);

    if (file->mp->nb_message == 0 && file->mp->nb_proc_att == 0 && file->mp->pid != -1) {
        kill(file->mp->pid, file->mp->sig);
        file->mp->pid = -1;
        file->mp->sig = 0;
    }

    file->mp->nb_message++;

    if (msync(file->mp, file->mp->taille_fichier, MS_SYNC) == -1) {
        perror("error synchronisation envoie memoire partage");
        return -1;
    }

    return 0;
}

int msg_send(MESSAGE *file, const void *msg, size_t len) {

    if (verification_send(file, len) == -1) {
        return -1;
    }

    while (file->mp->nb_message == file->mp->nb_message_max);

    return envoie(file, msg, len);
}

int msg_trysend(MESSAGE *file, const void *msg, size_t len) {

    if (verification_send(file, len) == -1) {
        return -1;
    }

    if (file->mp->nb_message == file->mp->nb_message_max) {
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