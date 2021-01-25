#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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

typedef enum {
    false = 0,
    true = 1
} bool;

int N = 50;
pthread_t tid[2];
pthread_t currTid;
sem_t sem_1;
sem_t sem_2;

void *func_print(void *args) {
    for (int i=1; i<=N; i++) {

        currTid = pthread_self();
        //printf("Entering => %ld\n", currTid);
        if (tid[0]==currTid) {
            //sleep(1);
            sem_wait(&sem_1);
            printf("%d => %ld\n", i, currTid);
            sem_post(&sem_2);
        } else {
            sem_wait(&sem_2);
            printf("%d => %ld\n", i, currTid);
            sem_post(&sem_1);
        }
        //printf("%d => %ld\n", i, currTid);
        //printf("\n");
    }
}

void *printNum(void* args) {
    for (int i=0; i<100; i++) {
        pthread_t currTid = pthread_self();
        if (currTid==tid[0]) {
            sem_wait(&sem_1);
            printf("%d => %ld\n", i, currTid);
            sem_post(&sem_2);
        } else {
            sem_wait(&sem_2);
            printf("%d => %ld\n", i, currTid);
            sem_post(&sem_1);
        }
    }
}

int main() {
    int err;

    err = sem_init(&sem_1, 0, 1);
    if (err!=0) printf("sem_init error\n");
    err = sem_init(&sem_2, 0, 0);
    if (err!=0) printf("sem_init error\n");

    err = pthread_create(&(tid[0]), NULL, &printNum, NULL);
    //sleep(1);
    err = pthread_create(&(tid[1]), NULL, &printNum, NULL);

    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);

    sem_destroy(&sem_1);
    sem_destroy(&sem_2);

    return 0;
}

