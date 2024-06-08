#include "read_input.h"

void *read_input(void *filename) {
    FILE *file = fopen((char *)filename, "r");
    if (!file) {
        perror("Failed to open input file");
        pthread_exit(NULL);
    }

    char line[256];
    int line_number = 0; // Dodany licznik linii
    while (fgets(line, sizeof(line), file)) {
        line_number++;
        // Sprawdzenie, czy linia kończy się znakiem '\n'
        size_t len = strlen(line);
        if (line[len - 1] != '\n') {
            fprintf(stderr, "Line %d exceeds buffer length\n", line_number); //Informacja o numerze linii
            // Opróżnienie reszty linii
            int ch;
            while ((ch = fgetc(file)) != '\n' && ch != EOF);
            continue;
        }
         
        int values[128];
        int count = read_line(line, values, 128);
        // Sprawdzenie czy ilośc wartość w linii jest odpowiednia
        if (count < 4) {
            fprintf(stderr, "Invalid number of values in line %d\n", line_number);
            continue; // Ignorowanie niepoprawnych linii
        }

        int signalType = values[0];
        int signalSize = values[1];
        //Sprawdzenie czy signalSize jest 12-bitowy (0,4096)
        if (signalSize < 0 || signalSize > 4095) {
            fprintf(stderr, "Invalid signalSize in line %d\n", line_number);
            continue; // Ignorowanie niepoprawnych linii
        }

        int valid = 0;
        //header.signalType - czy signalType jest równe 1 czy 4
        if (signalType == 1) {
            // ConnectionRequest - configPreset w zakresie (0,10), signalStrength 10 bitów (0-1023)
            valid = validate_connection_request(values,count);
        } else if (signalType == 4) {
            // DownlinkData - noOfConnections (0,15) 
            valid = validate_downlink_data(values,count);
        } else {
            fprintf(stderr, "Unknown signal type in line %d\n", line_number); // Dodana linia: Informacja o nieznanym typie sygnału
            continue; // Ignorowanie innych typów sygnałów
        }

        if (!valid) {
            fprintf(stderr, "Invalid values in line %d\n", line_number);
            continue;
        }

        // Blokowanie mutexa i wpisywanie danych do pamięci współdzielonej po sprawdzeniu poprawności
        pthread_mutex_lock(&mutex); //Zablokowanie mutexa
        
        if (signalType == 1) {
            write_connection_request_to_memory(values);
        } else if (signalType == 4) {
            write_downlink_data_to_memory(values);
        }

        dataReady = 1;
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);

        pthread_mutex_lock(&mutex);
        while (dataReady) {
            pthread_cond_wait(&cond, &mutex);
        }
        pthread_mutex_unlock(&mutex);
    }

    pthread_mutex_lock(&mutex);
    inputComplete = 1;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);

    fclose(file);
    pthread_exit(NULL);
}

int read_line(char *line, int *values, int max_values) {
    int count = 0;
    char *token = strtok(line, " ");
    while (token != NULL && count < max_values) {
        if (sscanf(token, "%d", &values[count]) != 1) {
            count = 0;
            break;
        }
        count++;
        token = strtok(NULL, " ");
    }
    return count;
}

int validate_connection_request(int *values, int count) {
    if (count != 5 || values[2] < 0 || values[3] < 0 || values[3] > 10 || values[4] < 0 || values[4] > 1023) {
        return 0;
    }
    return 1;
}

int validate_downlink_data(int *values, int count) {
    if (count < 7 || (count - 3) % 4 != 0 || values[2] < 0 || values[2] > 15) {
        return 0;
    }
    for (int i = 0; i < values[2]; i++) {
        int baseIndex = 3 + i * 4;
        if (values[baseIndex] < 0 || values[baseIndex + 1] < 0 || values[baseIndex + 1] > 31 ||
            values[baseIndex + 2] < 0 || values[baseIndex + 2] > 127 || values[baseIndex + 3] < 0) {
            return 0;
        }
    }
    return 1;
}

void write_connection_request_to_memory(int *values) {
    sharedMemory->isConnectionRequest = 1;
    sharedMemory->signal.connReq.header.signalType = values[0];
    sharedMemory->signal.connReq.header.signalSize = values[1];
    sharedMemory->signal.connReq.connectionId = values[2];
    sharedMemory->signal.connReq.configPreset = values[3];
    sharedMemory->signal.connReq.signalStrength = values[4];
}

void write_downlink_data_to_memory(int *values) {
    sharedMemory->isConnectionRequest = 0;
    sharedMemory->signal.downData.header.signalType = values[0];
    sharedMemory->signal.downData.header.signalSize = values[1];
    sharedMemory->signal.downData.noOfConnections = values[2];
    // Sprawdzenie rozmiarów w connectionDownlinkDataDetails
    for (int i = 0; i < sharedMemory->signal.downData.noOfConnections; i++) {
        int baseIndex = 3 + i * 4;
        sharedMemory->signal.downData.details[i].connectionId = values[baseIndex];
        sharedMemory->signal.downData.details[i].mcs = values[baseIndex + 1];
        sharedMemory->signal.downData.details[i].tbs = values[baseIndex + 2];
        sharedMemory->signal.downData.details[i].dataSize = values[baseIndex + 3];
    }
}