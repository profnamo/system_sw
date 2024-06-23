#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MAX 100
#define TIME_MAX 30
#define REQUEST_TYPE 1
#define RESPONSE_TYPE 2
#define MODULE3_TYPE 3

// Enumeration for the types of vehicles
typedef enum {
    SEDAN,
    VAN,
    TRUCK,
    SPECIAL,
    EMERGENCY,
    UNKNOWN_TYPE
} VehicleType;

// Enumeration for the purposes of vehicles
typedef enum {
    NON_COMMERCIAL,
    TRANSPORTATION,
    COURIER,
    RENTAL,
    UNKNOWN_PURPOSE
} VehiclePurpose;

// Structure to hold vehicle information
typedef struct {
    VehicleType type;           // Vehicle type
    VehiclePurpose purpose;     // Vehicle purpose
    int registrationNumber;     // Vehicle registration number
    char arrive_time[TIME_MAX];
} VehicleInfo;

// Structure for message queue
struct msg_buffer {
    long msg_type;
    char msg_text[MAX];
};
// Function to get vehicle type from the first three digits
VehicleType getVehicleType(const char* typeStr) {
    int typeNum = atoi(typeStr);
    if (typeNum >= 100 && typeNum <= 699) return SEDAN;
    if (typeNum >= 700 && typeNum <= 799) return VAN;
    if (typeNum >= 800 && typeNum <= 979) return TRUCK;
    if (typeNum >= 980 && typeNum <= 997) return SPECIAL;
    if (typeNum >= 998 && typeNum <= 999) return EMERGENCY;
    return UNKNOWN_TYPE;
}

// Function to get vehicle purpose from the character 
VehiclePurpose getVehiclePurpose(const char* purposeChar) 
{ 
  if (strcmp(purposeChar, "가") == 0 || strcmp(purposeChar, "나") == 0 || strcmp(purposeChar, "다") == 0 || strcmp(purposeChar, "라") == 0 || strcmp(purposeChar, "마") == 0 ||  strcmp(purposeChar, "차") == 0 || strcmp(purposeChar, "카") == 0 || strcmp(purposeChar, "타") == 0 || strcmp(purposeChar, "파") == 0 ) 
   { 
    return NON_COMMERCIAL; 
   } 
   if (strcmp(purposeChar, "아") == 0 || strcmp(purposeChar, "바") == 0 || strcmp(purposeChar, "사") == 0 || strcmp(purposeChar, "자") == 0) 
   { 
     return TRANSPORTATION; 
   } 
   if (strcmp(purposeChar, "배") == 0) 
   { 
     return COURIER; 
   } 
   if (strcmp(purposeChar, "허") == 0 || strcmp(purposeChar, "하") == 0 || strcmp(purposeChar, "호") == 0) 
   { 
     return RENTAL; 
   } 
    return UNKNOWN_PURPOSE; 
}

// Function to parse the license plate
void parseLicensePlate(const char* plate, VehicleInfo* info) {
    char typeStr[4];
    strncpy(typeStr, plate, 3);
    typeStr[3] = '\0';
    info->type = getVehicleType(typeStr);

    char purposeChar[4];
    strncpy(purposeChar, &plate[3], 3);
    purposeChar[4] = '\0';
    info->purpose = getVehiclePurpose(purposeChar);

    info->registrationNumber = atoi(&plate[6]);
}

// Function to display the vehicle information
void displayVehicleInfo(const VehicleInfo* info) {
    printf("Vehicle Type: ");
    switch (info->type) {
        case SEDAN: printf("Sedan\n"); break;
        case VAN: printf("Van\n"); break;
        case TRUCK: printf("Truck\n"); break;
        case SPECIAL: printf("Special\n"); break;
        case EMERGENCY: printf("Emergency\n"); break;
        default: printf("Unknown\n"); break;
    }

    printf("Vehicle Purpose: ");
    switch (info->purpose) {
        case NON_COMMERCIAL: printf("Non-commercial\n"); break;
        case TRANSPORTATION: printf("Transportation\n"); break;
        case COURIER: printf("Courier\n"); break;
        case RENTAL: printf("Rental\n"); break;
        default: printf("Unknown\n"); break;
    }

    printf("Vehicle Registration Number: %d\n", info->registrationNumber);
}

int main() {
    key_t key;
    key_t key3;	
    int msgid;
    int msgid3;	
    struct msg_buffer message;
    char licensePlate[11];  
    VehicleInfo myCar;

		
    // Generate a unique key
    key = ftok("progfile", 65);
    if (key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }
    key3 = ftok("module3", 85);
    if (key3 == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }	

    // Create a message queue and return the identifier
    msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }
    msgid3 = msgget(key3, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }
    // Set message type
    message.msg_type = REQUEST_TYPE;

    while(1)
    {
	 
    printf("Enter the license plate: ");
    scanf("%10s", licensePlate);

    parseLicensePlate(licensePlate, &myCar);
    displayVehicleInfo(&myCar);
	 
    // Send the message
    message.msg_type = REQUEST_TYPE;
    snprintf(message.msg_text, MAX, "time");
          
    if (msgsnd(msgid, &message, sizeof(message.msg_text), 0) == -1) {
          perror("msgsnd");
          exit(EXIT_FAILURE);
      }
    printf("Request sent: %s\n", message.msg_text);

    // Receive the response
    if (msgrcv(msgid, &message, sizeof(message.msg_text), RESPONSE_TYPE, 0) == -1) {
        perror("msgrcv");
        exit(EXIT_FAILURE);
    }
    strncpy(myCar.arrive_time,message.msg_text,TIME_MAX);
    printf("Current time received: %s\n", message.msg_text);

    // Send the message to Module 3
    message.msg_type = MODULE3_TYPE;
    char msg_buf[MAX];
    sprintf(msg_buf,"%d,%d,%d",myCar.type,myCar.purpose,myCar.registrationNumber);
    strcat(msg_buf,",");
    strcat(msg_buf,message.msg_text);
    strncpy(message.msg_text,msg_buf,MAX);

    if (msgsnd(msgid3, &message, sizeof(message.msg_text), 0) == -1) {
          perror("msgsnd");
          exit(EXIT_FAILURE);
      }

    }

    return 0;
}

