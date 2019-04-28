#include "Receive.c"


int main(int argc, char *argv[]) {

    int pid = fork();

    if (pid == 0) {
        MESSAGE *m = msg_connect("test", O_RDWR | O_CREAT, 10, 100);
        if (m == NULL) {
            exit(1);
        }
        char *texte = "test OUI OUI";
        int a = 5;
        int send = envoie(m, &a, sizeof(a));
        printf("%d\n", send);

        sleep(3);
        char* message_r = malloc(100);
        msg_receive(m, message_r, 100);
        printf("%s\n", message_r);
    }
    else {
        sleep(2);
        MESSAGE* m = msg_connect("test", O_RDWR, 10, 100);
        if (m == NULL) {
            exit(1);
        }
        char* message_r = malloc(100);
        int *a = malloc(1000);
        reception(m, a, 1000);
        printf("%d\n", *a);

        char* texte = "test NON";
        int send = msg_send(m, texte, strlen(texte));
        //printf("%d\n", send);

        int status;
        wait(&status);
    }
    return 0;
}