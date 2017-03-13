
#include <aio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "scheduler.h"

// this function is a wrapper for asynchronous read function that follows the semantics as 
// a synchronous read.

ssize_t read_wrap (int fd, void *buf, size_t count) {
	int return_val;
    // allocating memory to initializing an asynchronous I/O block structure with zero   
    struct aiocb *aIcB = calloc (1,sizeof(struct aiocb));
    // initializing all potentially non-zero values of aIcB structure as required by implementation
    aIcB->aio_nbytes = count;
    aIcB->aio_buf = buf;
    aIcB->aio_fildes = fd;
    // no need for notification on read complete, because polling.
    aIcB->aio_sigevent.sigev_notify = SIGEV_NONE;

    //set the seeking offset because aio_read cannot do this on its own
   int offset;
    offset=lseek(fd, 0, SEEK_CUR);
    if (offset !=-1)
      aIcB->aio_offset = offset;
    #ifdef DEBUG1
      if(offset == -1)
        printf ("file not seekable\n");
      else 
        printf("offset = %d\n", offset);
     #endif

    // call aio_read and check for errors
    int readRet = aio_read(aIcB);
    if(readRet == -1)
      perror("aio_read");
    // if read operation has been succefully queued, check its progress
    if(readRet == 0){
      // yield when read is still being performed, polling
      while (aio_error(aIcB) == EINPROGRESS)
        yield();                                
      // print errors in other cases
      if (aio_error(aIcB) == ECANCELED)
        perror("aio_read");
      if (aio_error(aIcB)>0){
        printf ("%d\n", aio_error(aIcB));
        perror("aio_read");
      }
      // if read successful, return results
      if(aio_error(aIcB) !=0){
        printf(" error in reading: %s\n", strerror(errno));
      }
    }
    return_val = aio_return(aIcB);
    if (return_val == -1)
       printf("error on return from reading: %s\n", strerror(errno));
    offset=lseek(fd, 0, SEEK_CUR);
    if (offset !=-1)
      aIcB->aio_offset = offset;
    return return_val;
}