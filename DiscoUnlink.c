#include "STRUCTURE.c"

int msg_unlink(const char* name) {
    Memoire_Partage mp;
    if (mp == NULL) {
        perror("error malloc");
        return -1;
    }

    int fd = shm_open(name, O_WRONLY, 0600);
    if (fd == -1) {
        perror("error open");
        return -1;
    }
    struct stat st;
    if (fstat(fd, &st) == -1) {
        perror("fstat");
        return -1;
    }
    mp = mmap(0, st.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    mp.acces = 1;
    shm_unlink(name);
}