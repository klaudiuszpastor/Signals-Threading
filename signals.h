#pragma once
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#define MAX_CONNECTIONS 16

typedef struct {
    uint16_t signalType: 4;  // 4 bity
    uint16_t signalSize: 12; // 12 bitów
} Header;

typedef struct {
    Header header;
    uint32_t connectionId;   // 32 bity
    uint8_t configPreset: 4; // 4 bity (0-10)
    uint16_t signalStrength: 10; // 10 bitów
} ConnectionRequest;

typedef struct {
    uint32_t connectionId;  // 32 bity
    uint8_t mcs: 5;         // 5 bity
    uint8_t tbs: 7;         // 7 bity
    uint32_t dataSize;      // 32 bity
} ConnectionDownlinkDataDetails;

typedef struct {
    Header header;
    uint8_t noOfConnections: 4; // 4 bity
    ConnectionDownlinkDataDetails details[MAX_CONNECTIONS]; // tablica struktur
} DownlinkData;

// Struktura do przekazywania danych między wątkami
typedef struct {
    int isConnectionRequest; // 1 jeśli ConnectionRequest, 0 jeśli DownlinkData
    union {
        ConnectionRequest connReq;
        DownlinkData downData;
    } signal;
} SharedMemory;


// Deklaracja zmiennych globanych
extern pthread_mutex_t mutex;
extern pthread_cond_t cond;
extern SharedMemory *sharedMemory;
extern int dataReady;
extern int inputComplete;