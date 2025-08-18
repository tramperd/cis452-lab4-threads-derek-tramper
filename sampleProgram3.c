/**********************************************************
 *
 * sampleProgram3.c
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

// global (shared and specific) data
int shared_data = 5;
char sample_array[2] = {'a', 'b'};

int main() {
   pthread_t thread1, thread2;
   void *result1, *result2;
   int thread_status1, thread_status2;
   int join_status1, join_status2;

   thread_status1 = pthread_create(&thread1, NULL, do_greeting, &sample_array[0]);
   if (thread_status1 != 0) {
      fprintf(stderr, "Thread create error %d: %s\n", thread_status1, strerror(thread_status1));
      exit(1);
   }

   thread_status2 = pthread_create(&thread2, NULL, do_greeting, &sample_array[1]);
   if (thread_status2 != 0) {
      fprintf(stderr, "thread create error %d: %s\n", thread_status2, strerror(thread_status2));
      exit(1);
   }

   printf("Parent sees %d\n", shared_data);
   shared_data++;

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
   printf("Parent sees %d\n", shared_data);

   return 0;
}

void *do_greeting(void *my_argument) {
   char *my_ptr = (char *)my_argument;

   printf("Child receiving %c initially sees %d\n", *my_ptr, shared_data);
   sleep(1);
   shared_data++;
   printf("Child receiving %c now sees %d\n", *my_ptr, shared_data);
   return NULL;
}
