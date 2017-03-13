#Inline comment

# void thread_switch(struct thread_t * old, thread_t * new)

.globl thread_switch

thread_switch:
   pushq %rbx
   pushq %rbp
   pushq %r12
   pushq %r13
   pushq %r14
   pushq %r15
   movq %rsp, (%rdi)
   movq (%rsi), %rsp
   popq %r15
   popq %r14
   popq %r13
   popq %r12
   popq %rbp
   popq %rbx  
   ret



# void thread_start(struct thread_t * old, struct thread_t * new)

.globl thread_start

thread_start:
   pushq %rbx
   pushq %rbp
   pushq %r12
   pushq %r13
   pushq %r14
   pushq %r15
   movq %rsp, (%rdi)
   movq (%rsi), %rsp
   jmp thread_wrap    #jumping to C function thread_wrap 
   
