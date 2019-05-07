#include "Send.c"

//Fonction qui verifie si la file n'est pas detruite ou si il n'y a pas un probleme de permissions
ssize_t verification_receive(MESSAGE *file, size_t len) {

    //Si la file est inutilisable
    if (file == NULL) {
        perror("file inutilisable");
        return -1;
    }

    //Si la memoire partagee est inutilisable
    if (file->mp == NULL) {
        perror("pas d'acces a la file");
        return -1;
    }

    //Si la longueur pour stocker un message est plus petit que la taille maximale
    if (len < file->mp->longueur) {
        perror("taille maximale");
        errno = EMSGSIZE;
        return -1;
    }

    //Si il n'a pas les droits de lecture
    if ((file->permission & O_RDONLY) != O_RDONLY ||
        (file->permission & O_RDWR) != O_RDWR) {
        perror("il n'y a pas de permissions");
        return -1;
    }
}

/*************************************************************************************************************/

ssize_t reception(MESSAGE *file, void *msg, size_t len) {
    int indice;
    size_t *taille = malloc(sizeof(size_t));

    //On commence par ouvrir le semaphore de last
    if (sem_wait(file->mp->sem_last) == -1) {
        perror("error semaphore wait last reception");
        return -1;
    }

    //on prends l'indice de last
    indice = file->mp->last;

    //On copie la taille du premier message
    memmove(taille, file->mp->liste + file->mp->last, sizeof(size_t));

    //On modifie last en ajoutant le taille d'un size_t + la longueur du message
    file->mp->last += sizeof(size_t) + *taille;

    //Il y a un message en moins
    file->mp->nb_message--;

    //Il y a aussi un processus en attente en moins
    file->mp->nb_proc_att--;

    //On peut liberer le semaphore
    if (sem_post(file->mp->sem_last) == -1) {
        perror("error semaphore release last reception");
        return -1;
    }

    //On alloue bien de la memoire a la variable passe en argument
    if (msg == NULL) {
        msg = malloc(len * sizeof(char));
    } else {
        msg = realloc(msg, len);
    }

    //On copie dans dans la variable
    memmove(msg, file->mp->liste + indice + sizeof(size_t), *taille);

    //On synchronise la memoire partagee
    if (msync(file->mp, file->mp->taille_fichier, MS_SYNC) == -1) {
        perror("error synchronisation reception memoire partage");
        return -1;
    }

    //On retourne 0 si il y a eu aucune erreur
    return 0;
}

/*************************************************************************************************************
                            MSG_RECEIVE ET TRY_RECEIVE
*************************************************************************************************************/

ssize_t msg_receive(MESSAGE *file, void *msg, size_t len) {

    //On verifie qu'il n'y a pas d'erreur
    verification_receive(file, len);

    //On ajoute le processus en attente
    file->mp->nb_proc_att++;

    //On attends si la file est vide
    while (file->mp->nb_message == 0);

    //Et on recoit le message
    return reception(file, msg, len);
}


ssize_t msg_tryreceive(MESSAGE *file, char *msg, size_t len) {

    //On verifie qu'il n'y a pas d'erreur
    verification_receive(file, len);

    //Si la file est vide on retourne une erreur
    if (file->mp->nb_message == 0) {
        perror("file empty");
        return -1;
    }

    //Sinon on retourne le message
    return reception(file, msg, len);
}

/*************************************************************************************************************/

ssize_t msg_readv(MESSAGE *message, struct iovec *iov, int count) {
    if (message->mp->nb_message == 0) {
        perror("file vide");
        return -1;
    }
    if (sem_wait(message->mp->sem_last) == -1) {
        perror("error semaphore");
        return -1;
    }
    ssize_t taille = 0;

    for (int i = 0; i < count || i < message->mp->nb_message; ++i) {
        size_t *t = malloc(sizeof(size_t));
        memmove(t, message->mp->liste + message->mp->last, sizeof(size_t));
        iov[i].iov_len = *t;
        iov[i].iov_base = malloc(*t);
        memmove(iov[i].iov_base, message->mp->liste + message->mp->last + sizeof(size_t), *t);
        message->mp->last = sizeof(size_t) + *t;
        taille += *t;

    }
    if (sem_post(message->mp->sem_last) == -1) {
        perror("error semaphore");
        return -1;
    }
    return taille;

}