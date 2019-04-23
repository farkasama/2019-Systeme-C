//
// Created by will on 23/04/19.
//
#include "STRUCTURE.c"

size_t msg_message_size(MESSAGE *m) {
    if (m == NULL) {
        perror("message inutilisable");
        return -1;
    }
    if (m->mp == NULL) {
        perror("memoire partagée inutilisable");
        return -1;
    }
    return m->mp->longueur;
}

size_t msg_capacite(MESSAGE *m) {
    if (m == NULL) {
        perror("message inutillisable ");
        return -1;
    }
    if (m->mp == NULL) {
        perror("memoire partagée inutilisable");
        return -1;
    }
    return m->mp->capacite;
}

size_t msg_nb(MESSAGE *m) {
    if (m == NULL) {
        perror("message inutilisable");
        return -1:
    }
    if (m->mp == NULL) {
        perror("message partagée inutlisable");
        return -1;
    }
    if (m->mp->first == -1) {
        return 0;
    }
    if (m->mp->first == m->mp->last) {
        return m->mp->capacite;
    }
    if (m->mp->first > m->mp->last) {
        return m->mp->first - m->mp->last;
    }
    return m->mp->first - m->mp->last + m->mp->capacite;
}