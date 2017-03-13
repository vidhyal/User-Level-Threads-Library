#include "Thread.h"
#include <stdio.h>
#include <stdlib.h>


struct thread_t *current_thread, *inactive_thread;
void yield();

int factorial(int n) {
    return n ==0? 1: n*factorial(n-1);
}


void fun_with_threads(void* arg){
    int i;
    for (i =0; i<8; i++)
    {
        printf ("in fun_with_threads\n");
        int n = *(int*) arg + i;
        printf("%d! = %d\n", n, factorial(n));

        #ifdef DEBUG
            printf ("Thread ID =%d\n", (current_thread->thread_id));
            printf( " i = %d   i%%3 = %d\n", i, (i%3));
        #endif
        if (i%3 ==1)
            yield();
    }
    #ifdef DEBUG
        printf( " thread with id = %d finished\n", current_thread->thread_id);
    #endif
}

main()
{
    int stack_size = 1024*1024;
    int i;
    // allocate space for thread going to become current thread
    current_thread = malloc(sizeof(struct thread_t));  
    
    // initializing arguments of current_thread
    current_thread->initial_function = fun_with_threads; 
    int * p = malloc(sizeof(int));
    *p = 5;
    current_thread->initial_argument = p;    
    current_thread->stack_pointer = malloc(stack_size) + stack_size;

    #ifdef DEBUG
        current_thread->thread_id =1;
    #endif

    // allocate space for the thread going to be inactive
    // no need to allocate space for it's stack because it is already running and has a stack
    // no initial function or argument initialized because it already is running  
    inactive_thread = malloc(sizeof(struct thread_t));

    #ifdef DEBUG
        inactive_thread->thread_id = 0;
    #endif

    // call to start current_thread
    thread_start(inactive_thread, current_thread);

    for (i =0; i<13;i++)
       if ( i%4 ==1)                
       {
           #ifdef DEBUG
               printf(" yielding in main i =%d\n", i);
           #endif
           yield();
       }
       #ifdef DEBUG
           printf( " thread with id = %d finished\n", current_thread->thread_id);
       #endif

}

// outside the context of setup and switching code, current_thread is the one that is running
void yield() {
    #ifdef DEBUG 
        printf("in yield\n");
        // to check which thread is running
        printf ("current thread ID =%d\n", (current_thread->thread_id));
     #endif 

    struct thread_t *temp = current_thread;
    current_thread = inactive_thread;
    inactive_thread = temp;

    #ifdef DEBUG 
        printf ("switching to thread_ID =%d\n", (current_thread->thread_id));
        printf( " calling switch \n");   
    #endif

    thread_switch(inactive_thread, current_thread);
} 
