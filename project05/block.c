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
	if (bytes_read == -1) {
		perror("Block read failed\n");
		exit(1);
	}

	return block;
}

void bwrite(int block_num, unsigned char *block)
{
	off_t offset = block_num * BLOCK_SIZE;
	off_t block_location = lseek(image_fd, offset, SEEK_SET);
	if (block_location == -1) {
		perror("File descriptor offset failed\n");
		exit(1);
	}
	int bytes_written = write(image_fd, block, 4);
	if (bytes_written == -1) {
		perror("Block write failed\n");
		exit(1);
	}


}
