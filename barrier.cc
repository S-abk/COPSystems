#include <pthread.h>
#include <iostream>

class Barrier {
public:
    Barrier(int n) : threshold(n), count(0), generation(0) {
        pthread_mutex_init(&mutex, NULL);
        pthread_cond_init(&cond, NULL);
    }

    ~Barrier() {
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&cond);
    }

    void wait() {
        pthread_mutex_lock(&mutex);

        int gen = generation;

        if (++count == threshold) {
            generation++;
            count = 0;
            pthread_cond_broadcast(&cond);
        } else {
            while (gen == generation) {
                pthread_cond_wait(&cond, &mutex);
            }
        }

        pthread_mutex_unlock(&mutex);
    }

private:
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int threshold;
    int count;
    int generation;
};

// global barrier instance
Barrier *barrier;

// thread function
void* thread_fun(void* param) {
    long thread_id = (long)param;
    // simulate work before waiting at the barrier
    std::cout << "Thread " << thread_id << " is doing some work before the barrier\n";
    barrier->wait();
    // simulate work after waiting at the barrier
    std::cout << "Thread " << thread_id << " is doing some work after the barrier\n";
    pthread_exit(NULL);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <number_of_threads>\n";
        return 1;
    }

    int num_threads = atoi(argv[1]);
    if (num_threads <= 0) {
        std::cerr << "Number of threads must be a positive integer\n";
        return 1;
    }

    pthread_t threads[num_threads];
    barrier = new Barrier(num_threads);

    // create threads
    for (long i = 0; i < num_threads; i++) {
        if (pthread_create(&threads[i], NULL, thread_fun, (void*)i) != 0) {
            std::cerr << "Failed to create thread " << i << "\n";
            return 1;
        }
    }

    // join threads
    for (int i = 0; i < num_threads; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            std::cerr << "Failed to join thread " << i << "\n";
            return 1;
        }
    }

    delete barrier;

    return 0;
}
