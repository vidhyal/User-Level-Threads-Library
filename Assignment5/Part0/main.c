#include "scheduler.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>


// DEBUG1 is used to debug through synchronous and asynchronous call
// DEBUG is used to view thread switching
void print_nth_prime(void * pn) {

  int n = *(int *) pn;
  printf("n =%d\n",n);
  int c = 1, i = 1;
  while(c <= n) {
    ++i;
    int j, isprime = 1;
    for(j = 2; j < i; ++j) {
      if(i % j == 0) {
        isprime = 0;
        break;
      }
    }
    if(isprime) {
      ++c;
    }
    yield();
  }
  printf("%dth prime: %d\n", n, i);
 
}

// function to call synchronous read
void read1(void * fd){
  char buf[5000];
  #ifdef DEBUG1
    printf ("-----------------------------reading synchronously-----------------------------\n");
  #endif
  size_t count = 5000;
  read(*(int*)fd, buf, count);
  #ifdef DEBUG1
    printf("-----------------------------printing synchronous-----------------------------\n");
  #endif
  printf("%s\n", (char*)buf);
}

// function to call wrapper function for asynchronous read
void read2(void * fd){
  char buf[5000];
  #ifdef DEBUG1
    printf ("-----------------------------reading asynchronously-----------------------------\n");
  #endif 
  size_t count = 5000;
  read_wrap(*(int*)fd, buf, 5000);
  #ifdef DEBUG1
    printf("-----------------------------printing asynchronous-------------------------------\n");
  #endif
  printf("%s\n", (char*)buf);
}

int main(void) {
  scheduler_begin();
  int n1 = 200, n2 = 10000, n3 = 300, i;
  int fno1 = STDIN_FILENO;
  int fno2= open("inputFile",O_RDONLY);  

  thread_fork(print_nth_prime, &n1);
  thread_fork(print_nth_prime, &n2);

  thread_fork(read2,&fno1);    // asynchronous read
  thread_fork(read2,&fno2);    // asynchronous read
for(i=0; i<500; i++)
yield();
  thread_fork(read2,&fno2);    // asynchronous read
  thread_fork(read1,&fno1);    // synchronous read
  thread_fork(read1,&fno2);    // synchronous read
  thread_fork(read1,&fno2);    // synchronous read


  thread_fork(print_nth_prime, &n3);
  scheduler_end();

}