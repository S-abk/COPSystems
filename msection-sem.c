#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define N 10 // Number of threads
int M = 3; // Max number of threads in critical section
sem_t semaphore; // Semaphore for synchronization

// Thread-specific data to keep track of threads in critical section
typedef struct {
    int thread_id;
} thread_data_t;

// Function declarations
void* doWork(void* arg);
void enter(sem_t* sem);
void leave(sem_t* sem);
void doCriticalWork(int thread_id);

int main() {
    pthread_t threads[N];
    thread_data_t thread_data[N];

    // Initialize semaphore with M permits
    if (sem_init(&semaphore, 0, M) != 0) {
        perror("sem_init");
        exit(EXIT_FAILURE);
    }

    // Create and start N threads
    for (int i = 0; i < N; ++i) {
        thread_data[i].thread_id = i;
        if (pthread_create(&threads[i], NULL, doWork, (void*)&thread_data[i]) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    // Join threads
    for (int i = 0; i < N; ++i) {
        pthread_join(threads[i], NULL);
    }

    // Cleanup semaphore
    sem_destroy(&semaphore);

    return 0;
}

void* doWork(void* arg) {
    thread_data_t* data = (thread_data_t*)arg;

    while (1) {
        enter(&semaphore);
        doCriticalWork(data->thread_id);
        leave(&semaphore);
        // Simulate more work with sleep
        sleep(1);
    }

    return NULL;
}

void enter(sem_t* sem) {
    sem_wait(sem); // Wait to enter the critical section
}

void leave(sem_t* sem) {
    sem_post(sem); // Leave the critical section, allowing another thread to enter
}

void doCriticalWork(int thread_id) {
    static int in_critical_section = 0; // Tracks number of threads in critical section

    // Entering critical section
    in_critical_section++;
    printf("Thread %d in critical section, count: %d\n", thread_id, in_critical_section);
    sleep(2); // Simulate work

    // Leaving critical section
    in_critical_section--;
}
