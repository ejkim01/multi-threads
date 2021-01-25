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
    FALSE = 0,
    TRUE = 1
} bool;

pthread_mutex_t mutexFuel = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condFuel = PTHREAD_COND_INITIALIZER;
int fuel = 0;

void *fuelFilling(void* args) {
    for (int i=0; i<5; i++) {
        pthread_mutex_lock(&mutexFuel);
        fuel += 30;
        printf("Filled fuel... %d\n", fuel);
        pthread_mutex_unlock(&mutexFuel);
        pthread_cond_signal(&condFuel);
        sleep(1);
    }
}

void *car(void* args) {
    pthread_mutex_lock(&mutexFuel);
    while (fuel<40) {
        printf("No fuel. Waiting...\n");
        pthread_cond_wait(&condFuel, &mutexFuel);
    }
    fuel -= 40;
    printf("Got fuel. Now left: %d\n", fuel);
    pthread_mutex_unlock(&mutexFuel);
}

bool signalFlag = FALSE;
void *sender(void* args) {
    sleep(1);
    pthread_mutex_lock(&mutexFuel);
    signalFlag = TRUE;
    pthread_cond_signal(&condFuel);
    pthread_mutex_unlock(&mutexFuel);
}

void *receiver(void *args) {

    //sleep(3);
    pthread_mutex_lock(&mutexFuel);
    //while (signalFlag==FALSE) {
        printf("No fuel. Waiting...\n");
        pthread_cond_wait(&condFuel, &mutexFuel);
    //}
    signalFlag = TRUE;
    pthread_mutex_unlock(&mutexFuel);
    printf("Event processed\n");
}

int main() {

    pthread_t tid[6];
    for (int i=0; i<2; i++) {
        if (i==0)
            pthread_create(&tid[i], NULL, &sender, NULL);
        else
            pthread_create(&tid[i], NULL, &receiver, NULL);
    }

    for (int i=0; i<2; i++) {
        pthread_join(tid[i], NULL);
    }

    return 0;
}
