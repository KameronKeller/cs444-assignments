simfs_test: simfs_test.c simfs.a
	gcc -Wall -Wextra -DCTEST_ENABLE -o $@ $^
	@./simfs_test

simfs.a: image.o
	ar rcs $@ $^

image.o: image.c
	gcc -Wall -Wextra -c $<

.PHONY: test

test: simfs_test
	./simfs_test

clean:
	rm -f simfs_test
