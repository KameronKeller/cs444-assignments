#include <stdio.h>
#include <string.h>
#include "ctest.h"
#include "image.h"
#include "block.h"
#include "free.h"
#include "inode.h"

void initialize_block(unsigned char *block, int value) {
	for (int i = 0; i < BLOCK_SIZE; i++) {
		block[i] = value;
	}
}

#ifdef CTEST_ENABLE

void test_image_open_and_close(void)
{
	int image_fd = image_open("test", 0);
	CTEST_ASSERT(image_fd == 3, "Test create and open new image");
	CTEST_ASSERT(image_close() == 0, "Test closing and image file");

	image_fd = image_open("test", 1);
	CTEST_ASSERT(image_fd == 3, "Test create and truncate new image");
	CTEST_ASSERT(image_close() == 0, "Test closing truncated file");
}

void test_block_write_and_read(void)
{
	int block_num = 7;
	unsigned char test_data[BLOCK_SIZE];
	initialize_block(test_data, 0); // Initialize array block with zeros
	image_open("test_image", 0); // Open the file
	bwrite(block_num, test_data);

	unsigned char read_data[BLOCK_SIZE];
	bread(block_num, read_data);

	CTEST_ASSERT(memcmp(test_data, read_data, BLOCK_SIZE) == 0, "Test write and read from the same block");
	image_close();
}

void test_set_free(void)
{
	int in_use = 1;
	int free = 0;

	int num = 500;
	int byte_num = num / 8;  // 8 bits per byte
	int bit_num = num % 8;

	unsigned char test_data[BLOCK_SIZE];
	initialize_block(test_data, 0); // Initialize the block with zeros

	set_free(test_data, num, in_use); // Mark the bit in use
	// Get the bit from the character
	unsigned char character = test_data[byte_num];
	int bit = (character >> bit_num) & in_use;

	CTEST_ASSERT(bit == in_use, "Test in use");

	set_free(test_data, num, free); // Mark the bit free
	// Get the bit from the character
	character = test_data[byte_num];
	bit = (character >> bit_num) & free;	

	CTEST_ASSERT(bit == free, "Test free");
}

void test_find_free(void)
{
	int block_num = 514; // Randomly chosen block number
	int byte_num = block_num / 8;
	int bit_num = block_num % 8;

	unsigned char test_data[BLOCK_SIZE];
	initialize_block(test_data, 255);
	test_data[byte_num] &= ~(1 << bit_num); // Mark as free
	// for (int i = 60; i < 70; i++) {
		// printf("i: %d, %d\n", i, test_data[i]);
	// }
	// printf("free_block: %d\n", find_free(test_data));
	CTEST_ASSERT(find_free(test_data) == block_num, "Test find free finds free block");
}

void test_ialloc(void)
{
	int inode_map = 1;

	int block_num = 35;
	int byte_num = block_num / 8;
	int bit_num = block_num % 8;

	image_fd = image_open("test", 0);
	unsigned char test_data[BLOCK_SIZE];
	initialize_block(test_data, 255);
	bwrite(1, test_data);
	int num = ialloc();
	printf("num: %d\n", num);
	CTEST_ASSERT(num == -1, "Test no free inodes in inode map");

	initialize_block(test_data, 255);
	test_data[byte_num] &= ~(1 << bit_num); // Mark as free
	bwrite(1, test_data);
	num = ialloc();
	printf("num: %d\n", num);
	CTEST_ASSERT(num == block_num, "Test ialloc finds the free inode");



	image_close();
}

int main(void)
{
    CTEST_VERBOSE(1);

	test_image_open_and_close();
	test_block_write_and_read();
	test_set_free();
	test_find_free();
	test_ialloc();

    CTEST_RESULTS();

    CTEST_EXIT();
}

#else

int main(void)
{
    printf("Running normally!\n");
}

#endif
