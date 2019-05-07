#include "STRUCTURE.c"

MESSAGE *msg_connect(const char *nom, int options, size_t nb_message, size_t len_max) {

    //On alloue de la memoire pour la structure message et on verifie si il y a uen erreur
    MESSAGE *m = malloc(sizeof(MESSAGE));
    if (m == NULL) {
        perror("malloc error");
        return NULL;
    }

    //si la file choisi est une file anonyme
    if (nom == NULL) {

        //Les permissions d'une file anonyme sont automatiquement lecture et ecriture
        m->permission = O_RDWR;

        //on creer la memoire partager en anonyme
        m->mp = mmap(NULL, sizeof(Memoire_Partage) + nb_message * len_max + nb_message * sizeof(size_t), PROT_READ | PROT_WRITE,
                     MAP_SHARED | MAP_ANONYMOUS, -1, 0);

        //On verifie si le mmap a bien fonctionner
        if (m->mp = MAP_FAILED) {
            perror("mmap anonyme");
            return NULL;
        }

        //On initialise la structure
        m->mp->longueur = len_max;
        m->mp->nb_message = 0;
        m->mp->nb_message_max = nb_message;
        m->mp->first = -1;
        m->mp->last = 0;
        m->mp->taille_fichier = sizeof(Memoire_Partage) + nb_message * len_max + nb_message * sizeof(size_t);
        m->mp->pid = -1;
        m->mp->sig = 0;

        //on fait bien attention a initialise le tableau avec le nombre de message que la file peut stocker et la longueur des messages
        m->mp->liste[len_max * nb_message + nb_message * sizeof(size_t)];

        //on initialise le semaphore pour first en anonyme et on verifie si il y a une erreur
        if (sem_init(m->mp->sem_first, 0, 1) == -1) {
            perror("error init sem anonym first");
            return NULL;
        }

        //on initialise le semaphore pour last en anonyme et on verifie si il y a une erreur
        if (sem_init(m->mp->sem_last, 0, 1) == -1) {
            perror("error init sem anonym last");
            return NULL;
        }

        //On verifie au'il n'y a pas d'erreur en synchronisant la memoire partagee et on retourne m
        if (msync(m->mp, sizeof(Memoire_Partage) + nb_message * len_max + nb_message * sizeof(size_t), MS_SYNC) == -1) {
            perror("error synchronisation envoie memoire partage");
            return NULL;
        }

        //on retourn la structure MESSAGE creer
        return m;
    }

    //Sinon si la structure demande n'est pas une file anonyme

    //Si le nom de la structure demande ne commence par par '/' alors on lui ajoute au debut
    else if (strchr(nom, '/') == NULL) {
        char *n = malloc(sizeof(char) * (strlen(nom) + 1));
        strcpy(n, "/");
        nom = strcat(n, nom);
    }
    //Sinon si le nom de la structure contient le caractere '/' mais ne commence pas par celui la alors retourn une erreur
    else if (strchr(nom, '/') != nom) {
        perror("error name with '/'");
        return NULL;
    }

    //On enregistre les droits de lecture ou/et d'ecriture
    //Si les droits passe en argument sont de lecture
    if ((options & O_RDONLY) == O_RDONLY) {
        m->permission = O_RDONLY;
    }
    //Sinon si ils sont d'ecriture
    if ((options & O_WRONLY) == O_WRONLY) {
        m->permission = O_WRONLY;
    }
    //Sinon si ils sont les deux
    if ((options & O_RDWR) == O_RDWR) {
        m->permission = O_RDWR;
    }
    //Si s'en ai aucun (ce qui est impossible mais on ne sait jamais)
    else {
        perror("aucune permission accorde");
        return NULL;
    }

    //On ouvre un objet memoire avec les options passe en arguement, en lecture et en ecriture
    //On verifie egalement si il y a une erreur
    int fd = shm_open(nom, options, 0600);
    if (fd == -1) {
        perror("erreur open");
        return NULL;
    }

    //Si la memoire partage doit deja exister
    if ((options & O_CREAT) != O_CREAT) {

        //On recupere la taille de l'objet memoire et on verifie si il n'y a pas d'erreur
        struct stat st;
        if (fstat(fd, &st) == -1) {
            perror("fstat");
            return NULL;
        }

        //On recupere la taille
        int len = st.st_size;

        //On alloue de la memoire et on verifie qu'il n'y a pas d'erreur
        m->mp = malloc(len);
        if (m->mp == NULL) {
            perror("malloc");
            return NULL;
        }

        //On ouvre la memoire partage et on verifie qu'il n'y a pas d'erreur
        m->mp = mmap(0, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

        if (m->mp == MAP_FAILED) {
            perror("echec mmap");
            return NULL;
        }

        //On cree un nouveau nom pour le semaphore de first avec le nom de la file + "_first"
        char *name_sem = malloc(sizeof(char) * (strlen(nom) + 6));
        strcpy(name_sem, nom);
        name_sem = strcat(name_sem, "_first");

        //On ouvre le semaphore et on verifie qu'il n'y a pas d'erreur
        m->mp->sem_first = sem_open(name_sem, 0);
        if (m->mp->sem_first == SEM_FAILED) {
            perror("error creation semaphore first already exist");
            return NULL;
        }

        //On cree un nouveau nom pour le semaphore de last avec le nom de la file + "_last"
        strcpy(name_sem, nom);
        name_sem = strcat(name_sem, "_last");

        //On ouvre le semaphore et on verifie au'il n'y a pas d'erreur
        m->mp->sem_last = sem_open(name_sem, 0);
        if (m->mp->sem_last == SEM_FAILED) {
            perror("error cration semaphore last already exist");
            return NULL;
        }

        //On retourne la file cree
        return m;
    }

    //Sinon si on doit creer une structure

    //On stock la longueur du fichier
    int len = sizeof(Memoire_Partage) + nb_message * len_max + nb_message * sizeof(size_t);

    //On creer plus d'espace sur le nouvel objet memoire creer
    if (ftruncate(fd, len) == -1) {
        perror("ftruncate error");
        return NULL;
    }

    //On ouvre la memoire partage et on verifie qu'il n'y a pas d'erreur
    m->mp = mmap(0, len, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    if (m->mp == MAP_FAILED) {
        perror("erreur mmap");
        return NULL;
    }

    //On initialise les valeurs
    m->mp->longueur = len_max;
    m->mp->nb_message = 0;
    m->mp->nb_message_max = nb_message;
    m->mp->first = -1;
    m->mp->last = 0;
    m->mp->taille_fichier = len;
    m->mp->pid = -1;
    m->mp->sig = 0;

    //on fait bien attention a initialise le tableau avec le nombre de message que la file peut stocker et la longueur des messages
    m->mp->liste[len_max * nb_message + nb_message * sizeof(size_t)];

    //On cree un nom pour le semaphore de first
    char *name_sem = malloc(sizeof(char) * (strlen(nom) + 6));
    strcpy(name_sem, nom);
    name_sem = strcat(name_sem, "_first");

    //On ouvre ce semaphore avec une place et on verifie qu'il n'y a pas d'erreur
    m->mp->sem_first = sem_open(name_sem, O_CREAT, 0600, 1);
    if (m->mp->sem_first == SEM_FAILED) {
        perror("error creation semaphore first");
        return NULL;
    }

    //On cree un nom pour le semaphore de last
    strcpy(name_sem, nom);
    name_sem = strcat(name_sem, "_last");

    //On ouvre ce semaphore avec une place et on verifie qu'il n'y a pas d'erreur
    m->mp->sem_last = sem_open(name_sem, O_CREAT, 0600, 1);
    if (m->mp->sem_last == SEM_FAILED) {
        perror("error cration semaphore last");
        return NULL;
    }

    //On verifie au'il n'y a pas d'erreur en synchronisant la memoire partagee et on retourne m
    if (msync(m->mp, len, MS_SYNC) == -1) {
        perror("error synchronisation envoie memoire partage");
        return NULL;
    }
    return m;

}