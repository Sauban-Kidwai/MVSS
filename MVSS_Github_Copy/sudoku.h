#ifndef SUDOKU_H
#define SUDOKU_H

#include <pthread.h>

#define GRID_SIZE 9

// Shared variables for the Sudoku grid and validation
extern int Sol[GRID_SIZE][GRID_SIZE];
extern int Row[GRID_SIZE], Col[GRID_SIZE], Sub[GRID_SIZE];
extern int Counter;
extern int threads_completed;
extern pthread_t last_thread_id;
extern pthread_mutex_t mutexCounter;
extern pthread_cond_t allDone;

// Structure to hold the result of a thread's validation
typedef struct {
    int valid;                   // Indicates if the validation was successful
    pthread_t thread_id;         // ID of the thread
    char details[256];           // Details of the validation result
} ThreadResult;

// Function declarations for validating rows, columns, and subgrids
int check_row(int row);
int check_column(int col);
int check_subgrid(int startRow, int startCol);
void* validate_rows_and_subgrids(void* arg);
void* validate_columns(void* arg);

#endif
