// module5.c
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

typedef enum {
    SEDAN,
    VAN,
    TRUCK,
    SPECIAL,
    EMERGENCY
} VehicleType;

typedef enum {
    NON_COMMERCIAL,
    TRANSPORTATION,
    COURIER,
    RENTAL,
    UNKNOWN_PURPOSE
} VehiclePurpose;

typedef struct {
    VehicleType type;
    VehiclePurpose purpose;
    char registrationNumber[20];
    char entryTime[30];
} VehicleInfo;

VehicleType getVehicleType(int type) {
    switch (type) {
        case 0: return SEDAN;
        case 1: return VAN;
        case 2: return TRUCK;
        case 3: return SPECIAL;
        case 4: return EMERGENCY;
        default: return SEDAN;
    }
}

double getRate(VehicleType type) {
    switch (type) {
        case SEDAN: return 1.0;
        case VAN: return 1.2;
        case TRUCK: return 1.4;
        case SPECIAL: return 2.0;
        case EMERGENCY: return 1.0;
        default: return 1.0;
    }
}

int getMaxCharge(VehicleType type) {
    switch (type) {
        case SEDAN: return 10000;
        case VAN: return 12000;
        case TRUCK: return 14000;
        case SPECIAL: return 20000;
        case EMERGENCY: return 10000;
        default: return 10000;
    }
}

void receiveMessage(int msgid, message_t* message, long msg_type) {
    msgrcv(msgid, message, sizeof(message->text), msg_type, 0);
}

int calculateParkingFee(const char* entryTime, VehicleType type) {
    struct tm entry_tm = {0};
    strptime(entryTime, "%a %b %d %H:%M:%S %Y", &entry_tm);
    time_t entryTime_t = mktime(&entry_tm);
    time_t currentTime = time(NULL);
    double hoursParked = difftime(currentTime, entryTime_t) / 3600.0;

    double rate = getRate(type);
    int charge = (int)(hoursParked * rate * 1000);
    int maxCharge = getMaxCharge(type);

    int days = (int)(hoursParked / 24);
    int totalCharge = charge > maxCharge ? (days + 1) * maxCharge : charge;
    return totalCharge;
}

int main() {
    key_t key4 = ftok("module4", 95);
    int msgid4 = msgget(key4, 0666 | IPC_CREAT);

    message_t message;
	while(1)
	{
		receiveMessage(msgid4, &message, 4); // Receive from module 4

		VehicleInfo vehicle;
		sscanf(message.text, "%d %d %s %49[^\n]", (int*)&vehicle.type, (int*)&vehicle.purpose, vehicle.registrationNumber, vehicle.entryTime);
		int fee = calculateParkingFee(vehicle.entryTime, vehicle.type);
		printf("Parking fee for vehicle %s: %d\n", vehicle.registrationNumber, fee);
	}
    msgctl(msgid4, IPC_RMID, NULL);

    return 0;
}
