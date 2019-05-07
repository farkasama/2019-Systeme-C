#include "STRUCTURE.c"

size_t msg_message_size(MESSAGE *m) {

    //Si la file est inutilisable
    if (m == NULL) {
        perror("message inutilisable");
        return -1;
    }

    //Si la memoire partager est inutilisable
    if (m->mp == NULL) {
        perror("memoire partagée inutilisable");
        return -1;
    }

    //retourne la longueur
    return m->mp->longueur;
}

size_t msg_capacite(MESSAGE *m) {

    //Si la file est inutilisable
    if (m == NULL) {
        perror("message inutilisable");
        return -1;
    }

    //Si la memoire partager est inutilisable
    if (m->mp == NULL) {
        perror("memoire partagée inutilisable");
        return -1;
    }

    //retourne le nombre maximal
    return m->mp->nb_message_max;
}

size_t msg_nb(MESSAGE *m) {
    //Si la file est inutilisable
    if (m == NULL) {
        perror("message inutilisable");
        return -1;
    }

    //Si la memoire partager est inutilisable
    if (m->mp == NULL) {
        perror("memoire partagée inutilisable");
        return -1;
    }

    //Retourne le nombre de message dans la file
    return m->mp->nb_message;
}