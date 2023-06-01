simfs_test: simfs_test.c simfs.a
	gcc -Wall -Wextra -DCTEST_ENABLE -g -o $@ $^

simfs.a: image.o block.o free.o inode.o mkfs.o pack.o directory.o ls.o dirbasename.o
	ar rcs $@ $^

dirbasename.o: dirbasename.c
	gcc -Wall -Wextra -g -c $<

ls.o: ls.c
	gcc -Wall -Wextra -g -c $<

directory.o: directory.c
	gcc -Wall -Wextra -g -c $<

pack.o: pack.c
	gcc -Wall -Wextra -g -c $<

mkfs.o: mkfs.c
	gcc -Wall -Wextra -g -c $<

inode.o: inode.c
	gcc -Wall -Wextra -g -c $<

free.o: free.c
	gcc -Wall -Wextra -g -c $<

block.o: block.c
	gcc -Wall -Wextra -g -c $<

image.o: image.c
	gcc -Wall -Wextra -g -c $<

.PHONY: test

test: simfs_test
	./simfs_test

clean:
	rm -f simfs_test test_image
