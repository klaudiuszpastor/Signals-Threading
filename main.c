#include "signals.h"
#include "read_input.h"
#include "write_output.h"

// Definicje zmiennych globalnych
pthread_mutex_t mutex;
pthread_cond_t cond;
SharedMemory *sharedMemory;
int dataReady = 0;
int inputComplete = 0;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input file> <output file>\n", argv[0]);
        return EXIT_FAILURE;
    }
    pthread_t inputThread, outputThread;

    // Alokacja pamięci dla sharedMemory, która obejmuje wszystkie tablice
    sharedMemory = (SharedMemory *)malloc(sizeof(SharedMemory));
    if (!sharedMemory) {
        perror("Failed to allocate shared memory");
        return EXIT_FAILURE;
    }

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    // Tworzenie wątków z przekazaniem plików jako argumentów
    pthread_create(&inputThread, NULL, read_input, argv[1]);
    pthread_create(&outputThread, NULL, write_output, argv[2]);

    // Oczekiwanie na zakończenie wątków
    pthread_join(inputThread, NULL);
    pthread_join(outputThread, NULL);

    // Zwolnienie zasobów
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    free(sharedMemory);

    return EXIT_SUCCESS;
}
