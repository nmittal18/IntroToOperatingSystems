#ifndef __QUEUE_H_
#include <pthread.h>

//Added:
//linked list queue and node structure
typedef struct __node_t
{
  int fd;
  struct __node_t* next;
}node_t;

typedef struct __queue_t
{
  int size;
  int max_size;
  node_t* head;
  node_t* tail;
  pthread_mutex_t lock;
}queue_t;

//Added:
//functions to initialize, enqueue and dequeue elements
void init_queue(queue_t* q, int mx_sz)
{
  q->head = NULL;
  q->tail = NULL;
  q->size = 0;
  q->max_size = mx_sz;
  pthread_mutex_init(&q->lock,NULL);
}

int enqueue(queue_t* q, int f)
{
  if(q->size == q->max_size)
    return -1;

  node_t* new = malloc(sizeof(node_t));

  if(new <= 0)
    return -1;

  new->fd = f;
  new->next = NULL;
  //  pthread_mutex_lock(&q->lock);
  q->size++;

  if(q->size == 1)//adding first element to the queue
  {
    q->tail = new;
    q->head = new;
  }
  else
  {
    q->tail->next = new;
    q->tail = new;
  }

  //  pthread_mutex_unlock(&q->lock);
  return 0;
}

int dequeue(queue_t* q)
{
  if(q->size <= 0)
    return -1;

  node_t* h;
  //  pthread_mutex_lock(&q->lock);
  h = q->head;
  q->head = q->head->next;
  q->size--;
  if(q->size == 0)
    q->tail = NULL;

  //  pthread_mutex_unlock(&q->lock);

  int f = h->fd;

  free(h);

  return f;
}
#endif
