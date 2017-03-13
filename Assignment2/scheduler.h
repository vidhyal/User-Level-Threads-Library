

typedef enum {
    RUNNING, // The thread is currently running.
    READY,   // The thread is not running, but is runnable.
    BLOCKED, // The thread is not running, and not runnable.
    DONE     // The thread has finished. 
} state_t;

struct thread
{
    unsigned char* stack_pointer;
    void (*initial_function)(void*);
    void* initial_argument;
    state_t state;
    #ifdef DEBUG
        int thread_id;
    #endif
};

void thread_start(struct thread * old, struct thread *new);
void thread_switch(struct thread * old, struct thread * new);
void scheduler_begin();
void thread_fork(void(*target)(void*), void * arg);
void yield();
void scheduler_end();

extern struct thread * current_thread, * inactive_thread;



