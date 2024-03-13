#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define N 10 // Number of threads to create
#define M 3  // Maximum number of threads in the critical section

// Global semaphore for controlling access to the critical section
sem_t semaphore;

// Mutex for protecting shared data
pthread_mutex_t mutex;

// Shared variable to track the number of threads currently in the critical section
int in_critical_section = 0;

// Prototype for the thread's work function
void* doWork(void* arg);

// Enter and leave functions for managing access to the critical section
void enter(sem_t* sem);
void leave(sem_t* sem);

// The critical work function that needs controlled access
void doCriticalWork(int thread_id);

int main() {
    pthread_t threads[N];

    // Initialize semaphore with M permits
    if (sem_init(&semaphore, 0, M) != 0) {
        perror("Failed to initialize semaphore");
        exit(EXIT_FAILURE);
    }

    // Initialize mutex
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        perror("Failed to initialize mutex");
        exit(EXIT_FAILURE);
    }

    // Create N threads
    for (int i = 0; i < N; ++i) {
        int* thread_id = malloc(sizeof(int));
        *thread_id = i;
        if (pthread_create(&threads[i], NULL, doWork, (void*)thread_id) != 0) {
            perror("Failed to create thread");
            exit(EXIT_FAILURE);
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < N; ++i) {
        pthread_join(threads[i], NULL);
    }

    // Cleanup
    sem_destroy(&semaphore);
    pthread_mutex_destroy(&mutex);

    return 0;
}

void* doWork(void* arg) {
    int thread_id = *((int*)arg);
    free(arg); // Free the allocated memory for the thread ID

    while (1) {
        enter(&semaphore);
        doCriticalWork(thread_id);
        leave(&semaphore);

        // Simulate doing more work
        sleep(1);
    }

    return NULL;
}

void enter(sem_t* sem) {
    sem_wait(sem); // Block if no permits are available
}

void leave(sem_t* sem) {
    sem_post(sem); // Release a permit
}

void doCriticalWork(int thread_id) {
    pthread_mutex_lock(&mutex);
    in_critical_section++;
    printf("Thread %d entering critical section, count: %d\n", thread_id, in_critical_section);
    pthread_mutex_unlock(&mutex);

    // Simulate critical work
    sleep(2);

    pthread_mutex_lock(&mutex);
    in_critical_section--;
    printf("Thread %d leaving critical section, count: %d\n", thread_id, in_critical_section);
    pthread_mutex_unlock(&mutex);
}
