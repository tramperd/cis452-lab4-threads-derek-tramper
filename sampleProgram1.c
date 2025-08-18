/**********************************************************
 *
 * sampleProgram1.c
 * CIS 451 Lab 4
 *
 * !!!NAMES!!!
 *************************************************************/

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void *do_greeting(void *arg);
// arguments:    arg is an untyped pointer
// returns:      a pointer to whatever was passed in to arg
// side effects: prints a greeting message

int main() {
   pthread_t thread1; // thread ID holder
   int status;        // captures any error code

   // create and start a thread executing the "do_greeting()" function
   if ((status = pthread_create(&thread1, NULL, do_greeting, NULL)) != 0) {
      fprintf(stderr, "thread create error %d: %s\n", status, strerror(status));
      exit(1);
   }

   return 0;
}

void *do_greeting(void *arg) {
   sleep(1);
   printf("Thread version of Hello, world.\n");
   return arg;
}