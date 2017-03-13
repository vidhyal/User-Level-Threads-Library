//#include "scheduler.h"
#include "queue.h"
#include "threadmap.c"
#include <stdio.h>
#include <stdlib.h>
#define _GNU_SOURCE
#include <sched.h>




struct thread * current_thread;
struct queue *ready_list;
int threadID =1;


void thread_wrap() {
    current_thread->initial_function(current_thread->initial_argument);
    current_thread->state = DONE;  // to let the scheduler know that this thread need not be scheduled anymore
    condition_signal(&(current_thread->cond)); // to signal any thread waiting on this thread's condition as a result of join
    yield();
}

void scheduler_begin(){
    // allocate memory for thread and readylist data structures
    set_current_thread ( malloc(sizeof(struct thread)));
    ready_list = malloc(sizeof(struct queue));
    current_thread->state = RUNNING;   
    mutex_init(&(current_thread->mutex1));
    condition_init(&(current_thread->cond));
   
    // prepare the readylist to be used for scheduling
    ready_list->head = NULL;
    ready_list->tail = NULL;    

    // clone for kernel
    int pid;
    int stack_size = 1024*1024;
    char *stackTop = malloc(stack_size)+stack_size;
    pid = clone (kernel_thread_begin, stackTop, CLONE_THREAD| CLONE_VM| CLONE_SIGHAND,0); 
}

int * kernel_thread_begin(void *arg){
    struct thread* idle_thread; 
    // no need to allocate stack or initial function for this thread coz this is the empty thread
    // we are taking about in figure 2
    idle_thread = malloc(sizeof(struct thread));
    idle_thread->state = RUNNING;
    set_current_thread (idle_thread);
    for (;;)
        yield(); // the idle thread would yield forever, in a way the kernel thread polls for work
// problem: infinitely looping program never ending

}

struct thread * thread_fork(void(*target)(void*), void*arg) {
    //create a new thread, allocate memory for its structure, stack and initialize its fields
    struct thread *new_thread;
    int stack_size = 1024*1024;
    new_thread = malloc(sizeof(struct thread));
    new_thread->stack_pointer = malloc(stack_size)+stack_size;
    new_thread->initial_argument = arg;
    new_thread->initial_function = target;
    mutex_init(&(new_thread->mutex1));
    condition_init(&(new_thread->cond));

    // prepare the current thread to be queued in the scheduler's ready list and enqueue it.
    current_thread->state = READY;
    thread_enqueue ( ready_list, current_thread);

    // prepare the new thread to run and switch the threads
    new_thread->state= RUNNING;
    struct thread *temp_thread = current_thread;
    set_current_thread (new_thread);

    thread_start (temp_thread, current_thread);

    // return to the calling function the thread created and started by the 
    // current_thread( the thread that was initially current)
    return new_thread;
}

void thread_join(struct thread* t){
    // check if the thread t ( the one being waited on) is done, if not, make the 
    // calling thread wait on the condition of 't', by locking its mutex first
    while( t->state != DONE){
        mutex_lock(&(t->mutex1));
        condition_wait(&(t->cond),&(t->mutex1));
     }
   
}


void yield() {
    // if the current thread has not done executing and is not blocked, add it to the scheduler's ready list.
    if (current_thread->state != DONE && current_thread->state !=BLOCKED){
        current_thread->state = READY;
        thread_enqueue (ready_list, current_thread);
    }
    // get the next thread from the ready list and prepare it for running
    struct thread *next_thread;
    if (!is_empty(ready_list)) {
        next_thread = thread_dequeue (ready_list);
          #ifdef DEBUG
              printList(ready_list->head);
          #endif
        next_thread->state = RUNNING;
    }
    else {
    //     next_thread = idle_thread;
	printf("fatal error empty ready list\n");
        exit(1);
    }


    // switch the current thread to the next thread 
    struct thread *temp_thread = current_thread;
    set_current_thread (next_thread);

    thread_switch(temp_thread, current_thread);
}

void scheduler_end() {
    // do not let the main thread exit if there are still threads, waiting to be run
    while (!is_empty(ready_list)){
        yield();
   }
}

void mutex_init(struct mutex *mutex1){
    // initialize all fields of the struct mutex
    mutex1->held =0;
    mutex1->heldBy = NULL;
    mutex1->waiting_threads =malloc(sizeof(struct queue));
    mutex1->waiting_threads->head = NULL;
    mutex1->waiting_threads->tail = NULL;
}

void mutex_lock (struct mutex *mutex1) {
    // error exit if thread holding the mutex tries to lock it
    if (mutex1->heldBy == current_thread){
        printf(" mutex held by current thread\n");
        exit(1); 
    }
    // set the lock if it is free and make this thread hold it
    if (mutex1->held ==0){
        mutex1->held =1;   // this thread got the lock
        mutex1->heldBy = current_thread;
    }
    // if the lock is not free, block the thread and put it on the mutex's blocked list
    else {
         current_thread->state = BLOCKED;
         thread_enqueue(mutex1->waiting_threads, current_thread);
         yield();
    } 
}

void mutex_unlock (struct mutex* mutex1){
    // allow unlock only if the current thread holds the lock
    if (mutex1->heldBy == current_thread){

        // if there are threads waiting for this mutex, unblock one, give it the lock 
        // and put it on ready_list
        if (!is_empty(mutex1->waiting_threads)){
            struct thread *t = thread_dequeue(mutex1->waiting_threads);
            t->state = READY;
            thread_enqueue(ready_list, t);
            mutex1->heldBy =t;
        }
        // if no threads waiting for mutex, reset the lock.
        else {
            mutex1->heldBy= NULL;
            mutex1->held =0;
        }
    }
    // error exit on threads not holding lock trying to unlock
    else {
      printf(" thread not holding mutex trying to unlock it\n");
      exit(1);
    }
}

void condition_init(struct condition * cond){
    // initialize all the fields of the condition structure
    cond->waiting_threads = malloc(sizeof(struct queue));
    cond->waiting_threads->head = NULL;
    cond->waiting_threads->tail = NULL;
}

void condition_wait(struct condition * cond, struct mutex* mutex1){
    // error exit if mutex is not locked
    if( mutex1->heldBy != current_thread){
        printf("condition's mutex not held by current thread\n");
        exit(1);
    }
    // unlock the mutex before putting the threa holding it to block ( else deadlock!)
    // block the thread and put it on condition's waiting list
    mutex_unlock (mutex1);
    current_thread->state = BLOCKED;
    thread_enqueue(cond->waiting_threads, current_thread);
    yield();
}

void condition_signal(struct condition* cond){
    // check if any thread is waiting on the conditions list and if yes, make the
    // first one ready and put it on ready_list
    if (!is_empty(cond->waiting_threads)){
        struct thread * t = thread_dequeue(cond->waiting_threads);
        t->state = READY;
        thread_enqueue(ready_list,t);
    }
}

void condition_broadcast(struct condition* cond){
    // put all the threads waiting on the condition on to the ready_list
    while (!is_empty(cond->waiting_threads)){
        struct thread * t = thread_dequeue(cond->waiting_threads);
        t->state = READY;
        thread_enqueue(ready_list,t);
    }
}