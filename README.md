# Threads Practice
The program uses threads to search a large array of random integers for the largest and smallest numbers.

The user inputs the number of desired threads. 
The program outputs the largest and smallest numbers found in the array and the total time taken to search.

## Threads and Locks
To prevent race conditions between threads, each thread contains two critical sections:

* one for the thread to select its own segment of the array,
* another for the thread to update the global maximum and minimum values found.

Each critical section utilizes a mutex lock. When a thread enters a critical section, 
the section becomes locked for other threads until the current thread exits.

## How To Compile
To run, the program must first be compiled. On Linux, use the following command:
```
gcc program1.c -o program1 -pthread
```
On Windows, you may need to install [Cygwin](https://www.cygwin.com/) to compile the source code.

## Author
Samy Masadi
