#include "Send.c"


 int main(int argc, char *argv[]) {
    MESSAGE* m = msg_connect("test", O_RDWR|O_CREAT, 10, 100);
    if (m == NULL) {
        exit(1);
    }
    int send = msg_send(m, "test", sizeof(char)*4);
    printf("%d\n", send);
    return 0;
}