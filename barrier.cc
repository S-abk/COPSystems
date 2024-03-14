#include <pthread.h>
#include <cstdio>

class Barrier {
private:
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int count;
    int numThreads;

public:
    Barrier(int n) : count(0), numThreads(n) {
        pthread_mutex_init(&mutex, NULL);
        pthread_cond_init(&cond, NULL);
    }

    ~Barrier() {
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&cond);
    }

    void wait() {
        pthread_mutex_lock(&mutex);

        // Increment the count of waiting threads
        count++;

        // If the count reaches the number of threads, signal all threads to continue
        if(count >= numThreads) {
            count = 0; // Reset the count for the next use of the barrier
            pthread_cond_broadcast(&cond);
        } else {
            // Wait until the condition is signaled (i.e., the count reaches numThreads)
            while(count < numThreads) {
                pthread_cond_wait(&cond, &mutex);
            }
        }

        pthread_mutex_unlock(&mutex);
    }
};

// Thread function that demonstrates the use of the Barrier
void* thread_fun(void* param) {
    Barrier* barrier = (Barrier*)param;
    
    // Simulate work before waiting at the barrier
    printf("Thread %lu is doing some work before the barrier.\n", pthread_self());
    fflush(stdout); // Flush the output buffer
    
    barrier->wait();

    // Simulate work after waiting at the barrier
    printf("Thread %lu has passed the barrier and is doing more work.\n", pthread_self());
    fflush(stdout); // Flush the output buffer

    pthread_exit(NULL);
}

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number of threads>\n", argv[0]);
        return 1;
    }

    int numThreads = atoi(argv[1]);
    if (numThreads <= 0) {
        fprintf(stderr, "Number of threads must be a positive integer.\n");
        return 1;
    }

    Barrier barrier(numThreads);
    pthread_t threads[numThreads];

    // Create threads
    for(int i = 0; i < numThreads; ++i) {
        if (pthread_create(&threads[i], NULL, thread_fun, (void*)&barrier)) {
            fprintf(stderr, "Failed to create thread.\n");
            return 1;
        }
    }

    // Join threads
    for(int i = 0; i < numThreads; ++i) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
