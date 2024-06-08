#include "write_output.h"

void *write_output(void *filename) {
    FILE *file = fopen((char *)filename, "w");
    if (!file) {
        perror("Failed to open output file");
        pthread_exit(NULL);
    }

    while (1) {
        pthread_mutex_lock(&mutex);
        while (!dataReady && !inputComplete) {
            pthread_cond_wait(&cond, &mutex);
        }
        if (inputComplete && !dataReady) {
            pthread_mutex_unlock(&mutex);
            break;
        }

        cJSON *json = cJSON_CreateObject();

        if (sharedMemory->isConnectionRequest) {
            write_connection_request(json);
        } else {
            write_downlink_data(json);
        }

        char *string = cJSON_Print(json);
        fprintf(file, "%s\n\n", string);

        free(string);
        cJSON_Delete(json); //zwalniania jest cała pamięć przydzielona do *json także obiekty podrzędne

        dataReady = 0;
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }

    fclose(file);
    pthread_exit(NULL);
}

void write_connection_request(cJSON *json) {
    cJSON *connectionRequest = cJSON_CreateObject();
    cJSON_AddItemToObject(json, "ConnectionRequest", connectionRequest);

    cJSON *header = cJSON_CreateObject();
    cJSON_AddItemToObject(connectionRequest, "header", header);
    cJSON_AddNumberToObject(header, "signalType", sharedMemory->signal.connReq.header.signalType);
    cJSON_AddNumberToObject(header, "signalSize", sharedMemory->signal.connReq.header.signalSize);

    cJSON_AddNumberToObject(connectionRequest, "connectionId", sharedMemory->signal.connReq.connectionId);
    cJSON_AddNumberToObject(connectionRequest, "configPreset", sharedMemory->signal.connReq.configPreset);
    cJSON_AddNumberToObject(connectionRequest, "signalStrength", sharedMemory->signal.connReq.signalStrength);
}

void write_downlink_data(cJSON *json) {
    cJSON *downlinkData = cJSON_CreateObject();
    cJSON_AddItemToObject(json, "DownlinkData", downlinkData);

    cJSON *header = cJSON_CreateObject();
    cJSON_AddItemToObject(downlinkData, "header", header);
    cJSON_AddNumberToObject(header, "signalType", sharedMemory->signal.downData.header.signalType);
    cJSON_AddNumberToObject(header, "signalSize", sharedMemory->signal.downData.header.signalSize);

    cJSON_AddNumberToObject(downlinkData, "noOfConnections", sharedMemory->signal.downData.noOfConnections);

    cJSON *detailsArray = cJSON_CreateArray();
    cJSON_AddItemToObject(downlinkData, "connectionDownlinkDataDetails", detailsArray);

    for (int i = 0; i < sharedMemory->signal.downData.noOfConnections; i++) {
        cJSON *detail = cJSON_CreateObject();
        cJSON_AddItemToArray(detailsArray, detail);
        cJSON_AddNumberToObject(detail, "connectionId", sharedMemory->signal.downData.details[i].connectionId);
        cJSON_AddNumberToObject(detail, "mcs", sharedMemory->signal.downData.details[i].mcs);
        cJSON_AddNumberToObject(detail, "tbs", sharedMemory->signal.downData.details[i].tbs);
        cJSON_AddNumberToObject(detail, "dataSize", sharedMemory->signal.downData.details[i].dataSize);
    }
}
