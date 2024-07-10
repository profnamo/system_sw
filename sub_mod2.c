// sub_module2.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define QUEUE_KEY_1 1234
#define QUEUE_KEY_2 5678

#define PORT 8080
#define BUFFER_SIZE 1024
#define IPADDR "127.0.0.1"

typedef struct {
    long msg_type;
    char registration_number[20];
} message_t;

typedef struct {
    long msg_type;
    char vehicle_info[100];
} vehicle_info_message_t;

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char *message = "Hello from client";
    char buffer[BUFFER_SIZE] = {0};
    message_t msg;
    vehicle_info_message_t vmsg;
	
    key_t key1 = QUEUE_KEY_1;
    key_t key2 = QUEUE_KEY_2;
    int msgid1 = msgget(key1, IPC_CREAT | 0666);
    int msgid2 = msgget(key2, IPC_CREAT | 0666);

    if (msgid1 == -1 || msgid2 == -1) {
        perror("msgget failed");
        exit(1);
    }

    // Create socket file descriptor
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_addr.s_addr = inet_addr(IPADDR);
	serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

	if(connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr))==-1) {
		printf("Can not connect\n");
		close(serv_addr);
		return -1;
	}

	while(1)
	{
		if (msgrcv(msgid1, &msg, sizeof(msg.registration_number), 1, 0) == -1) {
			perror("msgrcv failed");
			exit(1);
		}

		printf("Received registration number from queue: %s\n", msg.registration_number);

		// Send message to the server
		send(sock, message, strlen(message), 0);
		printf("Message sent\n");

		// Read response from the server
		int valread = read(sock, buffer, BUFFER_SIZE);
		printf("Received response: %s\n", buffer);

	}
	// Close the socket
	close(sock);	
#if 0 // 차량 정보를 데이터베이스 파일에서 찾지 않고 Main PC로 전달
    // Simulate searching CSV file
    FILE *file = fopen("vehicle_log.csv", "r");
    if (!file) {
        perror("fopen failed");
        exit(1);
    }

    char line[256];
    int found = 0;
    while (fgets(line, sizeof(line), file)) {
        char type[10], purpose[10], reg_num[20], entry_time[50];
        sscanf(line, "%[^,],%[^,],%[^,],%[^\n]", type, purpose, reg_num, entry_time);
        if (strcmp(reg_num, msg.registration_number) == 0) {
            found = 1;
            snprintf(vmsg.vehicle_info, sizeof(vmsg.vehicle_info), "%s,%s,%s,%s", type, purpose, reg_num, entry_time);
            break;
        }
    }
    fclose(file);

    if (found) {
        vmsg.msg_type = 1;
        if (msgsnd(msgid2, &vmsg, sizeof(vmsg.vehicle_info), 0) == -1) {
            perror("msgsnd failed");
            exit(1);
        }
        printf("Sent vehicle info to queue: %s\n", vmsg.vehicle_info);
    } else {
        printf("Vehicle not found.\n");
    }
#endif
    return 0;
}
