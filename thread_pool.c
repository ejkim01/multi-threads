#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define THREAD_NUM 1

typedef struct Task {
    //int a, b;
    int (*taskFunction)(int, int);
    int arg1, arg2;
} Task;

Task taskQueue[256];
int sumTasks[256] = {0,};
int sumIndex = 0;
//int prodTasks[256] = {0,};
//int prodIndex = 0;
int taskCount = 0;

pthread_mutex_t mutexQueue;
pthread_cond_t condQueue;

int sum(int a, int b) {
    usleep(50000);
    //sumTasks[sumIndex++] = a + b;
    int sum = a + b;
    printf("Sum of %d and %d is %d\n", a, b, sum);
    return sum;
}

//int product(int a, int b) {
//    usleep(50000);
//    //prodTasks[prodTasks++] = a * b;
//    printf("Prod of %d and %d is %d\n", a, b, prod);
//    return prod;
//}

int executeTask(Task* task) {
    return task->taskFunction(task->arg1, task->arg2);
    //usleep(50000);
    //int result = task->a + task->b;
    //printf("The sum of %d and %d is %d\n", task->a, task->b, result);
}

void submitTask(Task task) {
    pthread_mutex_lock(&mutexQueue);
    taskQueue[taskCount] = task;
    taskCount++;
    pthread_mutex_unlock(&mutexQueue);
    pthread_cond_signal(&condQueue);
}

void* startThread(void* args) {
    while (1) {
        Task task;

        pthread_mutex_lock(&mutexQueue);
        while (taskCount == 0) {
            pthread_cond_wait(&condQueue, &mutexQueue);
        }

        task = taskQueue[0];
        int i;
        for (i = 0; i < taskCount - 1; i++) {
            taskQueue[i] = taskQueue[i + 1];
        }
        taskCount--;
        pthread_mutex_unlock(&mutexQueue);
        executeTask(&task);
    }
}

int main(int argc, char* argv[]) {
    pthread_t th[THREAD_NUM];
    pthread_mutex_init(&mutexQueue, NULL);
    pthread_cond_init(&condQueue, NULL);
    int i;
    for (i = 0; i < THREAD_NUM; i++) {
        if (pthread_create(&th[i], NULL, &startThread, NULL) != 0) {
            perror("Failed to create the thread");
        }
    }

    srand(time(NULL));
    for (i = 0; i < 30; i++) {
        Task t = {
            .taskFunction = &sum,
            .arg1 = rand() % 100,
            .arg2 = rand() % 100
        };
        submitTask(t);
    }

    for (i = 0; i < THREAD_NUM; i++) {
        if (pthread_join(th[i], NULL) != 0) {
            perror("Failed to join the thread");
        }
    }
    for (int i=0; i<sumIndex; i++) {
        printf("sumTasks[%d] = %d\n", i, sumTasks[i]);
    }
    //for (int i=0; i<prodIndex; i++) {
    //    printf("prodTasks[%d] = %d\n", i, prodTasks[i]);
    //}
    pthread_mutex_destroy(&mutexQueue);
    pthread_cond_destroy(&condQueue);
    return 0;
}

