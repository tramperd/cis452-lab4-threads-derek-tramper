/**********************************************************
 *
 * sampleProgram2.c
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

void *do_greeting(void *myArgument);

int thread_counter = 0;

int main() {
   pthread_t thread1, thread2;         // thread ID's
   void *result1, *result2;            // return values
   int thread_status1, thread_status2; // store the result of the thread creation
   int join_status1, join_status2;     // store the result of the thread join

   thread_status1 = pthread_create(&thread1, NULL, do_greeting, "hi");
   if (thread_status1 != 0) {
      fprintf(stderr, "Thread create error %d: %s\n", thread_status1, strerror(thread_status1));
      exit(1);
   }

   thread_status2 = pthread_create(&thread2, NULL, do_greeting, "bye");
   if (thread_status2 != 0) {
      fprintf(stderr, "Thread create error %d: %s\n", thread_status2, strerror(thread_status2));
      exit(1);
   }

   // join with the threads (wait for them to terminate);  get their return vals
   join_status1 = pthread_join(thread1, &result1);
   if (join_status1 != 0) {
      fprintf(stderr, "Join error %d: %s\n", join_status1, strerror(join_status1));
      exit(1);
   }

   join_status2 = pthread_join(thread2, &result2);
   if (join_status2 != 0) {
      fprintf(stderr, "Join error %d: %s\n", join_status2, strerror(join_status2));
      exit(1);
   }

   printf("Thread one returned: [%s]\n", (char *)result1);
   printf("Thread two returned: [%s]\n", (char *)result2);

   return 0;
}

// function called by the threads, will print out a different message based on the argument passed
// to it
void *do_greeting(void *my_argument) {
   printf("String passed = %s\n", (char *)my_argument);
   thread_counter = thread_counter + 1;
   printf("ThreadCounter = %d\n", thread_counter);

   // Print out a message based on the argument passed to us
   for (int loop = 0; loop < 10; loop++) {
      if (strcmp((char *)my_argument, "hi") == 0) {
         printf("Hello\n");
      } else {
         printf("Good Bye\n");
      }
   }

   if (strcmp((char *)my_argument, "hi") == 0) {
      my_argument = "Hello is done";
   } else {
      my_argument = "Good Bye is done";
   }
   return my_argument;
}
