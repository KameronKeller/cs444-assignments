#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "eventbuf.h"

struct eventbuf *eb;
sem_t *mutex;
sem_t *fill;
sem_t *empty;

int num_events;

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

void validate_input(int argc) {
    if (argc != 5) {
        fprintf(stderr, "usage: pcseml num_producers num_consumers num_events num_oustanding\n");
        exit(1);
    }
}

int convert_to_int(char *input)
{
    return (int)strtol(input, NULL, 10);
}


void *produce(void *arg)
{
    int *producer_number = arg;

    for (int i = 0; i < num_events; i++) {
        int event_number = *producer_number * 100 + i;
        sem_wait(empty); // spaces.wait()
        sem_wait(mutex); // mutex.wait()
        printf("P%d: adding event %d\n", *producer_number, event_number);
        eventbuf_add(eb, event_number);
        sem_post(mutex); // mutex.signal()
        sem_post(fill); // items.signal()
    }
    printf("P%d: exiting\n", *producer_number);


    return NULL;
}

void *consume(void *arg)
{
    int *consumer_number = arg;
    int event;

    while(1) {
        sem_wait(fill); //items.wait()
        sem_wait(mutex); //mutex.wait()
        if (eventbuf_empty(eb)) {
            sem_post(mutex);
            printf("C%d: exiting\n", *consumer_number);
            break;
        } else {
            event = eventbuf_get(eb); // event = buffer.get()
            sem_post(mutex);
            printf("C%d: got event %d\n", *consumer_number, event);
            sem_post(empty);
        }

    }

    return NULL;
}


int main(int argc, char *argv[])
{
    validate_input(argc);

    int num_producers = convert_to_int(argv[1]);
    int num_consumers = convert_to_int(argv[2]);
    num_events = convert_to_int(argv[3]);
    int num_outstanding = convert_to_int(argv[4]);

    eb = eventbuf_create();

    // Allocate thread handle array for all producers
    pthread_t *producer_thread = calloc(num_producers, sizeof *producer_thread);

    // Allocate thread ID array for all producers
    int *producer_thread_id = calloc(num_producers, sizeof *producer_thread_id);

    // Allocate thread handle array for all consumers
    pthread_t *consumer_thread = calloc(num_consumers, sizeof *consumer_thread);

    // Allocate thread ID array for all consumers
    int *consumer_thread_id = calloc(num_consumers, sizeof *consumer_thread_id);

    mutex = sem_open_temp("mutex", 1);
    fill = sem_open_temp("fill", 0);
    empty = sem_open_temp("empty", num_outstanding);

    for (int i = 0; i < num_producers; i++) {
        producer_thread_id[i] = i;
        pthread_create(producer_thread + i, NULL, produce, producer_thread_id + i);
    }

    for (int i = 0; i < num_consumers; i++) {
        consumer_thread_id[i] = i;
        pthread_create(consumer_thread + i, NULL, consume, consumer_thread_id + i);
    }
    


    for (int i = 0; i < num_producers; i++)
        pthread_join(producer_thread[i], NULL);

    for (int i = 0; i < num_consumers; i++) {
        sem_post(fill);
    }

    for (int i = 0; i < num_consumers; i++) {
        // sem_post(fill);
        pthread_join(consumer_thread[i], NULL);
    }

    return 0;
}
