#include <stdlib.h>
#include <stdio.h>
#include "block.h"
#include "image.h"
#include "free.h"

off_t get_block_location(int block_num)
{
	off_t offset = block_num * BLOCK_SIZE;
	off_t block_location = lseek(image_fd, offset, SEEK_SET);
	return block_location;
}

unsigned char *bread(int block_num, unsigned char *block)
{
	off_t block_location = get_block_location(block_num);
	if (block_location == FAILURE) {
		perror("bread(): File descriptor offset failed\n");
		exit(1);
	}
	int bytes_read = read(image_fd, block, BLOCK_SIZE);
	if (bytes_read == FAILURE) {
		perror("Block read failed\n");
		exit(1);
	}

	return block;
}

void bwrite(int block_num, unsigned char *block)
{
	off_t block_location = get_block_location(block_num);
	if (block_location == FAILURE) {
		perror("bwrite(): File descriptor offset failed\n");
		exit(1);
	}
	int bytes_written = write(image_fd, block, BLOCK_SIZE);
	if (bytes_written == FAILURE) {
		perror("Block write failed\n");
		exit(1);
	}
}

int alloc(void)
{	
	// Create a block buffer
	unsigned char block[BLOCK_SIZE];
	// Read to the block buffer
	bread(BLOCK_MAP, block);
	// Find a free block
	int num = find_free(block);

	// If no blocks available
	if (num == 0) {
		return FAILURE;
	} else {
		// Set the block in use and write to file
		set_free(block, num, IN_USE);
		bwrite(num, block);
		return num;
	}
}
