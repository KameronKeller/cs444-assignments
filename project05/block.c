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
	unsigned char block[BLOCK_SIZE];
	bread(BLOCK_MAP, block);
	int num = find_free(block);
	printf("num: %d\n", num);

	if (num == 0) {
		return FAILURE;
	} else {
		set_free(block, num, 1);
		bwrite(num, block);
		return num;
	}
}
