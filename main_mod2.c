#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>

#define MAX 100
#define REQUEST_TYPE 1
#define RESPONSE_TYPE 2


// Structure for message queue
struct msg_buffer {
    long msg_type;
    char msg_text[MAX];
};

int main() {
    key_t key;
    int msgid;
    struct msg_buffer message;

    // Generate a unique key
    key = ftok("progfile", 65);
    if (key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    // Get the message queue identifier
    msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    // Receive the message
    while(1)
    {
	    if (msgrcv(msgid, &message, sizeof(message.msg_text), 1, 0) == -1) {
		perror("msgrcv");
		exit(EXIT_FAILURE);
	    }

	    printf("Message received: %s\n", message.msg_text);

	    // Check if the received message is "time"
	    if (strcmp(message.msg_text, "time") == 0) {
		// Get the current time
		time_t t;
		time(&t);
		snprintf(message.msg_text, MAX, "%s", ctime(&t));

		// Set message type to response
		message.msg_type = RESPONSE_TYPE;

		// Send the current time back
		if (msgsnd(msgid, &message, sizeof(message.msg_text), 0) == -1) {
		    perror("msgsnd");
		    exit(EXIT_FAILURE);
		}

		printf("Response sent: %s\n", message.msg_text);
	    }	    
    }

    // To destroy the message queue
    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        exit(EXIT_FAILURE);
    }

    return 0;
}

