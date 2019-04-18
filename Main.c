#include "Connect.c"


 int main(int argc, char *argv[]) {
    MESSAGE* m = msg_connect("test", O_RDWR|O_CREAT|O_EXCL, 10, 10);
    printf("%zu\n", m->mp->longueur);
    return 0;
}