#include "scheduler.h"
#include <stdio.h>


/* part of code commented out was used for testing purposes*/
void print_nth_prime(void * pn) {

  int n = *(int *) pn;
  printf("n =%d\n",n);
 // scanf("%d", &n);
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

  }
  printf("%dth prime: %d\n", n, i);
 
}


int main(void) {
  scheduler_begin();
  //int n1 = 200, n2 = 100, n3 = 300;
  //int i;
  int n1 = 20000, n2 = 10000, n3 = 30000;
  thread_fork(print_nth_prime, &n1);
  /*for ( i =0; i<1220;i++)
      yield();*/
  //scheduler_end();
  //scanf("%d\n", &n2);
  thread_fork(print_nth_prime, &n2);
  thread_fork(print_nth_prime, &n3);
  scheduler_end();

}