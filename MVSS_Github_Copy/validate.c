#include "sudoku.h"
#include <stddef.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

// Function to check a row of the Sudoku grid
int check_row(int row) {
    int freq[GRID_SIZE + 1] = {0};
    for (int col = 0; col < GRID_SIZE; col++) {
        int num = Sol[row][col];
        if (num < 1 || num > 9 || freq[num] == 1) {
            return 0;  // Row is invalid
        }
        freq[num]++;
    }
    return 1;  // Row is valid
}

// Function to check a column of the Sudoku grid
int check_column(int col) {
    int freq[GRID_SIZE + 1] = {0};
    for (int row = 0; row < GRID_SIZE; row++) {
        int num = Sol[row][col];
        if (num < 1 || num > 9 || freq[num] == 1) {
            return 0;  // Column is invalid
        }
        freq[num]++;
    }
    return 1;  // Column is valid
}

// Function to check a 3x3 subgrid of the Sudoku grid
int check_subgrid(int startRow, int startCol) {
    int freq[GRID_SIZE + 1] = {0};
    for (int row = startRow; row < startRow + 3 && row < GRID_SIZE; row++) {
        for (int col = startCol; col < startCol + 3 && col < GRID_SIZE; col++) {
            int num = Sol[row][col];
            if (num < 1 || num > 9 || freq[num] == 1) {
                return 0;  // Subgrid is invalid
            }
            freq[num]++;
        }
    }
    return 1;  // Subgrid is valid
}

// Thread function to validate rows and subgrids
void* validate_rows_and_subgrids(void* arg) {
    int* range = (int*)arg;
    ThreadResult* result = (ThreadResult*)malloc(sizeof(ThreadResult));
    result->valid = 1;
    result->thread_id = pthread_self();
    result->details[0] = '\0';

    // Print which thread is working
    pthread_t self_id = pthread_self();
    printf("Thread ID: %lu is checking the rows and subgrids.....\n", (unsigned long)self_id);

    // Validate the rows and subgrids
    pthread_mutex_lock(&mutexCounter);
    for (int i = range[0]; i <= range[1] && i < GRID_SIZE; i++) {
        if (!check_row(i)) {
            sprintf(result->details + strlen(result->details), "row %d, ", i + 1);
            result->valid = 0;
        } else {
            Row[i] = 1;
            Counter++;
        }

        int subgridIndex = (i / 3) * 3 + (i % 3);
        if (!check_subgrid((i / 3) * 3, (i % 3) * 3)) {
            sprintf(result->details + strlen(result->details), "sub-grid %d, ", subgridIndex + 1);
            result->valid = 0;
        } else {
            Sub[subgridIndex] = 1;
            Counter++;
        }
    }
    // Update completion status
    threads_completed++;
    last_thread_id = self_id;  // Update last thread ID
    if (threads_completed == 4) {
        printf("\n\nThread ID-%lu is the last thread!\n\n", (unsigned long)self_id);
        pthread_cond_signal(&allDone);
    }
    pthread_mutex_unlock(&mutexCounter);

    return result;
}

// Thread function to validate columns
void* validate_columns(void* arg) {
    int delay = *(int*)arg;
    ThreadResult* result = (ThreadResult*)malloc(sizeof(ThreadResult));
    result->valid = 1;
    result->thread_id = pthread_self();
    result->details[0] = '\0';

    // Print which thread is working
    pthread_t self_id = pthread_self();
    printf("Thread ID: %lu is checking the columns......\n", (unsigned long)self_id);

    // Validate the columns
    pthread_mutex_lock(&mutexCounter);
    for (int col = 0; col < GRID_SIZE; col++) {
        sleep(delay);
        if (!check_column(col)) {
            sprintf(result->details + strlen(result->details), "column %d, ", col + 1);
            result->valid = 0;
            Col[col] = 0;
        } else {
            Col[col] = 1;
            Counter++;
        }
    }
    // Update completion status
    threads_completed++;
    last_thread_id = self_id;
    if (threads_completed == 4) {
        printf("\n\nThread ID-%lu is the last thread!\n\n", (unsigned long)self_id);
        pthread_cond_signal(&allDone);
    }
    pthread_mutex_unlock(&mutexCounter);

    return result;
}