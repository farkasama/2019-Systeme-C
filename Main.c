#include "Receive.c"


 int main(int argc, char *argv[]) {
    MESSAGE* m = msg_connect("test", O_RDWR, 10, 100);
    if (m == NULL) {
        exit(1);
    }
    int send = msg_send(m, "test OUI OUI", 99);
    printf("%d\n", send);

    char* message_r = malloc(100);
    msg_receive(m, message_r, 100);
    printf("%s", message_r);

    return 0;
}