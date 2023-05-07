#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "block.h"
#include "image.h"

# define BLOCK_SIZE 4096 // 4 KB block size

unsigned char *bread(int block_num, unsigned char *block)
{
	off_t offset = block_num * BLOCK_SIZE;
	off_t block_location = lseek(image_fd, offset, SEEK_SET);
	int bytes_read = read(block_location, block, BLOCK_SIZE);

	return block;
}

void bwrite(int block_num, unsigned char *block)
{
	off_t offset = block_num * BLOCK_SIZE;
	off_t block_location = lseek(image_fd, offset, SEEK_SET);
	printf("%lld\n", block_location);
	int bytes_written = write(image_fd, block, BLOCK_SIZE);
	if (bytes_written == -1) {
		printf("Failed to write\n");
		exit(1);
	}
	// printf("%d\n", bytes_written);

}
