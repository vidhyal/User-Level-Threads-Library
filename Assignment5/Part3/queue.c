/*
 * CS533 Course Project
 * Thread Queue ADT
 * queue.c
 *
 * Feel free to modify this file. Please thoroughly comment on
 * any changes you make.
 */


#include "queue.h"
#include <stdlib.h>

void thread_enqueue(struct queue * q, struct thread * t) {

  struct queue_node * temp = malloc(sizeof(struct queue_node));

  temp->t = t;
  temp->next = NULL;

  if(!q->head) {
    q->head = q->tail = temp;
    return;
  }
  q->tail->next = temp;
  q->tail = temp;  

}

struct thread * thread_dequeue(struct queue * q) {

  if(!q->head) {
    return NULL;
  }

  struct thread * t = q->head->t;
  struct queue_node * temp = q->head;
  q->head = q->head->next;
  free(temp);

  if(!q->head) {
    q->tail = NULL;
  }

  return t;

}

int is_empty(struct queue * q) {
  return !q->head;
}

extern void * safe_mem(int, void*);
#define malloc(srg) safe_mem(0,((void*) (arg)))
#define free(arg) safe_mem(1,arg)
