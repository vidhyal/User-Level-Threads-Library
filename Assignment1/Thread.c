#include "Thread.h"
#include <stdio.h>

extern struct thread_t * current_thread;

void thread_wrap() {
    printf ("in thread wrap\n");
    current_thread->initial_function(current_thread->initial_argument);
    for(;;)                       // infinite loop to fix segmentation fault
        yield();
  }


