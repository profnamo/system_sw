// module3.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>

#define MAX_TEXT 100

typedef struct {
    long msg_type;
    char text[MAX_TEXT];
} message_t;

void receiveMessage(int msgid, message_t* message, long msg_type) {
    msgrcv(msgid, message, sizeof(message->text), msg_type, 0);
}

int main() {
    key_t key3 = ftok("module3", 85);
    int msgid3 = msgget(key3, 0666 | IPC_CREAT);

    FILE *file = fopen("vehicle_log.csv", "a");
    if (!file) {
        perror("Unable to open file");
        exit(EXIT_FAILURE);
    }

    fprintf(file, "Type,Purpose,Registration Number,Time\n");

    while (1) {
        message_t message;
        receiveMessage(msgid3, &message, 3); // Receive from module 1

        fprintf(file, "%s\n", message.text);
		printf("%s\n", message.text);
        fflush(file); // Ensure data is written to file
    }

    fclose(file);
    msgctl(msgid3, IPC_RMID, NULL);

    return 0;
}
