#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void *print_numbers(void *thread_name) {
	for (int i = 0; i < 5; i++) {
		printf("%s: %d\n", (char *)thread_name, i);
	}
	return NULL;
}

int main() {
	printf("Launching threads\n");
	pthread_t t1;
	pthread_t t2;

	pthread_create(&t1, NULL, print_numbers, "thread 1");
	sleep(0);
	pthread_create(&t2, NULL, print_numbers, "thread 2");

    (void) pthread_join(t1, NULL);
    (void) pthread_join(t2, NULL);
	
	printf("Threads complete!\n");
	return 0;
}
