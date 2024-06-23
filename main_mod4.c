// module4.c
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

void sendMessage(int msgid, const char* text, long msg_type) {
    message_t message;
    message.msg_type = msg_type;
    strncpy(message.text, text, MAX_TEXT);
    msgsnd(msgid, &message, sizeof(message.text), 0);
}

int main() {
    key_t key4 = ftok("module4", 95);
    int msgid4 = msgget(key4, 0666 | IPC_CREAT);

    char registrationNumber[20];

    while(1)
    {	  
		printf("Enter vehicle registration number for exit: ");
		scanf("%s", registrationNumber);

		FILE *file = fopen("vehicle_log.csv", "r");
		if (!file) {
			perror("Unable to open file");
			exit(EXIT_FAILURE);
		}

		char line[256];
		int found = 0;
		while (fgets(line, sizeof(line), file)) {
			if (strstr(line, registrationNumber)) {
				sendMessage(msgid4, line, 4); // Send to module 5
				found = 1;
				break;
			}
		}

		if (!found) {
			printf("Vehicle not found.\n");
		}
		fclose(file);
    }
      msgctl(msgid4, IPC_RMID, NULL);

    return 0;
}
