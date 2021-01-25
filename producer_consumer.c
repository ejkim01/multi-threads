#include <string.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <poll.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <inttypes.h>

pthread_t tid[8];
#define BUFFER_SIZE 10
int buffer[BUFFER_SIZE];
int fill_ptr = 0;
int use_ptr = 0;
int count = 0;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t fill = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
sem_t semEmpty;
sem_t semFull;

void put(int value) {
    buffer[fill_ptr] = value;
    fill_ptr = (fill_ptr+1)%BUFFER_SIZE;
    count++;
}

int get() {
    int tmp = buffer[use_ptr];
    use_ptr = (use_ptr+1)%BUFFER_SIZE;
    count--;
    return tmp;
}

void* producer_conditoinal(void* args) {
    while (1) {
        sleep(1);
        pthread_mutex_lock(&mtx);
        while (count==BUFFER_SIZE)
            pthread_cond_wait(&empty, &mtx);
        put(rand()%100);
        pthread_cond_signal(&fill);
        pthread_mutex_unlock(&mtx);
        printf("P: %u\n", pthread_self());
    }
}

void* consumer_conditional(void* args) {
    while (1) {
        pthread_mutex_lock(&mtx);
        while (count==0)
            pthread_cond_wait(&fill, &mtx);
        int tmp = get();
        pthread_cond_signal(&empty);
        pthread_mutex_unlock(&mtx);
        printf("C: %u\n", pthread_self());
    }
}

void* producer_semaphore(void* args) {
    while (1) {
        //usleep(100000);
        int val = rand()%100;
        sem_wait(&semEmpty);
        pthread_mutex_lock(&mtx);
        //put(val);
        buffer[fill_ptr] = val;
        fill_ptr = (fill_ptr+1)%BUFFER_SIZE;
        printf("P: val %d, thread %u\n", val, pthread_self());
        pthread_mutex_unlock(&mtx);
        sem_post(&semFull);
    }
}

void* consumer_semaphore(void* args) {
    while (1) {
        //usleep(500000);
        usleep(1000);
        int tmp;
        sem_wait(&semFull);
        pthread_mutex_lock(&mtx);
        //int tmp = get();
        tmp = buffer[use_ptr];
        use_ptr = (use_ptr+1)%BUFFER_SIZE;
        pthread_mutex_unlock(&mtx);
        sem_post(&semEmpty);
        printf("C: get tmp %d, thread %u\n", tmp, pthread_self());
    }
}

int main() {
    int err;

    sem_init(&semEmpty, 0, 10);
    sem_init(&semFull, 0, 0);
    
    for (int i=0; i<8; i++) {
        if (i%2==0)
            pthread_create(&(tid[0]), NULL, &producer_semaphore, NULL);
        else
            pthread_create(&(tid[0]), NULL, &consumer_semaphore, NULL);
    }

    for (int i=0; i<8; i++)
        pthread_join(tid[i], NULL);
    //err = pthread_create(&(tid[0]), NULL, &producer, NULL);
    //err = pthread_create(&(tid[1]), NULL, &consumer, NULL);

    //pthread_join(tid[0], NULL);
    //pthread_join(tid[1], NULL);

    return 0;
}

