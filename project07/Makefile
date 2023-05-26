simfs_test: simfs_test.c simfs.a
	gcc -Wall -Wextra -DCTEST_ENABLE -o $@ $^

simfs.a: image.o block.o free.o inode.o mkfs.o pack.o directory.o ls.o
	ar rcs $@ $^

ls.o: ls.c
	gcc -Wall -Wextra -c $<

directory.o: directory.c
	gcc -Wall -Wextra -c $<

pack.o: pack.c
	gcc -Wall -Wextra -c $<

mkfs.o: mkfs.c
	gcc -Wall -Wextra -c $<

inode.o: inode.c
	gcc -Wall -Wextra -c $<

free.o: free.c
	gcc -Wall -Wextra -c $<

block.o: block.c
	gcc -Wall -Wextra -c $<

image.o: image.c
	gcc -Wall -Wextra -c $<

.PHONY: test

test: simfs_test
	./simfs_test

clean:
	rm -f simfs_test test_image
