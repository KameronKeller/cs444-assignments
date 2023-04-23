/*
 * Kameron Keller
 * OS 2 - Spring 2023
 * Project 3
 */

#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "eventbuf.h"

struct eventbuf *eb;
sem_t *mutex;
sem_t *fill;
sem_t *empty;

// Struct for passing values to produce function
struct producer_args {
    int *producer_number;
    int num_events;
};

// Semaphore creation function
sem_t *sem_open_temp(const char *name, int value)
{
    sem_t *sem;

    // Create the semaphore
    if ((sem = sem_open(name, O_CREAT, 0600, value)) == SEM_FAILED)
        return SEM_FAILED;

    // Unlink it so it will go away after this process exits
    if (sem_unlink(name) == -1) {
        sem_close(sem);
        return SEM_FAILED;
    }

    return sem;
}

// Validate correct number of args on the command line
void validate_input(int argc) {
    if (argc != 5) {
        fprintf(stderr, "usage: pcseml num_producers num_consumers num_events num_oustanding\n");
        exit(1);
    }
}

// Convert command line input to int
int convert_to_int(char *input)
{
    return (int)strtol(input, NULL, 10);
}

// Produce an event
void *produce(void *arg)
{
    // Unpack the values from the struct
    struct producer_args *p_args = (struct producer_args*) arg;
    int *producer_number = p_args->producer_number;
    int num_events = p_args->num_events;

    // Free the struct
    free(p_args);

    // Create the events
    for (int i = 0; i < num_events; i++) {
        int event_number = *producer_number * 100 + i;
        sem_wait(empty); // Wait for signal that there is room for an event
        sem_wait(mutex); // Grab the mutex
        printf("P%d: adding event %d\n", *producer_number, event_number);
        eventbuf_add(eb, event_number); // Add the event
        sem_post(mutex); // Release the mutex
        sem_post(fill); // Signal to consumers that there is an event to consume
    }

    printf("P%d: exiting\n", *producer_number);

    return NULL;
}

void *consume(void *arg)
{
    int *consumer_number = arg;
    int event;

    while(1) {
        sem_wait(fill); // Wait for signal from producers
        sem_wait(mutex); // Grab the mutex

        // If the buffer is empty, your work is done- exit.
        if (eventbuf_empty(eb)) {
            sem_post(mutex); // Release the mutex
            printf("C%d: exiting\n", *consumer_number);
            break;
        } else {
            event = eventbuf_get(eb); // Grab the event
            sem_post(mutex); // Release the mutex
            printf("C%d: got event %d\n", *consumer_number, event);
            sem_post(empty); // Signal to producers that there is room for another event
        }
    }
    return NULL;
}


int main(int argc, char *argv[])
{
    validate_input(argc);

    int num_producers = convert_to_int(argv[1]);
    int num_consumers = convert_to_int(argv[2]);
    int num_events = convert_to_int(argv[3]);
    int num_outstanding = convert_to_int(argv[4]);

    eb = eventbuf_create();

    // Initialize semaphores
    // Semaphore as a mutex
    mutex = sem_open_temp("mutex", 1);

    // Semaphore for notifying consumers to begin consuming
    fill = sem_open_temp("fill", 0);

    // Semaphore for notifying producers the event buffer has space
    empty = sem_open_temp("empty", num_outstanding);

    // Allocate thread handle array for all producers
    pthread_t *producer_thread = calloc(num_producers, sizeof *producer_thread);

    // Allocate thread ID array for all producers
    int *producer_thread_id = calloc(num_producers, sizeof *producer_thread_id);

    // Allocate thread handle array for all consumers
    pthread_t *consumer_thread = calloc(num_consumers, sizeof *consumer_thread);

    // Allocate thread ID array for all consumers
    int *consumer_thread_id = calloc(num_consumers, sizeof *consumer_thread_id);


    // Kick off producer threads
    for (int i = 0; i < num_producers; i++) {
        struct producer_args *p_args = malloc(sizeof p_args);
        p_args->producer_number = producer_thread_id + i;
        p_args->num_events = num_events;
        producer_thread_id[i] = i;
        // pthread_create(producer_thread + i, NULL, produce, producer_thread_id + i);
        pthread_create(producer_thread + i, NULL, produce, p_args);
    }

    // Kick off consumer threads
    for (int i = 0; i < num_consumers; i++) {
        consumer_thread_id[i] = i;
        pthread_create(consumer_thread + i, NULL, consume, consumer_thread_id + i);
    }

    // Join producer threads with the main thread
    for (int i = 0; i < num_producers; i++)
        pthread_join(producer_thread[i], NULL);

    // Signal to the consumers that the event buffer is empty 
    for (int i = 0; i < num_consumers; i++) {
        sem_post(fill);
    }

    // Join consumer threads with main thread
    for (int i = 0; i < num_consumers; i++) {
        pthread_join(consumer_thread[i], NULL);
    }

    return 0;
}
