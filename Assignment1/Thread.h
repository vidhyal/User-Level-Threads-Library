struct thread_t
{
    unsigned char* stack_pointer;   // pointer to single byte 
    void (*initial_function)(void*);  // pointer to functions taking a single void parameter and no return value
    void* initial_argument;           // pointer to value of any type: freedom to choose initial argument
    #ifdef DEBUG
        int thread_id;                  // to check switching
    #endif
};

void thread_start(struct thread_t * old, struct thread_t *new);
void thread_switch(struct thread_t * old, struct thread_t * new);
