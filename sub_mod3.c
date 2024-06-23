// sub_module3.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>

#define QUEUE_KEY_2 5678
#define QUEUE_KEY_3 9101

typedef struct {
    long msg_type;
    char vehicle_info[100];
} vehicle_info_message_t;

typedef struct {
    long msg_type;
    char current_time[50];
} time_message_t;

typedef struct {
    long msg_type;
    char payment_info[150];
} payment_info_message_t;

void calculate_parking_fee(const char *entry_time_str, const char *exit_time_str, double rate, double max_fee, double *fee) {
    struct tm entry_time = {0};
    struct tm exit_time = {0};
    strptime(entry_time_str, "%a %b %d %H:%M:%S %Y", &entry_time);
    strptime(exit_time_str, "%a %b %d %H:%M:%S %Y", &exit_time);

    time_t entry = mktime(&entry_time);
    time_t exit = mktime(&exit_time);
    double seconds = difftime(exit, entry);
    double hours = seconds / 3600.0;

    *fee = hours * rate;
    if (*fee > max_fee) {
        *fee = max_fee;
    }
}

int main() {
    key_t key2 = QUEUE_KEY_2;
    key_t key3 = QUEUE_KEY_3;
    int msgid2 = msgget(key2, IPC_CREAT | 0666);
    int msgid3 = msgget(key3, IPC_CREAT | 0666);

    if (msgid2 == -1 || msgid3 == -1) {
        perror("msgget failed");
        exit(1);
    }

    vehicle_info_message_t vmsg;
    time_message_t tmsg;
    payment_info_message_t pmsg;

    if (msgrcv(msgid2, &vmsg, sizeof(vmsg.vehicle_info), 1, 0) == -1) {
        perror("msgrcv failed");
        exit(1);
    }

    printf("Received vehicle info from queue: %s\n", vmsg.vehicle_info);

    char type[10], purpose[10], reg_num[20], entry_time[50];
    sscanf(vmsg.vehicle_info, "%[^,],%[^,],%[^,],%[^\n]", type, purpose, reg_num, entry_time);

    // Request current time from Main PC module 2
    // Simulate current time
    time_t now = time(NULL);
    strftime(tmsg.current_time, sizeof(tmsg.current_time), "%a %b %d %H:%M:%S %Y", localtime(&now));
    tmsg.msg_type = 1;
    // Assuming we send request here and get the current time in response

    printf("Current time: %s\n", tmsg.current_time);

    // Calculate parking fee
    double rate, max_fee;
    if (strcmp(type, "0") == 0) {  // 승용차
        rate = 1000;
        max_fee = 10000;
    } else if (strcmp(type, "1") == 0) {  // 승합차
        rate = 1200;
        max_fee = 12000;
    } else if (strcmp(type, "2") == 0) {  // 화물차
        rate = 1400;
        max_fee = 14000;
    } else if (strcmp(type, "3") == 0) {  // 특수차
        rate = 2000;
        max_fee = 20000;
    } else {  // 긴급차
        rate = 1000;
        max_fee = 10000;
    }

    double fee;
    calculate_parking_fee(entry_time, tmsg.current_time, rate, max_fee, &fee);

    snprintf(pmsg.payment_info, sizeof(pmsg.payment_info), "Vehicle: %s, Entry: %s, Exit: %s, Fee: %.2f", reg_num, entry_time, tmsg.current_time, fee);
    pmsg.msg_type = 1;

    if (msgsnd(msgid3, &pmsg, sizeof(pmsg.payment_info), 0) == -1) {
        perror("msgsnd failed");
        exit(1);
    }

    printf("Sent payment info to queue: %s\n", pmsg.payment_info);

    return 0;
}
