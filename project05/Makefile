simfs_test: simfs_test.c simfs.a
	gcc -Wall -Wextra -o $@ $^

simfs.a: image.o
	ar rcs $@ $^

image.o: image.c
	gcc -Wall -Wextra -c $<

.PHONY: test

test: simfs_test
	./simfs_test
