#include "STRUCTURE.c"

MESSAGE *msg_connect(const char *nom, int options, size_t nb_message, size_t len_max) {
    MESSAGE *m = malloc(sizeof(MESSAGE));
    if (m == NULL) {
        perror("malloc error");
        return NULL;
    }

    if (nom == NULL) {
        m->permission = O_RDWR;

        m->mp = mmap(NULL, sizeof(Memoire_Partage) + nb_message * len_max, PROT_READ | PROT_WRITE,
                     MAP_SHARED | MAP_ANONYMOUS, -1, 0);

        if (m->mp = MAP_FAILED) {
            perror("mmap anonyme");
            return NULL;
        }

        m->mp->first = -1;
        m->mp->last = 0;
        m->mp->capacite = nb_message;
        m->mp->longueur = len_max;
        //m->mp->liste[nb_message];

        if (sem_init(m->mp->sem_first, 0, 1) == -1) {
            perror("error init sem anonym first");
            return NULL;
        }

        if (sem_init(m->mp->sem_last, 0, 1) == -1) {
            perror("error init sem anonym last");
            return NULL;
        }
        msync(m->mp, sizeof(Memoire_Partage) + nb_message * len_max, MS_SYNC);
        return m;
    } else if (strchr(nom, '/') == NULL) {
        char *n = malloc(sizeof(char) * (strlen(nom) + 1));
        strcpy(n, "/");
        nom = strcat(n, nom);
    } else if (strchr(nom, '/') != nom) {
        perror("error name with '/'");
        return NULL;
    }

    if ((options & O_RDONLY) == O_RDONLY) {
        m->permission = O_RDONLY;
    }
    if ((options & O_WRONLY) == O_WRONLY) {
        m->permission = O_WRONLY;
    }
    if ((options & O_RDWR) == O_RDWR) {
        m->permission = O_RDWR;
    } else {
        perror("aucune permission accorde");
        return NULL;
    }

    int fd = shm_open(nom, options, 0600);
    if (fd == -1) {
        perror("erreur open");
        return NULL;
    }

    if ((options & O_CREAT) != O_CREAT) {

        struct stat st;
        if (fstat(fd, &st) == -1) {
            perror("fstat");
            return NULL;
        }

        int len = st.st_size;

        m->mp = malloc(len);
        m->mp = mmap(0, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

        if (m->mp == MAP_FAILED) {
            perror("echec mmap");
            return NULL;
        }


        char* name_sem = malloc(sizeof(char)*(strlen(nom)+6));
        strcpy(name_sem, nom);
        name_sem = strcat(name_sem, "_first");

        m->mp->sem_first = sem_open(name_sem, 0);
        if (m->mp->sem_first== SEM_FAILED) {
            perror("error creation semaphore first already exist");
            return NULL;
        }

        strcpy(name_sem, nom);
        name_sem = strcat(name_sem, "_last");
        m->mp->sem_last = sem_open(name_sem, 0);
        if (m->mp->sem_last == SEM_FAILED) {
            perror("error cration semaphore last already exist");
            return NULL;
        }
        return m;
    }

    int len = sizeof(Memoire_Partage) + nb_message * len_max;

    if (ftruncate(fd, len) == -1) {
        perror("ftruncate error");
        return NULL;
    }

    //m->mp = malloc(sizeof(Memoire_Partage));

    m->mp = mmap(0, sizeof(Memoire_Partage), PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);

    if (m->mp == MAP_FAILED) {
        perror("erreur mmap");
        return NULL;
    }

    m->mp->longueur = len_max;
    m->mp->nb_message = 0;
    m->mp->nb_message_max = nb_message;
    m->mp->first = -1;
    m->mp->last = 0;
    m->mp->taille_fichier = len;
    m->mp->pid = -1;
    m->mp->sig = 0;
    m->mp->liste[len_max*nb_message + nb_message*sizeof(size_t)];

    char* name_sem = malloc(sizeof(char)*(strlen(nom)+6));
    strcpy(name_sem, nom);
    name_sem = strcat(name_sem, "_first");
    m->mp->sem_first = sem_open(name_sem, O_CREAT, 0600, 1);
    if (m->mp->sem_first== SEM_FAILED) {
        perror("error creation semaphore first");
        return NULL;
    }

    strcpy(name_sem, nom);
    name_sem = strcat(name_sem, "_last");
    m->mp->sem_last = sem_open(name_sem, O_CREAT, 0600, 1);
    if (m->mp->sem_last == SEM_FAILED) {
        perror("error cration semaphore last");
        return NULL;
    }

    msync(m->mp, len, MS_SYNC);
    return m;

}