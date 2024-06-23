// sub_module2.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define QUEUE_KEY_1 1234
#define QUEUE_KEY_2 5678

typedef struct {
    long msg_type;
    char registration_number[20];
} message_t;

typedef struct {
    long msg_type;
    char vehicle_info[100];
} vehicle_info_message_t;

int main() {
    key_t key1 = QUEUE_KEY_1;
    key_t key2 = QUEUE_KEY_2;
    int msgid1 = msgget(key1, IPC_CREAT | 0666);
    int msgid2 = msgget(key2, IPC_CREAT | 0666);

    if (msgid1 == -1 || msgid2 == -1) {
        perror("msgget failed");
        exit(1);
    }

    message_t msg;
    vehicle_info_message_t vmsg;

    if (msgrcv(msgid1, &msg, sizeof(msg.registration_number), 1, 0) == -1) {
        perror("msgrcv failed");
        exit(1);
    }

    printf("Received registration number from queue: %s\n", msg.registration_number);

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

    return 0;
}
