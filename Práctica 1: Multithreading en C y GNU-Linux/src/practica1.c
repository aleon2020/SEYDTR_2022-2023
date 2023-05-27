#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NUM_THREADS 4
#define DEADLINE 0.9
#define NS 1000000000

void *print_message_function(void *ptr);

/*Methods*/
void *print_message_function(void *ptr) {
    char *message;
    message = (char *) ptr;
    for (int i = 1; i < 6; i++) {
        struct timespec time;
        clock_gettime(CLOCK_REALTIME, &time);
        struct timespec begin;
        clock_gettime(CLOCK_MONOTONIC, &begin);
        volatile unsigned long long j;
        for (j=0; j < 400000000ULL; j++);
        struct timespec end;
        clock_gettime(CLOCK_MONOTONIC, &end);
        double seconds = end.tv_sec - begin.tv_sec;
        double nanoseconds = end.tv_nsec - begin.tv_nsec;
        nanoseconds = nanoseconds / NS;
        double cost = seconds + nanoseconds;
        double wait = (DEADLINE - cost) * NS;
        struct timespec sleep;
        sleep.tv_sec = 0;
        sleep.tv_nsec = wait;
        printf("[%ld.%ld] ", time.tv_sec, time.tv_nsec);
        printf("%s - Iteracion %d: Coste=%.2f s.", message, i, cost);  
        if (cost <= DEADLINE) {
            printf("\n");
            struct timespec rem;
            nanosleep(&sleep, &rem);
        } 
        else {
            printf(" (fallo temporal)\n");
        }            
    }
}

/*Principal function (main method)*/
int main() {
    pthread_t threads[NUM_THREADS];
    char *messages[NUM_THREADS] = {"Thread 1", "Thread 2", "Thread 3", "Thread 4"};
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, (void*)print_message_function, (void *) messages[i]);
    }
    for (int j = 0; j < NUM_THREADS; j++) {
        pthread_join(threads[j], NULL);
    }
    exit(0);
}