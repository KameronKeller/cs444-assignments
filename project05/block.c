#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "block.h"
#include "image.h"

# define BLOCK_SIZE 4096 // 4 KB block size

off_t get_block_location(int block_num)
{
	off_t offset = block_num * BLOCK_SIZE;
	off_t block_location = lseek(image_fd, offset, SEEK_SET);
	return block_location;
}

unsigned char *bread(int block_num, unsigned char *block)
{
	// off_t offset = block_num * BLOCK_SIZE;
	// off_t block_location = lseek(image_fd, offset, SEEK_SET);
	off_t block_location = get_block_location(block_num);
	// printf("bread %lld\n", block_location);
	// printf("fd %d\n", image_fd);
	if (block_location == -1) {
		perror("bread(): File descriptor offset failed\n");
		exit(1);
	}
	int bytes_read = read(image_fd, block, BLOCK_SIZE);
	if (bytes_read == -1) {
		perror("Block read failed\n");
		exit(1);
	}

	return block;
}

void bwrite(int block_num, unsigned char *block)
{
	// off_t offset = block_num * BLOCK_SIZE;
	// off_t block_location = lseek(image_fd, offset, SEEK_SET);
	off_t block_location = get_block_location(block_num);
	// printf("bwrite %lld\n", block_location);
	// printf("fd %d\n", image_fd);
	if (block_location == -1) {
		perror("bwrite(): File descriptor offset failed\n");
		exit(1);
	}
	int bytes_written = write(image_fd, block, BLOCK_SIZE);
	if (bytes_written == -1) {
		perror("Block write failed\n");
		exit(1);
	}


}
