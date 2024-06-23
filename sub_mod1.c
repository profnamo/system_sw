// sub_module1.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define QUEUE_KEY_1 1234

typedef struct {
    long msg_type;
    char registration_number[20];
} message_t;

int main() {
    key_t key = QUEUE_KEY_1;
    int msgid = msgget(key, IPC_CREAT | 0666);

    if (msgid == -1) {
        perror("msgget failed");
        exit(1);
    }

    message_t msg;
    msg.msg_type = 1;
    printf("Enter vehicle registration number: ");
    scanf("%s", msg.registration_number);

    if (msgsnd(msgid, &msg, sizeof(msg.registration_number), 0) == -1) {
        perror("msgsnd failed");
        exit(1);
    }

    printf("Sent registration number to queue: %s\n", msg.registration_number);

    return 0;
}
