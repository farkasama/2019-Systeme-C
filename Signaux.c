#include "Receive.c"

int msg_registrer_signal(MESSAGE* file, int sig) {
    if (file == NULL) {
        perror("file inutilisable");
        return -1;
    }
    if (file->mp == NULL) {
        perror("file detruite/inutilisable");
        return -1;
    }
    if (file->mp->pid != -1) {
        perror("un processus est deja enregistrer");
        return -1;
    }

    file->mp->pid = getpid();
    file->mp->sig = sig;

    return 0;
}

int msg_cancel_signal(MESSAGE* file) {
    if (file == NULL) {
        perror("file inutilisable");
        return -1;
    }
    if (file->mp == NULL) {
        perror("file detruite/inutilisable");
        return -1;
    }
    if (file->mp->pid == -1) {
        perror("pas de processus enregistrer");
        return -1;
    }
    if (file->mp->pid != getpid()){
        perror("pas autorise");
        return -1;
    }

    file->mp->pid = -1;
    file->mp->sig = 0;

    return 0;
}