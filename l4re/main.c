#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <pthread-l4.h>
#include <signal.h>
#include <stdint.h>
#include <l4/re/env.h>


static uint64_t gettime(void)
{
  uint64_t res;
  struct timeval tv;

  gettimeofday(&tv, NULL);

  res = tv.tv_sec;
  res = res * 1000000 + tv.tv_usec;

  return res;
}

#define NTHREADS 100
void *thread_function(void *);
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
int  counter = 0;

struct thread_data{
    int thread_id;
    int loopCount;
    int maxLoop;
    int prio;
    clock_t start_time;
    double RT;
    double TAT_single;
};

int main(void)
{
   int loopCounts[10] = 
   {10000, 20000, 30000, 10000, 10000, 20000, 30000, 10000, 10000, 20000};

   int maxLoops[10] = 
   {100000, 200000, 300000, 100000, 100000, 200000, 300000, 100000, 100000, 200000};

   int prios[10] = {60, 50, 40, 60, 60, 50, 40, 60, 60, 50};

    pthread_t thread_id[NTHREADS];
    pthread_attr_t a[NTHREADS];
    struct sched_param sp[NTHREADS];

   clock_t start;
   clock_t end;
   int i, j;
   struct thread_data td[NTHREADS];

    double response_time = 0, turn_arround_time = 0;

   start = gettime();
   for(i=0; i < NTHREADS; i++)
   {
      td[i].thread_id = i;
      td[i].loopCount = loopCounts[i%10];
      td[i].maxLoop = maxLoops[i%10];
      td[i].prio = prios[i%10];
      td[i].start_time = gettime();
    
    pthread_attr_init(&a[i]);
    // a[i].create_flags |= PTHREAD_L4_ATTR_NO_START;
    sp[i].sched_priority = prios[i%10];
    pthread_attr_setschedpolicy(&a[i], SCHED_L4);
    pthread_attr_setschedparam(&a[i], &sp[i]);
    pthread_attr_setinheritsched(&a[i], PTHREAD_EXPLICIT_SCHED);

      pthread_create( &thread_id[i], NULL, thread_function, (void *)&(td[i]));
      printf("Thread %d Created\n", i);
    //   pthread_attr_destroy(&a[i]);

    //   l4_scheduler_run_thread(SCHED_L4,pthread_l4_cap(thread_id[i]), l4_sched_param(prios[i], 0));
   }
//    for (i=0; i< NTHREADS; i++){
//        l4_scheduler_run_thread(l4re_env()->scheduler, thread_id[i], &sp[i]);
//    }


   for(j=0; j < NTHREADS; j++)
   {
      pthread_join( thread_id[j], NULL); 
      response_time +=td[j].RT;
      turn_arround_time += td[j].TAT_single;
   }
  
   /* Now that all threads are complete I can print the final result.     */
   /* Without the join I could be printing a value before all the threads */
   /* have been completed.                                                */
   end = gettime();
   double TAT = (double)(end-start)/1000000;
   double AvRT = response_time/NTHREADS;
   double AvTAT = turn_arround_time/NTHREADS;
   double TP = TAT/NTHREADS;
   printf("TT: %f, AvRT: %f, AvTAT: %f, TP: %f\n", TAT, AvRT, AvTAT, TP);
   pthread_exit(NULL);
}

void *thread_function(void *threadarg)
{
//    printf("Thread number %ld\n", pthread_self());
//    pthread_mutex_lock( &mutex1 );
//    counter++;
//    pthread_mutex_unlock( &mutex1 );
//    printf("%d\n", counter);
clock_t start, end, resp;

struct thread_data *t_data;
t_data = (struct thread_data *) threadarg;

int id = t_data->thread_id;

unsigned int v = pthread_self();
v = -v;
int	loopCount = t_data->loopCount;
int maxloops = t_data->maxLoop;
int iteration = 1;
int i = 0;
// printf("%d\n", v);

// nice(t_data->prio);

start = gettime();
while (1) {
    /* This calculation is done to keep the value of v unpredictable. Since
the compiler can't calculate it in advance (even from the original
value of v and the loop count), it has to do the loop. */
    v = (v << 4) - v;
    if (++i == loopCount) {
      /* Exit if we've reached the maximum number of loops. If maxloops is
0 (or negative), this'll never happen... */
      if (iteration == maxloops) {
break;
      }
    //   printf ("%06d\n",iteration);
        // fflush (stdout);
      iteration += 1;
      i = 0;
    }
  }
  end = gettime();
  resp = start - t_data->start_time;
  t_data->RT = resp/1000000;
  t_data->TAT_single = (double)(end-start)/1000000;
  printf("Thread %d TAT: %fs RT: %fs (result: %d)\n", id, (double)(end-start)/1000000, (double)resp/1000000, v);
  pthread_exit(NULL);
}