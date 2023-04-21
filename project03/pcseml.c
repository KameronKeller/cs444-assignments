#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include "eventbuf.h"

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


int main(int argc, char *argv[])
{
    validate_input(argc);

    int num_producers = convert_to_int(argv[1]);
    int num_consumers = convert_to_int(argv[2]);
    int num_events = convert_to_int(argv[3]);
    int num_outstanding = convert_to_int(argv[4]);

    printf("%d\n", num_producers);
    printf("%d\n", num_consumers);
    printf("%d\n", num_events);
    printf("%d\n", num_outstanding);

    struct eventbuf *eb;
    eb = eventbuf_create();
    
    eventbuf_add(eb, 12);
    eventbuf_add(eb, 37);
    printf("%d\n", eventbuf_get(eb));    // 12
    printf("%d\n", eventbuf_get(eb));    // 37
    printf("%d\n", eventbuf_empty(eb));  // 1
    eventbuf_free(eb);

    return 0;
}
