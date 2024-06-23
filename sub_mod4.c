// sub_module4.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define QUEUE_KEY_3 9101
#define QUEUE_KEY_4 1122

typedef struct {
    long msg_type;
    char payment_info[150];
} payment_info_message_t;

int main() {
    key_t key3 = QUEUE_KEY_3;
    key_t key4 = QUEUE_KEY_4;
    int msgid3 = msgget(key3, IPC_CREAT | 0666);
    int msgid4 = msgget(key4, IPC_CREAT | 0666);

    if (msgid3 == -1 || msgid4 == -1) {
        perror("msgget failed");
        exit(1);
    }

    payment_info_message_t pmsg;

    if (msgrcv(msgid3, &pmsg, sizeof(pmsg.payment_info), 1, 0) == -1) {
        perror("msgrcv failed");
        exit(1);
    }

    printf("Received payment info from queue: %s\n", pmsg.payment_info);

    // Simulate payment process
    printf("Processing payment...\n");

    // Send payment confirmation to Main PC module 4
    if (msgsnd(msgid4, &pmsg, sizeof(pmsg.payment_info), 0) == -1) {
        perror("msgsnd failed");
        exit(1);
    }

    printf("Sent payment confirmation to queue\n");

    return 0;
}
