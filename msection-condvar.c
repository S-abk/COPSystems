#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define N 10
int M = 3; // Maximum threads in critical section
int current_in_critical_section = 0; // Tracks the number of threads in the critical section

// Mutex and condition variable for synchronization
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

// Thread function prototype
void* doWork(void* arg);
void enter();
void leave();
void doCriticalWork(int thread_id);

int main() {
    pthread_t threads[N];

    // Create N threads
    for (int i = 0; i < N; ++i) {
        int* id = malloc(sizeof(int));
        *id = i;
        if (pthread_create(&threads[i], NULL, doWork, id) != 0) {
            perror("Failed to create thread");
            return 1;
        }
    }

    // Join N threads
    for (int i = 0; i < N; ++i) {
        pthread_join(threads[i], NULL);
    }

    // Clean up
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}

void* doWork(void* arg) {
    int thread_id = *(int*)arg;
    free(arg); // Free the dynamically allocated memory

    while (1) {
        enter();
        doCriticalWork(thread_id);
        leave();

        sleep(1); // Simulate doing more work
    }

    return NULL;
}

void enter() {
    pthread_mutex_lock(&mutex);
    while (current_in_critical_section >= M) {
        pthread_cond_wait(&cond, &mutex);
    }
    current_in_critical_section++;
    pthread_mutex_unlock(&mutex);
}

void leave() {
    pthread_mutex_lock(&mutex);
    current_in_critical_section--;
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);
}

void doCriticalWork(int thread_id) {
    printf("Thread %d in critical section, count: %d\n", thread_id, current_in_critical_section);
    // Simulate critical work
    sleep(2);
}
