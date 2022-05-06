#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <minix/mthread.h>
#include <stdint.h>

#define NTHREADS 60
void *thread_function(void *);
int  counter = 0;

static uint64_t gettime(void)
{
  uint64_t res;
  struct timeval tv;

  gettimeofday(&tv, NULL);

  res = tv.tv_sec;
  res = res * 1000000 + tv.tv_usec;

  return res;
}

struct thread_data{
    int thread_id;
    int loopCount;
    int maxLoop;
    int prio;
    clock_t start_time;
    double RT;
    double TAT_single;
};

int main()
{
   int loopCounts[10] =
   {10000, 20000, 30000, 10000, 10000, 20000, 30000, 10000, 10000, 20000};

   int maxLoops[10] =
   {100000, 200000, 300000, 100000, 100000, 200000, 300000, 100000, 100000, 200000};
   int prios[10] = {60, 50, 40, 60, 60, 50, 40, 60, 60, 50};
   mthread_thread_t thread_id[NTHREADS];
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
      mthread_create( &thread_id[i], NULL, thread_function, (void *)&(td[i]));
      printf("Thread %d Created\n", i);
   }

   for(j=0; j < NTHREADS; j++)
   {
      mthread_join( thread_id[j], NULL);
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
   mthread_exit(NULL);
   return 0;
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
unsigned int  v = mthread_self();;
int	loopCount = t_data->loopCount;
int maxloops = t_data->maxLoop;
int iteration = 1;
int i = 0;

nice(t_data->prio);

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
      //printf ("%s:%06d\n", idStr, iteration);
     //   fflush (stdout);
      iteration += 1;
      i = 0;
    }
  }
  end = gettime();
  resp = (start - t_data->start_time)/1000000;
  t_data->RT = resp;
  t_data->TAT_single = (double)(end-start)/1000000;
  printf("Thread %d TAT: %f WT: %f (result: %d)\n", id, (double)(end-start)/1000000, (double)resp, v);
  mthread_exit(NULL);



}
