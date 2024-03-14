#include <pthread.h>
#include <iostream>

class Barrier {
private:
    pthread_mutex_t mutex; // Mutex for locking the critical section
    pthread_cond_t cond;   // Condition variable for blocking/waiting threads
    int count;             // Number of waiting threads
    int numThreads;        // Number of threads that must call wait before releasing

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
        pthread_mutex_lock(&mutex); // Enter critical section

        count++; // Increment the count of waiting threads
        if (count == numThreads) {
            // If N threads are waiting, wake up all threads
            count = 0; // Reset the count for the next use
            pthread_cond_broadcast(&cond);
        } else {
            // Wait on condition variable until the Nth thread arrives
            while (count < numThreads) {
                pthread_cond_wait(&cond, &mutex);
            }
        }

        pthread_mutex_unlock(&mutex); // Leave critical section
    }
};

// Example thread function
void* thread_fun(void* param) {
    Barrier* barrier = static_cast<Barrier*>(param);

    // Do some work here...

    barrier->wait(); // Suspend thread until all have called wait

    // Do more work after the barrier...

    return NULL;
}

// Example usage
int main() {
    const int N = 10; // Number of threads to synchronize
    pthread_t threads[N];
    Barrier barrier(N);

    // Create threads
    for (int i = 0; i < N; ++i) {
        if (pthread_create(&threads[i], NULL, thread_fun, &barrier)) {
            std::cerr << "Failed to create thread " << i << std::endl;
            return 1;
        }
    }

    // Join threads
    for (int i = 0; i < N; ++i) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
