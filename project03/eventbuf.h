#ifndef EVENTBUF_H
#define EVENTBUF_H

/*
 * Functions:
 *
 *   eventbuf_create() -- make a new eventbuf
 *   eventbuf_free()   -- free previously-created eventbuf
 *   eventbuf_add()    -- add an event to the eventbuf
 *   eventbuf_get()    -- remove an event to the eventbuf
 *   eventbuf_empty()  -- true if there are no items in the eventbuf
 *
 * Example Usage:
 *
 *   #include "eventbuf.h"  // Double quotes means "in this directory"
 *
 *   struct eventbuf *eb;
 *
 *   eb = eventbuf_create();
 * 
 *   eventbuf_add(eb, 12);
 *   eventbuf_add(eb, 37);
 *
 *   printf("%d\n", eventbuf_get(eb));    // 12
 *   printf("%d\n", eventbuf_get(eb));    // 37
 *
 *   printf("%d\n", eventbuf_empty(eb));  // 1
 *
 *   eventbuf_free(eb);
 *
 * Compilation instructions
 *
 *   In your Makefile, include this file on the gcc command line. For
 *   example, if your main program is called `foo.c`, you can build it
 *   all with:
 *
 *   gcc -Wall -Wextra -o foo foo.c eventbuf.c
 */

struct eventbuf {
    struct eventbuf_node *head, *tail;
};

struct eventbuf *eventbuf_create(void);
void eventbuf_free(struct eventbuf *eb);
int eventbuf_add(struct eventbuf *eb, int val);
int eventbuf_get(struct eventbuf *eb);
int eventbuf_empty(struct eventbuf *eb);

#endif
