#include "STRUCTURE.c"

int msg_unlink(const char *name) {
    if (name == NULL) {
        perror("name empty");
        return -1;
    }
    if (strchr(name, '/') == NULL) {
        char *n = malloc(sizeof(char) * (strlen(name) + 1));
        strcpy(n, "/");
        name = strcat(n, name);
    } else if (strchr(name, '/') != name) {
        perror("error name with '/'");
        return -1;
    }

    if (shm_unlink(name) == -1) {
        perror("error shm unlink");
        return -1
    }
    return 0;

}

int msg_disconnect(MESSAGE *file) {
    if (file == NULL) {
        perror("error file null");
        return -1;
    }
    file->mp->nb_proc--;
    file = NULL;
    return 0;
}