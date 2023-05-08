#include <stdio.h>
#include <string.h>
#include "ctest.h"
#include "image.h"
#include "block.h"
#include "free.h"
#include "inode.h"
#include "mkfs.h"

void initialize_block(unsigned char *block, int value) {
	for (int i = 0; i < BLOCK_SIZE; i++) {
		block[i] = value;
	}
}

#ifdef CTEST_ENABLE

void test_image_open_and_close(void)
{
	int image_fd = image_open("test", READ_WRITE);
	CTEST_ASSERT(image_fd == 3, "Test create and open new image");
	CTEST_ASSERT(image_close() == 0, "Test closing and image file");

	image_fd = image_open("test", TRUNCATE);
	CTEST_ASSERT(image_fd == 3, "Test create and truncate new image");
	CTEST_ASSERT(image_close() == 0, "Test closing truncated file");
}

void test_get_block_location(void)
{
	image_open("test", READ_WRITE);
	int sample_block_number = 5;
	int actual_block_location = sample_block_number * BLOCK_SIZE; 
	off_t block_location = get_block_location(sample_block_number);
	CTEST_ASSERT(actual_block_location == block_location, "Test calculating the block location");
	image_close();
}

void test_block_write_and_read(void)
{
	// Create test_block
	unsigned char test_block[BLOCK_SIZE];
	// Initialize array block with zeros
	initialize_block(test_block, 0);
	// Choose a block to test writing to
	int block_num = 1;
	// Open the file
	image_open("test_image", READ_WRITE);
	// Write to the test block
	bwrite(block_num, test_block);
	// Create a buffer to read from
	unsigned char read_block[BLOCK_SIZE];
	// Read the data
	bread(block_num, read_block);
	// Compare original to what was read
	CTEST_ASSERT(memcmp(test_block, read_block, BLOCK_SIZE) == 0, "Test write and read from the same block");
	// Close the image
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
	
	set_free(test_data, block_num, 0);
	// test_data[byte_num] &= ~(1 << bit_num); // Mark as free
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

	image_fd = image_open("test", READ_WRITE);
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

void test_alloc(void)
{
	int block_map = 2;

	int block_num = 35;
	int byte_num = block_num / 8;
	int bit_num = block_num % 8;

	image_fd = image_open("test", READ_WRITE);
	unsigned char test_data[BLOCK_SIZE];
	initialize_block(test_data, 255);
	bwrite(block_map, test_data);
	int num = alloc();
	printf("num: %d\n", num);
	CTEST_ASSERT(num == -1, "Test no free blocks in block map");

	initialize_block(test_data, 255);
	test_data[byte_num] &= ~(1 << bit_num); // Mark as free
	// set_free(test_data, block_num, 1);
	bwrite(block_map, test_data);
	num = alloc();
	printf("num: %d\n", num);
	CTEST_ASSERT(num == block_num, "Test alloc finds the free block");



	image_close();	
}

void test_mkfs(void)
{
	image_open("test1", READ_WRITE);
	mkfs();
	unsigned char block[BLOCK_SIZE];
	bread(2, block);
	int free = find_free(block);
	printf("free: %d\n", free);

	image_close();
}

int main(void)
{
    CTEST_VERBOSE(1);

	test_image_open_and_close();
	test_get_block_location();
	test_block_write_and_read();
	test_set_free();
	test_find_free();
	test_ialloc();
	test_alloc();
	test_mkfs();

    CTEST_RESULTS();

    CTEST_EXIT();
}

#else

int main(void)
{
    printf("Running normally!\n");
}

#endif
