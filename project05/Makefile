simfs_test: simfs_test.c simfs.a
	gcc -Wall -Wextra -DCTEST_ENABLE -o $@ $^
	@./simfs_test

simfs.a: image.o block.o
	ar rcs $@ $^

block.o: block.c
	gcc -Wall -Wextra -c $<

image.o: image.c
	gcc -Wall -Wextra -c $<

.PHONY: test

test: simfs_test
	./simfs_test

clean:
	rm -f simfs_test
