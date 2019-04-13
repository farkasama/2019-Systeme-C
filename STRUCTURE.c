typedef struct Memoire_Partage Memoire_Partage;
struct Memoire_Partage {

    size_t longueur;
    size_t capacite;
    int first;
    int last;
    //semaphore
    void* liste[];
};

typedef struct MESSAGE MESSAGE;

struct MESSAGE {
    int type;
    void* mp;
};