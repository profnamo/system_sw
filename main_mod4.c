// module4.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_TEXT 100
#define PORT 8080

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
///// TCP server code
	int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    
	// Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Attach socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the network address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

   // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d\n", PORT);
	
    while(1)
    {	  
		printf("Enter vehicle registration number for exit: ");
		scanf("%s", registrationNumber);

		// Accept an incoming connection
		if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
			perror("accept");
			close(server_fd);
			exit(EXIT_FAILURE);
		}

		printf("Connection accepted\n");

		// Read data from the client
		int valread = read(new_socket, buffer, BUFFER_SIZE);
		if (valread < 0) {
			perror("read");
			close(new_socket);
			close(server_fd);
			exit(EXIT_FAILURE);
		}
		printf("Received message: %s\n", buffer);

		// Simulate processing and send a response
		char *response = "Message received by server";
		if (send(new_socket, response, strlen(response), 0) < 0) {
			perror("send");
			close(new_socket);
			close(server_fd);
			exit(EXIT_FAILURE);
		}
		printf("Response sent\n");

		close(new_socket);

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
    close(server_fd);
	
    return 0;
}
