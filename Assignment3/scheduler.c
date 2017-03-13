#include "scheduler.h"
#include "queue.h"
#include <stdio.h>
#include <stdlib.h>

void thread_wrap() {
    current_thread->initial_function(current_thread->initial_argument);
    current_thread->state = DONE;  // to let the scheduler know that this thread need not be scheduled anymore
   // for(;;)                        // infinite loop to fix segmentation fault
    yield();
}



struct thread * current_thread;
struct queue *ready_list;
int threadID =1;

void scheduler_begin(){
    // allocate memory for thread and realylist data structures
    current_thread = malloc(sizeof(struct thread));
    ready_list = malloc(sizeof(struct queue));
    current_thread->state = RUNNING;      
    // preapre the readylist to be used for scheduling
    ready_list->head = NULL;
    ready_list->tail = NULL;
}

void thread_fork(void(*target)(void*), void*arg) {
    //create a new thread, allocate memory for its structure, stack and initialize its fields
    struct thread *new_thread;
    int stack_size = 1024*1024;
    new_thread = malloc(sizeof(struct thread));
    new_thread->stack_pointer = malloc(stack_size)+stack_size;
    new_thread->initial_argument = arg;
    new_thread->initial_function = target;

    #ifdef DEBUG
        new_thread->thread_id = threadID;
        threadID++;
        printf("current_thread is thread%d\n", current_thread->thread_id);
    #endif

    // prepare the current thread to be queued in the scheduler's ready list and enqueue it.
    current_thread->state = READY;
    thread_enqueue ( ready_list, current_thread);

    // prepare the new thread to run and switch the threads
    new_thread->state= RUNNING;
    struct thread *temp_thread = current_thread;
    current_thread = new_thread;
    #ifdef DEBUG
        printf("creating thread thread%d\n", current_thread->thread_id);
    #endif
    thread_start (temp_thread, current_thread);
}

void yield() {
    // if the current thread has not done executing, add it to the scheduler's ready list.
    if (current_thread->state != DONE){
        current_thread->state = READY;
        thread_enqueue (ready_list, current_thread);
    }
    // get the next thread from the ready list and prepare it for running
    struct thread *next_thread;
    if (!is_empty(ready_list)) {
        next_thread = thread_dequeue (ready_list);
        next_thread->state = RUNNING;
    }

    #ifdef DEBUG
        printf("current thread is thread%d\n", current_thread->thread_id);
    #endif

    // switch the current thread to the next thread 
    struct thread *temp_thread = current_thread;
    current_thread = next_thread;
    #ifdef DEBUG
        printf("switching to thread thread%d\n", current_thread->thread_id);
    #endif
    thread_switch(temp_thread, current_thread);
}

void scheduler_end() {
    // do not let the main thread exit if there are still threads, waiting to be run
    while (!is_empty(ready_list)){
        #ifdef DEBUG 
            printf("current thread = thread%d wants to terminate, but queue is not empty\n", current_thread->thread_id);
        #endif
        yield();
   }
}