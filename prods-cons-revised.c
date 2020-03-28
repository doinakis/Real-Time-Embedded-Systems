/*
 *	File	: pc.c
 *
 *	Title	: Demo Producer/Consumer.
 *
 *	Short	: A solution to the producer consumer problem using
 *		pthreads.
 *
 *	Long 	:
 *
 *	Author	: Andrae Muys
 *
 *	Date	: 18 September 1997
 *
 *	Revised	: Doinakis Michalis doinakis@ece.auth.gr
 */

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <math.h>

#define QUEUESIZE 1500
#define LOOP 1000
#define   p   3
#define   c   2

//functions that simulate jobs added by producers
void* function_print_1(void* x);
void* function_print_2(void* x);
void* function_sin(void* x);
void* function_cos(void* x);
void* function_acos(void* x);
void* function_asin(void* x);
void* function_tan(void* x);
void* function_atan(void* x);

// array of the functions to pick one at a time at random
static void* (*functions_array[8])(void *) = {&function_print_1,&function_print_2,
                                              &function_sin,&function_cos,
                                              &function_asin,&function_acos,
                                              &function_tan,&function_atan};
static int random_arguments[10] = {1,2,3,4,5,6,7,8,9,10};
void *producer (void *args);
void *consumer (void *args);


typedef struct{

  void* (*work)(void*);
  void* arg;
  /*This part is added for testing purposes
    variables that hold when a function is added to the queue, when it arrives
    at the consumer and the substraction is the delay_time*/
  // struct timeval start_time,end_time;
  // double delay_time;

}workFunction;


typedef struct {
  //modify the buffer to take variables of type workFunction
  workFunction buf[QUEUESIZE];
  long head, tail;
  int full, empty;

  /*This part is added for testing purposes
  added mutex variable im_done to update counter whenever a producer is done*/

  // pthread_mutex_t *im_done;

  pthread_mutex_t *mut;
  pthread_cond_t *notFull, *notEmpty;

  //variable thst holds how many producer are done
  int done;

} queue;

queue *queueInit (void);
void queueDelete (queue *q);
//change the in and out to workFunction type
void queueAdd (queue *q, workFunction in);
void queueDel (queue *q, workFunction *out);

//file for each consumer to write how much did the job arrived took to arrive
FILE *f;

int main ()
{
  queue *fifo;

  //initialize array of threads of cons and prods
  pthread_t *pro = (pthread_t *) malloc(p * sizeof(pthread_t));
  pthread_t *con = (pthread_t *) malloc(c * sizeof(pthread_t));

  fifo = queueInit ();

  if (fifo ==  NULL) {
    fprintf (stderr, "main: Queue Init failed.\n");
    exit (1);
  }
  /*This part of the code is added for testing purposes
  //open the file where the consumers are going to write*/
  // f = fopen("file.csv","a");

  //check if open completed successfully
  // if (f == NULL){
  //     printf("Error opening file!\n");
  //     exit(1);
  // }
  //
  //create p producer threads
  for(int i =0; i<p;i++){
    pthread_create (&pro[i], NULL, producer, (void *)fifo);
  }

  //create c consumer threads
  for(int j=0;j<c;j++){
    pthread_create (&con[j], NULL, consumer, (void *)fifo);
  }

  //wait for all the producers/consumers to finish their work
  for(int i=0;i<p;i++){
    pthread_join (pro[i], NULL);
  }

  for(int j=0;j<c;j++){
    pthread_join (con[j], NULL);
  }

  //fclose(f);

  queueDelete (fifo);

  return 0;
}

//producer modifications
void *producer (void *q)
{
  queue *fifo;
  int i;

  //add a void variable to pass the integer as void * type to the function
  void * pointer;
  //srand to pick a function at random
  srand(time(NULL));

  fifo = (queue *)q;
  for (i = 0; i < LOOP; i++) {
    pthread_mutex_lock (fifo->mut);
    while (fifo->full) {
      printf ("producer: queue FULL.\n");
      pthread_cond_wait (fifo->notFull, fifo->mut);
    }

    //add the function in the queue
    workFunction  work;
    work.work = functions_array[rand()%8];
    pointer = &random_arguments[rand()%10];
    work.arg = pointer;
    /*This part of the code is added for testing purposes*/

    // gettimeofday(&work.start_time,NULL);

    queueAdd (fifo, work);
    pthread_mutex_unlock (fifo->mut);
    pthread_cond_signal (fifo->notEmpty);

  }
  /*This part of the code is added for testing purposes
    producer done update variable
  pthread_mutex_lock (fifo->im_done);
  fifo->done++;
  pthread_mutex_unlock (fifo->im_done);
  */

  return (NULL);
}

void *consumer (void *q)
{
  queue *fifo;
  //make d a workFunction variable
  workFunction d;
  fifo = (queue *)q;

  // change the loop to while 1
  while(1){
    pthread_mutex_lock (fifo->mut);
    while (fifo->empty) {
      printf ("consumer: queue EMPTY.\n");
      /*This part of the code is added for testing purposes*/
      //close the file if the queue is empty and all the producers are done
      // if(fifo->done == p){
      //   fclose(f);
      // }

      pthread_cond_wait (fifo->notEmpty, fifo->mut);
    }
    //d is a workFunction variable produced by the producer
    queueDel (fifo, &d);

    /*This part of the code is added for testing purposes*/
    // gettimeofday(&d.end_time,NULL);
    // d.delay_time = (double)((d.end_time.tv_usec-d.start_time.tv_usec)/1.0e6+d.end_time.tv_sec-d.start_time.tv_sec);
    // fprintf(f,"%f \n",d.delay_time);
    pthread_mutex_unlock (fifo->mut);
    pthread_cond_signal (fifo->notFull);


    /*keep in mind that with the calculation inside the critical part we may add
    some overhead to the next delay_times */
    (*d.work)(d.arg);
  }
  return (NULL);
}



queue *queueInit (void)
{
  queue *q;

  q = (queue *)malloc (sizeof (queue));
  if (q == NULL) return (NULL);

  q->done = 0;
  q->empty = 1;
  q->full = 0;
  q->head = 0;
  q->tail = 0;
  q->mut = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
  pthread_mutex_init (q->mut, NULL);

  /*This part of the code is added for testing purposes*/

  // q->im_done = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
  // pthread_mutex_init (q->im_done, NULL);

  q->notFull = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
  pthread_cond_init (q->notFull, NULL);
  q->notEmpty = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
  pthread_cond_init (q->notEmpty, NULL);

  return (q);
}

void queueDelete (queue *q)
{
  pthread_mutex_destroy (q->mut);
  free (q->mut);
  /*This part of the code is added for testing purposes*/

  // pthread_mutex_destroy (q->im_done);
  // free (q->im_done);

  pthread_cond_destroy (q->notFull);
  free (q->notFull);
  pthread_cond_destroy (q->notEmpty);
  free (q->notEmpty);
  free (q);
}

//changed the in variable to be type workFunction
void queueAdd (queue *q, workFunction in)
{
  q->buf[q->tail] = in;
  q->tail++;
  if (q->tail == QUEUESIZE)
    q->tail = 0;
  if (q->tail == q->head)
    q->full = 1;
  q->empty = 0;

  return;
}

//changed the out variable to be type workFunction
void queueDel (queue *q, workFunction *out)
{
  *out = q->buf[q->head];

  q->head++;
  if (q->head == QUEUESIZE)
    q->head = 0;
  if (q->head == q->tail)
    q->empty = 1;
  q->full = 0;

  return;
}


//series of  functions that just print something
// compile like that to work gcc func_pointer.c -o func_pointer -lm
void* function_print_1(void* arg){
  //print a message
  printf("Function 1 called randomly, the random argument is : %d  \n",*((int *) arg));
  return (NULL);
}

void*  function_print_2(void* arg){
  //print a message
  printf("Function 2 called randomly, the random argument is : %d  \n",*((int *) arg));
  return (NULL);
}

void* function_sin(void* arg){
  srand(time(NULL));
  //compute 10 random integer sins
  for(int i =0; i < 9; i++){
    sin(rand() % 361);
  }
  return (NULL);
}

void*  function_cos(void* arg){
  srand(time(NULL));
  //compute 10 random integer cos
  for(int i =0; i < 9; i++){
    cos(rand() % 361);
  }
  return (NULL);
}

void*  function_asin(void* arg){
  srand(time(NULL));
  //compute 10 random integer asin
  for(int i =0; i < 9; i++){
    asin(rand() % 361);
  }
  return (NULL);
}

void*  function_acos(void* arg){
  srand(time(NULL));
  //compute 10 random integer acos
  for(int i =0; i < 9; i++){
    acos(rand() % 361);
  }
  return (NULL);
}

void*  function_tan(void* arg){
  srand(time(NULL));
  //compute 10 random integer tan
  for(int i =0; i < 9; i++){
    tan(rand() % 361);
  }
  return (NULL);
}

void*  function_atan(void* arg){
  srand(time(NULL));
  //compute 10 random integer atan
  for(int i =0; i < 9; i++){
    atan(rand() % 361);
  }
  return (NULL);
}
