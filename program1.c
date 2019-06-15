/*	Samy Masadi
	CSCI 435
	Program 1

	The program uses threads to search a large array of
	random integers for the largest and smallest numbers.
	
	The user inputs the number of desired threads.
	The program outputs the largest and smallest
	numbers found in the array and the total time
	taken to search.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <time.h>

#define ARR_LEN 100000000

// to contain random ints
int arr[ARR_LEN];
// global min/max
int smallest;
int largest;
// segment length for each thread
int seg_length;
// for thread array segment bound
int bound = 0;
// seed for random number generator
unsigned int randSeed;
// global lock variables
pthread_mutex_t lockA;
pthread_mutex_t lockB;


// threaded function
void* run(void* args)
{
    // critical section for thread to select its array segment bounds
    pthread_mutex_lock( &lockA );
    int start = bound;
    bound = bound + seg_length;
    int end;
    if ((ARR_LEN - bound) >= seg_length)
    {
        end = bound;
    }
	// in case threads don't divide evenly into array,
    // last thread takes segment plus remainder
    else
    {
        end = ARR_LEN;
    }
    pthread_mutex_unlock( &lockA );
    // end critical section

    // init local min/max
    int smallestL = arr[start];
    int largestL = arr[start];

    // find local min/max
    for (int i = start; i < end; i++)
    {
        if (arr[i] < smallestL)
        {
            smallestL = arr[i];
        }

        if (arr[i] > largestL)
        {
            largestL = arr[i];
        }
    }

    printf("Thread local max: %d\n", largestL);
    printf("Thread local min: %d\n", smallestL);

    // critical section to update global max/min
    pthread_mutex_lock( &lockB );
    if (smallestL < smallest)
    {
        smallest = smallestL;
    }
    if (largestL > largest)
    {
        largest = largestL;
    }
    pthread_mutex_unlock( &lockB );
    // end critical section

    pthread_exit(NULL);
}

// fill array with random numbers using rand_r
// pid is the seed
void fill_arr()
{
    for (int i = 0; i < ARR_LEN; i++)
    {
        arr[i] = rand_r(&randSeed);
    }
}

int main()
{
    // seed is pid
    randSeed = getpid();

    // user can decide number of threads
    int num_threads;
    printf("Enter number of threads: ");
    num_threads = getchar();

    // start clock after user input
    clock_t start_time = clock();

    // determine intended int value
    num_threads = num_threads - '0';

    // some simple limits on input
    if (num_threads < 1)
    {
        num_threads = 1;
    }
    if (num_threads > 16)
    {
        num_threads = 16;
    }

    // init segment length size
    seg_length = ARR_LEN / num_threads;

    // declare an array of threads
    pthread_t threads[num_threads];
    // init locks
    pthread_mutex_init( &lockA, NULL);
    pthread_mutex_init( &lockB, NULL);

    // populate the array    
    fill_arr();

    // init global min/max values
    smallest = arr[0];
    largest = arr[0];

    // start the threads
    for (int i = 0; i < num_threads; i++)
    {
        pthread_create(&threads[i], NULL, run, NULL);
    }

    // wait for all threads to complete
    for (int i = 0; i < num_threads; i++)
    {
        pthread_join(threads[i], NULL);
    }

    printf("Global min: %d\n", smallest);
    printf("Global max: %d\n", largest);

    // stop clock
    clock_t end_time = clock();
    double time_used = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("Time taken: %f seconds.\n", time_used);

}
	
