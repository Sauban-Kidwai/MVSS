#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "sudoku.h"

#define GRID_SIZE 9

// Shared resources
int Sol[GRID_SIZE][GRID_SIZE];
int Row[GRID_SIZE], Col[GRID_SIZE], Sub[GRID_SIZE];
int Counter;
int threads_completed = 0;
pthread_t last_thread_id = 0;
pthread_mutex_t mutexCounter;
pthread_cond_t allDone;

int main(int argc, char *argv[]) {
    // Check for correct usage
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <filepath> <delay>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Open the input file
    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Failed to open file");
        return EXIT_FAILURE;
    }

    // Read the 9x9 grid from the input file
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (fscanf(file, "%d", &Sol[i][j]) != 1) {
                fprintf(stderr, "Error: Incorrect input file, expected a 9x9 grid.\n");
                fclose(file);
                return EXIT_FAILURE;
            }
        }
    }
    fclose(file);

    // Initialize synchronization primitives
    pthread_mutex_init(&mutexCounter, NULL);
    pthread_cond_init(&allDone, NULL);

    printf("\nSudoku puzzle Validator:\n\n");

    pthread_t threads[4];

    // Get the delay value from the command line arguments and check it does not exceed 10
    int delay = atoi(argv[2]);
    if (delay > 10 || delay < 1) {
        fprintf(stderr, "Error: Delay value should be between 1 and 10.\n");
        return EXIT_FAILURE;
    }
    int ranges[4][2] = {{0, 2}, {3, 5}, {6, 8}, {0, 8}};
    void** results = (void**)malloc(4 * sizeof(void*));
    
    // Initialize the results array
    for (int i = 0; i < 4; i++) {
        results[i] = NULL;
    }

    // Create 4 threads for validation
    for (int i = 0; i < 4; i++) {
        if (i < 3) {
            pthread_create(&threads[i], NULL, validate_rows_and_subgrids, &ranges[i]);
        } else {
            pthread_create(&threads[i], NULL, validate_columns, &delay);
        }
    }

    // Wait for all threads to complete
    pthread_mutex_lock(&mutexCounter);
    while (threads_completed < 4) {
        pthread_cond_wait(&allDone, &mutexCounter);
    }
    pthread_mutex_unlock(&mutexCounter);

    printf("\n");

    // Join the threads and display the results
    for (int i = 0; i < 4; i++) {
        if (pthread_join(threads[i], &results[i]) != 0) {
            fprintf(stderr, "Error joining thread %d\n", i);
            continue;
        }
        ThreadResult* res = (ThreadResult*)results[i];
        if (res == NULL) {
            fprintf(stderr, "Error: Thread %d did not return a valid result\n", i);
        } else if (res->valid) {
            printf("Thread ID-%d (%lu): valid\n", i + 1, (unsigned long)res->thread_id);
        } else {
            printf("Thread ID-%d (%lu): %sinvalid\n", i + 1, (unsigned long)res->thread_id, res->details);
        }
        free(res);
    }

    // Display the final result
    printf("\nThere are %d valid rows, columns, and sub-grids, and thus the solution is %s.\n\n",
           Counter, Counter == 27 ? "valid" : "invalid");

    // Clean up
    free(results);
    pthread_mutex_destroy(&mutexCounter);
    pthread_cond_destroy(&allDone);

    return EXIT_SUCCESS;
}