#include <atomic_ops.h>
#define current_thread (get_current_thread())

typedef enum {
    RUNNING, // The thread is currently running.
    READY,   // The thread is not running, but is runnable.
    BLOCKED, // The thread is not running, and not runnable.
    DONE     // The thread has finished. 
} state_t;

struct mutex {
    int held;
    struct queue *waiting_threads;
    struct thread *heldBy;
};
void mutex_init( struct mutex*);
void mutex_lock(struct mutex *);
void mutex_unlock (struct mutex*);

struct condition {
    struct queue *waiting_threads;
};
void condition_init( struct condition *);
void condition_wait(struct condition *, struct mutex*);
void condition_signal(struct condition*);
void condition_broadcast (struct condition*);


int kernel_thread_begin(void *);
struct thread
{
    unsigned char* stack_pointer;
    void (*initial_function)(void*);
    void* initial_argument;
    state_t state;
    #ifdef DEBUG
        int thread_id;
    #endif
    struct condition cond;
    struct mutex mutex1;
};

void thread_start(struct thread * old, struct thread *new);
void thread_switch(struct thread * old, struct thread * new);
void scheduler_begin();
struct thread * thread_fork(void(*target)(void*), void * arg);
void thread_join( struct thread*);
void yield();
void scheduler_end();
extern struct thread * current_thread, * inactive_thread;

extern void * safe_mem(int, void*);

#define malloc(arg) safe_mem(0,((void*) (arg)))
#define free(arg) safe_mem(1,arg)
