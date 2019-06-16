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

struct ThreadDetails
{
    int id;
    int min;
    int max;
};

// threaded function
void *run(void *args)
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

    // cast type back to ThreadDetails to access variables
    struct ThreadDetails *details = (struct ThreadDetails *)args;

    // init local min/max
    details->min = arr[start];
    details->max = arr[start];

    // find local min/max
    for (int i = start; i < end; ++i)
    {
        if (arr[i] < details->min)
        {
            details->min = arr[i];
        }

        if (arr[i] > details->max)
        {
            details->max = arr[i];
        }
    }

    // critical section to update global max/min
    pthread_mutex_lock( &lockB );
    if (details->min < smallest)
    {
        smallest = details->min;
    }
    if (details->max > largest)
    {
        largest = details->max;
    }
    pthread_mutex_unlock( &lockB );
    // end critical section

    pthread_exit(NULL);
}

// fill array with random numbers using rand_r
// pid is the seed
void fill_arr()
{
    for (int i = 0; i < ARR_LEN; ++i)
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

    // start program clock after user input
    clock_t start_time = clock();

    // just a space for readability
    printf("\n");

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

    // declare a corresponding array of thread details
    struct ThreadDetails thread_details[num_threads];

    // init locks
    pthread_mutex_init(&lockA, NULL);
    pthread_mutex_init(&lockB, NULL);

    // populate the array    
    fill_arr();

    // init global min/max values
    smallest = arr[0];
    largest = arr[0];

    // start search clock
    clock_t start_search = clock();

    // start the threads
    for (int i = 0; i < num_threads; ++i)
    {
        thread_details[i].id = i;
        pthread_create(&threads[i], NULL, run, &thread_details[i]);
    }

    // wait for all threads to complete
    for (int i = 0; i < num_threads; ++i)
    {
        pthread_join(threads[i], NULL);
    }

    // stop search clock
    clock_t end_search = clock();
    double search_time = ((double)(end_search - start_search)) / CLOCKS_PER_SEC;

    // print out local details, global details, and elapsed times
    for (int i = 0; i < num_threads; ++i)
    {
        printf("Thread %d local min: %d\n", thread_details[i].id, thread_details[i].min);
        printf("Thread %d local max: %d\n", thread_details[i].id, thread_details[i].max);
    }

    printf("\nGlobal min: %d\n", smallest);
    printf("Global max: %d\n\n", largest);

    // stop program clock
    clock_t end_time = clock();
    double time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    
    printf("Search time: %f seconds\n", search_time);
    printf("Program time: %f seconds\n", time_used);

}
	
