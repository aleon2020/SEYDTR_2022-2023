#define _GNU_SOURCE
#include <pthread.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

/*Matrix that reads the data from the csv file 
(I have established 10 as the maximum number of cores that can appear, 
although in this practice there are 6 cores in the laboratories 
and 4 in the raspberry pi)*/
long long files_data[10][2];

void *calculate_latency_function(void *ptr);

/*Methods*/
void *calculate_latency_function(void *ptr){
  int CPU = *(int *) ptr;
  cpu_set_t set;
  FILE *csv = fopen("cyclictestURJC.csv","w+");
  struct sched_param param;
  param.sched_priority = 99;
  CPU_ZERO(&set);
  CPU_SET(CPU,&set);
  /*Each thread must execute in the SCHED_FIFO priority queue, with priority 99*/
  pthread_setschedparam(pthread_self(),SCHED_FIFO,&param);
  /*Affinity is established, since each thread must run on a different core*/
  pthread_setaffinity_np(pthread_self(),sizeof(cpu_set_t),&set);
  struct timespec begin, begin_latency, end, end_latency, max, deadline;
  long LATENCY;
  max.tv_nsec = 0;
  int ITERATION_NUMBER = 0, executing = 1;
  long long average = 0;
  double execution_time;
  clock_gettime(CLOCK_MONOTONIC,&begin);
  while(executing){
    deadline.tv_nsec = 1000, deadline.tv_sec = 0;
    clock_gettime(CLOCK_MONOTONIC,&begin_latency);
    /*The thread must be constantly calculating the scheduling latency using
    sleeping techniques (perform waits of the order of milliseconds).*/
    clock_nanosleep(CLOCK_MONOTONIC,TIMER_ABSTIME,&deadline,NULL);
    clock_gettime(CLOCK_MONOTONIC,&end_latency);
    LATENCY = (end_latency.tv_nsec + end_latency.tv_sec*1000000000) - (begin_latency.tv_nsec+begin_latency.tv_sec * 1000000000) - deadline.tv_nsec;
    if(LATENCY > max.tv_nsec){
      max.tv_nsec = LATENCY;
    }
    average = average + LATENCY;
    ITERATION_NUMBER++;
    clock_gettime(CLOCK_MONOTONIC,&end);
    execution_time = (end.tv_sec - begin.tv_sec);
    /*It must be saved in a CSV file (cyclictestURJC.csv) in palin text format,
    all measurements obtained in the following format:
    CPU,ITERATION_NUMBER,LATENCY*/
    fprintf(csv,"%d, %d, %ld\n",CPU,ITERATION_NUMBER,LATENCY);
    /*This program must run continuously for 1 minute*/
    if(execution_time >= 60){
      executing = 0;
    }
  }
  files_data[CPU][0] = average / ITERATION_NUMBER;
  files_data[CPU][1] = max.tv_nsec;
  printf( "[%d] latencia media = %lld ns. | max = %lld ns\n",CPU,files_data[CPU][0],files_data[CPU][1]);
}

/*Principal function (main method)*/
int main(){
  /*The program starts by opening the file /dev/cpu_dma_latency and writing a 0*/
  static int32_t latency_target_value = 0;
  int latency_target_fd = open("/dev/cpu_dma_latency", O_RDWR);
  write(latency_target_fd, &latency_target_value, 4);
  /*Function used to know the number of cores*/
  int N = (int) sysconf(_SC_NPROCESSORS_ONLN);
  pthread_t thread[N];
  for (int i = 0; i < N; i++) {
    int *id = malloc(sizeof(int));
    *id = i;
    pthread_create(&thread[i], NULL, calculate_latency_function, (void*) id);
  }
  for (int i = 0; i < N; i++){
    pthread_join(thread[i],NULL);
  }
  long totalAverage = 0, totalMax = 0;
  for(int i = 0; i < N; i++){
    if(files_data[i][1] > totalMax){
      totalMax = files_data[i][1];
    }
    totalAverage = totalAverage + files_data[i][0];
  }
  totalAverage = totalAverage / N;
  printf("Total latencia media = %ld ns. | max = %ld ns\n", totalAverage, totalMax);
  exit(0);
}
