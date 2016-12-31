#include "cs537.h"
#include "request.h"
#include <pthread.h> //Added
#include "queue.h" //Added

// 
// server.c: A very, very simple web server
//
// To run:
//  server <portnum (above 2000)>
//
// Repeatedly handles HTTP requests sent to this port number.
// Most of the work is done within routines written in request.c
//

// CS537: Parse the new arguments too

// Added:
// original:
// void getargs(int *port, int argc, char *argv[])
// Modified to handle four inputs.
void getargs(int *port, int argc, char *argv[], int* threads, int* buffers)
{
  //Added: Modified from "!= 2" to "!= 4"
  if (argc != 4) {
    //Added: Modified usage for all four inputs	
    fprintf(stderr, "Usage: %s <port> <thread> <buffers>\n", argv[0]);
    exit(1);
  }
  *port = atoi(argv[1]);

  //Added:
  //get the inputs for threads and buffers
  //TODO: do we need to check fo accurate inputs?
  *threads = atoi(argv[2]);
  *buffers = atoi(argv[3]);
}

//Added:
//function for the threads to handle requests in.  Will never return.
static void* thread_start(void* arg)
{
  int connfd;
  queue_t* queue = (queue_t*) arg;

  while(1)
  {
    pthread_mutex_lock(&queue->lock);

    while(queue->size <= 0)
    {
      pthread_mutex_unlock(&queue->lock);
      //TODO implement conditional variables
      pthread_mutex_lock(&queue->lock);
    }
    connfd = dequeue(queue);
    pthread_mutex_unlock(&queue->lock);

    requestHandle(connfd);

    Close(connfd);
  }//end of while(1)

  //should never happen
  while(1); //block return

  return NULL;
}//end of thread_start


int main(int argc, char *argv[])
{
  //Added:
  int num_threads, num_buffers;
  pthread_t* thread_array;
  queue_t* queue;

  int listenfd, connfd, port, clientlen;
  struct sockaddr_in clientaddr;

  //Added:
  //original:
  //getargs(&port, argc, argv);
  //
  //Modified for four inputs.
  getargs(&port, argc, argv, &num_threads, &num_buffers);

  //Added:
  //Create array for threads array is larger than needed by one thread
  thread_array = malloc(sizeof(pthread_t)*(num_threads+1));

  //Added:
  //Initialized a global queue to store connfd in
  queue = malloc(sizeof(queue_t));
  init_queue(queue, num_buffers);

  //Added: 
  //create num_threads number of threads
  int i;  
  for(i = 0; i<num_threads; i++)
  {
    if(pthread_create(&thread_array[i], NULL, &thread_start, (void*) queue) != 0)
    {
      fprintf(stderr, "Threads failed to create\n");
      exit(1);
    }
  } 
  // */
  // 
  // CS537: Create some threads...
  //

  listenfd = Open_listenfd(port);

  while (1) {
    clientlen = sizeof(clientaddr);

    connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);


    pthread_mutex_lock(&queue->lock);
    while(queue->size >= queue->max_size)
    {
      pthread_mutex_unlock(&queue->lock);
      //TODO implement conditional variable to avoid spin waiting
      pthread_mutex_lock(&queue->lock);
    }
    if (enqueue(queue, connfd) == -1)
    {
      printf("enqueue failed\n");
    }

    pthread_mutex_unlock(&queue->lock);

    // 
    // CS537: In general, don't handle the request in the main thread.
    // Save the relevant info in a buffer and have one of the worker threads 
    // do the work. However, for SFF, you may have to do a little work
    // here (e.g., a stat() on the filename) ...
    // 
    //Added:
    //moved the handling and closing to thread_start for the threads to handle.
    /*       requestHandle(connfd);

             Close(connfd);
    // */
  } //End of while(1) loop

} //End of main






