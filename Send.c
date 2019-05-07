#include "Connect.c"

//Fonction qui verifie si la file n'est pas detruite ou si il n'y a pas un probleme de permissions
int verification_send(MESSAGE *file, size_t len) {

    //Si la file est detruite
    if (file == NULL) {
        perror("message inutilisable");
        return -1;
    }

    //Si il y a un probleme sur la memoire partage
    if (file->mp == NULL) {
        perror("file detruite/inutilisable");
        return -1;
    }

    //Si la longueur du message est superieur a la longueur maximale autorise par la file
    if (len > file->mp->longueur) {
        perror("longueur trop grande");
        errno = EMSGSIZE;
        return -1;
    }

    //Si la file n'a pas les droits d'ecriture
    if (!((file->permission & O_WRONLY) == O_WRONLY ||
          (file->permission & O_RDWR) == O_RDWR)) {
        perror("pas les permissions");
        return -1;
    }

    return 0;
}

/*************************************************************************************************************/

int envoie(MESSAGE *file, const void *msg, size_t len) {
    int indice;

    //On demande l'acces au semaphore du premier octet libre et on verifie qu'il n'y a pas de probleme
    if (sem_wait(file->mp->sem_first) == -1) {
        perror("error semaphore wait first send");
        return -1;
    }

    //Si l'emplacement du premier octet libre + la taille du message depasse la longueur maximale
    if (file->mp->first + sizeof(size_t) + len > file->mp->nb_message * file->mp->longueur) {

        //On demande egalement l'acces a last pour les modifications
        if (sem_wait(file->mp->sem_last) == -1) {
            perror("error semaphore wait last send");
            return -1;
        }

        //Si la file est vide, cela le veut dire que last est trop loin, nous devons juste le mettre a 0
        if (file->mp->nb_message == 0) {
            file->mp->first = 0;
            file->mp->last = 0;
        }
            //Sinon on doit deplacer tous les messages (donc de first-last) qui commencent a last au debut de la file
        else {
            memmove(file->mp->liste, file->mp->liste + file->mp->last, file->mp->first - file->mp->last);
            //le premier octect libre se trouve desormais a first-last
            file->mp->first = file->mp->first - file->mp->last;
            //le premier message se trouve maintenant a la position 0
            file->mp->last = 0;
        }

        //on libere le semaphore de last
        if (sem_post(file->mp->sem_last) == -1) {
            perror("error semaphore release last send");
            return -1;
        }
    }

    //On prends l'indice de first et ensuite on lui ajoute la taille du message
    indice = file->mp->first;
    file->mp->first = file->mp->first + len + sizeof(size_t);

    //On copie d'abord la longueur du message dans la file
    memmove(file->mp->liste + indice, &len, sizeof(size_t));

    //on copie le message dans la file
    memmove(file->mp->liste + indice + sizeof(size_t), msg, len);

    //On libere le semaphore de first pour permettre a un autre processus d'ecrire
    //Liberer le semaphore est une maniere viable SEULEMENT avec un tableau circulaire
    //Etant donne que nous utilisons la deuxieme methode (le "shift"), nous avons peur que si nous liberons le semaphore avant la copie et que la longueur du prochain message depasse la capacite maxiamle de la file, le nouveau message que nous sommes en train de copie ne sera pas "shifte" et donc il y aura une erreur
    if (sem_post(file->mp->sem_first) == -1) {
        perror("error semaphore release first send");
        return -1;
    }

    //Si la file de message est vide, il n'y a aucun processus en attente de recevoir un message et un processus est en attente d'un signal alors on lui en envoie un
    //On verifie qu'il n'y a pas d'erreur et on supprime apres le proc en attente
    if (file->mp->nb_message == 0 && file->mp->nb_proc_att == 0 && file->mp->pid != -1) {
        if (kill(file->mp->pid, file->mp->sig) == -1) {
            perror("error envoie signal");
            return -1;
        }
        file->mp->pid = -1;
        file->mp->sig = 0;
    }

    //On incremente le nombre de message
    file->mp->nb_message++;

    //On synchronise la memoire partagee en verifiant qu'il n'y a pas d'erreur
    if (msync(file->mp, file->mp->taille_fichier, MS_SYNC) == -1) {
        perror("error synchronisation envoie memoire partage");
        return -1;
    }

    //On retourne qu'il n'y a pas d'erreur
    return 0;
}


/*************************************************************************************************************
                            MSG_SEND ET TRY_SEND
*************************************************************************************************************/

int msg_send(MESSAGE *file, const void *msg, size_t len) {

    //On verifie s'il y n'y a pas d'erreur
    if (verification_send(file, len) == -1) {
        return -1;
    }

    //Si la fil est est pleine on attends
    while (file->mp->nb_message == file->mp->nb_message_max);

    //On si il y a eu une erreur pour l'envoie ou si il y en a pas eu
    return envoie(file, msg, len);
}

int msg_trysend(MESSAGE *file, const void *msg, size_t len) {

    //On verifie qu'il n'y a pas d'erreur
    if (verification_send(file, len) == -1) {
        return -1;
    }

    //Si la file est pleine on retourne une erreur
    if (file->mp->nb_message == file->mp->nb_message_max) {
        perror("file pleine");
        return -1;
    }

    //On si il y a eu une erreur pour l'envoie ou si il y en a pas eu
    return envoie(file, msg, len);
}

/*************************************************************************************************************/


ssize_t msg_writev(MESSAGE *file, struct iovec *iov, int iovcnt) {

    //Si le nombre de message a envoye est plus grand que la capacite de la file alors on retourne une erreur
    if (iovcnt > file->mp->nb_message_max) {
        perror("nombre de message trop grand");
        return -1;
    }

    //on initialise la taille de tous les message
    int taille_msg = sizeof(size_t) * iovcnt;

    for (int i = 0; i < iovcnt; ++i) {

        //Si un messga est trop grand alors on retourne une erreur
        if (file->mp->longueur < iov[i].iov_len) {
            perror("longueur du message trop grande");
            return -1;
        }
        //Sinon on ajour la longueur du message
        taille_msg += iov[i].iov_len;
    }

    //On initialise le message a envoyer et on verifie si il n'y a pas d'erreur
    void *str = malloc(taille_msg);
    if (str == NULL) {
        perror("erreur malloc");
        return -1;
    }

    int res = 0;

    //On copie tous les messages avec leurs longueur dans str
    for (int j = 0; j < iovcnt; ++j) {
        memmove(str + res, &(iov[j].iov_len), sizeof(size_t));
        memmove(str + res + sizeof(size_t), iov[j].iov_base, iov[j].iov_len);
        res += iov[j].iov_len + sizeof(size_t);
    }

    //Ayant verifie prealablement avant que chaque message est inferieur a la longueur maximale d'un message dans la file, il nous reste uniquement a faire les autres verifications (file supprimer ou permission)
    verification_send(file, 0);

    while (file->mp->nb_message + iovcnt >= file->mp->nb_message_max);

    int indice;

    //On demande l'acces au semaphore du premier octet libre et on verifie qu'il n'y a pas de probleme
    if (sem_wait(file->mp->sem_first) == -1) {
        perror("error semaphore wait first send");
        return -1;
    }

    //Si l'emplacement du premier octet libre + la taille du message depasse la longueur maximale
    if (file->mp->first + res > file->mp->nb_message * file->mp->longueur) {

        //On demande egalement l'acces a last pour les modifications
        if (sem_wait(file->mp->sem_last) == -1) {
            perror("error semaphore wait last send");
            return -1;
        }

        //Si la file est vide, cela le veut dire que last est trop loin, nous devons juste le mettre a 0
        if (file->mp->nb_message == 0) {
            file->mp->first = 0;
            file->mp->last = 0;
        }
            //Sinon on doit deplacer tous les messages (donc de first-last) qui commencent a last au debut de la file
        else {
            memmove(file->mp->liste, file->mp->liste + file->mp->last, file->mp->first - file->mp->last);
            //le premier octect libre se trouve desormais a first-last
            file->mp->first = file->mp->first - file->mp->last;
            //le premier message se trouve maintenant a la position 0
            file->mp->last = 0;
        }

        //on libere le semaphore de last
        if (sem_post(file->mp->sem_last) == -1) {
            perror("error semaphore release last send");
            return -1;
        }
    }

    //On prends l'indice de first et on lui ajoute tous les messages
    indice = file->mp->first;
    file->mp->first = file->mp->first + res;

    //on copie les messages dans la file
    memmove(file->mp->liste + indice, str, res);

    //On libere le semaphore de first pour permettre a un autre processus d'ecrire
    //Liberer le semaphore est une maniere viable SEULEMENT avec un tableau circulaire
    //Etant donne que nous utilisons la deuxieme methode (le "shift"), nous avons peur que si nous liberons le semaphore avant la copie et que la longueur du prochain message depasse la capacite maxiamle de la file, le nouveau message que nous sommes en train de copie ne sera pas "shifte" et donc il y aura une erreur
    if (sem_post(file->mp->sem_first) == -1) {
        perror("error semaphore release first send");
        return -1;
    }

    //Si la file de message est vide, il n'y a aucun processus en attente de recevoir un message et un processus est en attente d'un signal alors on lui en envoie un
    //On verifie qu'il n'y a pas d'erreur et on supprime apres le proc en attente
    if (file->mp->nb_message == 0 && file->mp->nb_proc_att == 0 && file->mp->pid != -1) {
        if (kill(file->mp->pid, file->mp->sig) == -1) {
            perror("error envoie signal");
            return -1;
        }
        file->mp->pid = -1;
        file->mp->sig = 0;
    }

    //On incremente le nombre de message
    file->mp->nb_message += iovcnt;

    //On synchronise la memoire partagee en verifiant qu'il n'y a pas d'erreur
    if (msync(file->mp, file->mp->taille_fichier, MS_SYNC) == -1) {
        perror("error synchronisation envoie memoire partage");
        return -1;
    }

    //On retourne qu'il n'y a pas d'erreur
    return 0;
}