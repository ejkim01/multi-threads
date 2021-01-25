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

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_t tid[4];

void *printHello(void *args) {

    while (1) {
        //usleep(500000);
        //pthread_mutex_lock(&mutex1);
        if (pthread_mutex_trylock(&mutex2)==0) {
            sleep(1);
            pthread_mutex_unlock(&mutex2);
        } else {
            printf("Couldn't get lock2\n");
        }
        //pthread_mutex_unlock(&mutex1);
    }
}

void *printWorld(void *args) {

    while (1) {
        pthread_mutex_lock(&mutex1);
        pthread_mutex_lock(&mutex2);

        pthread_mutex_unlock(&mutex2);
        pthread_mutex_unlock(&mutex1);
    }
}


int main() {

    for (int i=0; i<4; i++)
        pthread_create(&(tid[i]), NULL, &printHello, NULL);
    //pthread_create(&tid2, NULL, &printWorld, NULL);

    for (int i=0; i<4; i++)
        pthread_join(tid[i], NULL);
    //pthread_join(tid2, NULL);
    
    return 0;
}
