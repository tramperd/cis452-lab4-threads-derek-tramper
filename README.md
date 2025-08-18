 CIS 452 Lab 4:  Multi-threaded Programming:  pthreads
------------------------------------------------------------------------

Lab originally written by Prof. Greg Wolffe.

### Overview

The purpose of this lab is to introduce multi-threaded programming:
the fundamental mechanisms for thread creation, execution, management,
communication and termination.
It examines the first essential element of any thread system:
execution context (the remaining two elements being scheduling and synchronization).

The documentation refers to the POSIX **pthreads** API,
a standardized run-time library that natively implements threads on Linux.
See the online documentation (i.e. `man` pages) for additional details on the
syntax and operation of the library calls described in this lab.


### Thread Creation and Execution

A traditional process in a UNIX system is simply a single-threaded program.
It has a single thread (or path) of execution, known as the initial thread,
that begins by executing `main()`.

Like processes, all threads
(with the exception of the initial one executing `main()`)
are spawned from an existing thread.
The syntax of the library routine that creates a thread is given below:

```c
int pthread_create (pthread_t* tid               // thread id (returned from create routine)
                    const pthread_attr_t* attr,  // optional attributes
                    void* (*start)(void*),       // address of function to execute
                    void* arg);                  // argument passed to thread
```

This call creates a new thread running the user-defined function named `start`.
The `start()` function is passed a single argument `arg`, of type `void*`,
and returns a value of the same type. 
Any optional attributes (e.g. scheduling policy) may be set at creation time.
The identifier of the newly created thread is returned in `tid`.
After completion of this function call,
the newly created thread will begin executing the function `start()` using the
argument(s) pointed to by `arg`.
As usual, the return value of the function call is a status indicator;
a non-zero value means the function failed.

The sample program below demonstrates simple thread creation.
Default values are used for the attributes.
The function does not take any arguments.
Note the use of error-checking for all function calls.

*Sample Program 1*

```c
#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void* do_greeting (void* arg);
// arguments:    arg is an untyped pointer
// returns:      a pointer to whatever was passed in to arg
// side effects: prints a greeting message

int main()
{
    pthread_t thread1;  // thread ID holder
    int status;         // captures any error code

    // create and start a thread executing the "do_greeting()" function
    if ((status = pthread_create (&thread1, NULL,  do_greeting, NULL)) != 0) {
        fprintf (stderr, "thread create error %d: %s\n", status, strerror(status));
        exit (1);
    }

    return 0;
}

void* do_greeting (void* arg)
{
    sleep(1);
    printf ("Thread version of Hello, world.\n");
    return arg;
}
```

**Try the following operations and answer the numbered questions:**

* Compile and run Sample Program 1. You will need to link in the pthreads library
  (i.e. you must compile with the `-pthread` compiler option).
* Observe the results.
* Try inserting a 2-second `sleep()` into the `main()` function
  (after thread creation); compile and re-run.

1. describe/explain your observations,
   i.e. what must have happened in the original, unmodified program?

### Thread Suspension and Termination

Similar to UNIX processes, threads have the equivalent of the `wait()` and `exit()` system calls ---
in this case called `join()` and `exit()`.
The calls are used to block threads and terminate threads, respectively.

To instruct a thread to block while waiting for another thread to complete,
use the `pthread_join()` function.
This function is also the mechanism used to get a return value from a thread.
Note that any thread can join on (and hence wait for) any other thread.
The function prototype is:

```c
int pthread_join (pthread_t thread1, void **value_ptr)
```

This function specifies that the calling thread should block and wait for
`thread1` to complete execution.
The value returned by `thread1` will be accessible via the argument
`value_ptr`.
In addition to  explicitly joining, threads may also use
semaphores, conditional waits and other synchronization mechanisms
to keep track of when other threads exit.

Sometimes parent threads have ongoing work to perform --
for example, functioning as a dispatcher.
Instead of waiting for a child thread to complete,
a parent can specify that it does not require a return value or any explicit
synchronization with a child thread.
To do this, the parent thread uses the `pthread_detach()` function.
A child thread can also specify that it does not need to join with any other
threads by detaching itself.
After the call to detach, there is no thread waiting for the child -
it executes independently until termination.
The prototype for this function is as follows:

```c
int pthread_detach (pthread_t thread1)
```

with `thread1` representing the identity of the detached thread.

The `pthread_exit()` function causes the calling thread to terminate.
Resources are recovered, and a value is returned to the joined thread (if one exists).
A thread may explicitly call `pthread_exit()`, or it may simply terminate,
usually by returning from its start function.
Although it is not strictly enforced that you use either `join()` or
`detach()`, it is good practice because non-detached threads which have exited but have
not been joined are equivalent to zombie processes
(i.e. their resources cannot be fully recovered).
Okay, now read that last sentence again, and make a habit of implementing it.

The sample program below is a functioning multi-threaded program that uses the
library functions described above.
It may not behave exactly as you might expect.

*Sample Program 2*

```c
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void *doGreeting(void *myArgument);

int thread_counter = 0;

int main() {
   pthread_t thread1, thread2;         // thread ID's
   void *result1, *result2;            // return values
   int thread_status1, thread_status2; // store the result of the thread creation
   int join_status1, join_status2;     // store the result of the thread join

   thread_status1 = pthread_create(&thread1, NULL, doGreeting, "hi");
   if (thread_status1 != 0) {
      fprintf(stderr, "Thread create error %d: %s\n", thread_status1, strerror(thread_status1));
      exit(1);
   }

   thread_status2 = pthread_create(&thread2, NULL, doGreeting, "bye");
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
void *doGreeting(void *myArgument) {
   printf("String passed = %s\n", (char *)myArgument);
   thread_counter = thread_counter + 1;
   printf("ThreadCounter = %d\n", thread_counter);

   // Print out a message based on the argument passed to us
   for (int loop = 0; loop < 10; loop++) {
      if (strcmp((char *)myArgument, "hi") == 0) {
         printf("Hello\n");
      } else {
         printf("Good Bye\n");
      }
   }

   if (strcmp((char *)myArgument, "hi") == 0) {
      myArgument = "Hello is done";
   } else {
      myArgument = "Good Bye is done";
   }
   return myArgument;
}
```

**Try the following operations and answer the numbered questions:**

* Compile and run Sample Program 2.

2. What does `sampleProgram2` output? If you run it a repeated number of times does the output vary? Why?

* insert a one-second `sleep()` at the beginning of the loop in the `do_greeting2()` function.
  Compile and run the modified program.

3. report your results again. Explain why they are different from the results seen in question 2.


### Thread Communication

There are two methods used by threaded programs to communicate.
The first method uses shared memory (as described in class).
But sometimes it is desirable to communicate thread-specific information to
each individual thread.
For this purpose, we can use the second method of communication:
via the argument value (the `void *`) passed to a thread's start function.
The following sample program uses shared memory for communication;
it also demonstrates the mechanism of thread-specific arguments to pass each
thread unique information.

*Sample Program 3*

```c
#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void* do_greeting3 (void* arg);
// arguments :   arg is an untyped pointer pointing to a character
// returns :     a pointer to NULL
// side effects: prints a greeting

// global (shared and specific) data
int sharedData = 5;
char val[2] = {'a','b'};

int main()
{
    pthread_t thread1, thread2;
    void *result1, *result2;
    int status;

    // create and start two threads executing the "do_greeting3" function
    // pass each thread a pointer to its respective argument
    if ((status = pthread_create (&thread1, NULL,  do_greeting3, &val[0])) != 0) {
        fprintf (stderr, "thread create error %d: %s\n", status, strerror(status));
        exit (1);
    }
    if ((status = pthread_create (&thread2, NULL,  do_greeting3, &val[1])) != 0) {
        fprintf (stderr, "thread create error %d: %s\n", status, strerror(status));
        exit (1);
    }
    printf ("Parent sees %d\n", sharedData);
    sharedData++;

    // join with the threads (wait for them to terminate);  get their return vals
    if ((status = pthread_join (thread1, &result1)) != 0) {
        fprintf (stderr, "join error %d: %s\n", status, strerror(status));
        exit (1);
    }
    if ((status = pthread_join (thread2, &result2)) != 0) {
        fprintf (stderr, "join error %d: %s\n", status, strerror(status));
        exit (1);
    }
    printf ("Parent sees %d\n", sharedData);
    return 0;
}

void* do_greeting3 (void* arg)
{
    // note the cast of the void pointer to the desired data type
    char *val_ptr = (char *) arg;

    // print out a message
    printf ("Child receiving %c initially sees %d\n", *val_ptr, sharedData);
    sleep(1);
    sharedData++;
    printf ("Child receiving %c now sees %d\n", *val_ptr, sharedData);
    return NULL;
}
```

**Try the following operations and answer the numbered questions:**

5. compile the sample program and run it multiple times
   (you may see some variation between runs).
   Choose one particular sample run.
   Describe, trace, and explain the output of the program.

6. explain in your own words how the thread-specific (not shared) data is
   communicated to the child threads.

### Lab Programming Assignment (Blocking Multi-threaded Server)

Complete the programming assignment for the lab.

* Be prepared to demo the Programming Assignment next week and to answer
  questions about its operation.

* I will check through the code for good structure and style.

* Your code should begin with a comment containing, at a minimum,
  the names of both partners, the name of the lab, and a few-sentence
  description of what the program is meant to do.

* Your code should check for error conditions in system calls and calls to
  standard library routines.
  You have seen simple examples of how to look for and display errors,
  but you should look at the `man` pages to be sure you understand how this
  works for different routines.

### Lab Programming Assignment (Blocking Multi-threaded Server)

Recall the class discussion of a multi-threaded fileserver,
in which a Dispatch thread receives a file access request from a client,
dispatches a Worker thread to satisfy the request,
and resumes accepting new requests from other clients.
The worker threads proceed to service their assigned request
(potentially blocking while waiting for the disk).
This mini-programming assignment simulates the thread execution manifested by a
multi-threaded fileserver process.

Develop a multi-threaded program with the following specifications:

**Dispatch thread:**

* Input a string from the user
  (simulating the name of a file to access).
* Spawn a child thread and communicate to it the filename requested/entered by
  the user.
* *Immediately* repeat the input/spawn sequence
  (i.e. accept a new file request).

**Worker threads:**

* Obtain the simulated filename from the dispatcher.
* Sleep for a certain amount of time,
  simulating the time spent performing a file access:
    * With 80% probability, sleep for 1 second.
      This simulates the scenario that the Worker thread has found the desired
      file in the disk cache and serves it up quickly.
    * With 20% probability, sleep for 7-10 seconds (randomly).
      This simulates the scenario that the worker thread has *not* found the
      requested file in the disk cache and must block while it is read in from
      the hard drive.
* Wake up, print a diagnostic message that includes the name of the file
  accessed, then terminate.
* It's ok if the resulting display looks somewhat "messy";
  that shows that true concurrency is occurring.
* Your program should continue to execute until terminated by the user (`^C`).
  At that point, your program should print out the total number of file requests
  received.

When terminated, your program should cleanup as appropriate and shutdown
gracefully.

In addition to being correct, your program should be efficient and
should execute in parallel.

**Remember**: Threads share data by definition
-- as a rule, all multi-threaded programs should be carefully
scrutinized for potential race conditions or "data clobbering".

Note: the threads in this assignment mostly sleep (simulating blocking I/O).
However, you may at some point write a more computationally intense program and
want to "watch" your threads executing.
The EOS environment provides a graphical tool called
`gnome-system-monitor`.
Make sure the "Resources" tab is selected.
The top graph displays CPU activity on all CPU cores
(Linux sees Intel hyperthreading as two cores,
so a quad-core system reports 8 CPUs).
If your threads are sufficiently busy and correctly concurrent,
you should see them occupying and executing on multiple cores.

