#include <stdio.h>
#include <string.h>
#include "ctest.h"
#include "image.h"
#include "block.h"
#include "free.h"
#include "inode.h"
#include "mkfs.h"

#define ALL_ONES 255
#define ZEROS 0

void initialize_block(unsigned char *block, int value) {
	for (int i = 0; i < BLOCK_SIZE; i++) {
		block[i] = value;
	}
}

#ifdef CTEST_ENABLE

void test_image_open_and_close(void)
{
	int image_fd = image_open("test_image", READ_WRITE);
	CTEST_ASSERT(image_fd == 3, "Test create and open new image");
	CTEST_ASSERT(image_close() == 0, "Test closing and image file");

	image_fd = image_open("test_image", TRUNCATE);
	CTEST_ASSERT(image_fd == 3, "Test create and truncate new image");
	CTEST_ASSERT(image_close() == 0, "Test closing truncated file");
}

void test_get_block_location(void)
{
	image_open("test_image", READ_WRITE);
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
	int num = 500; // Random test value
	int byte_num = num / BYTE;
	int bit_num = num % BYTE;

	// Initialize the block with zeros
	unsigned char test_block[BLOCK_SIZE];
	initialize_block(test_block, ZEROS); 

	// Mark the bit in use
	set_free(test_block, num, IN_USE); 

	// Get the bit from the character
	unsigned char character = test_block[byte_num];
	int bit = (character >> bit_num) & IN_USE;

	// Verify the bit matches what it was set to
	CTEST_ASSERT(bit == IN_USE, "Test in use");

	// Mark the bit FREE
	set_free(test_block, num, FREE); 

	// Get the bit from the character
	character = test_block[byte_num];
	bit = (character >> bit_num) & FREE;	

	// Verify the bit matches what it was set to
	CTEST_ASSERT(bit == FREE, "Test free");
}

void test_find_free(void)
{
	int num = 514; // Random test value 

	// Create a block filled with 1's
	unsigned char test_block[BLOCK_SIZE];
	initialize_block(test_block, ALL_ONES);
	
	// Set a bit as FREE
	set_free(test_block, num, FREE);

	// Check if find free locates the bit
	CTEST_ASSERT(find_free(test_block) == num, "Test find free finds free block");
}

void test_ialloc(void)
{

	int num = 35; // Random test value

	// Open the test image
	image_fd = image_open("test_image", READ_WRITE);

	// Initialize a test block to all ones
	unsigned char test_block[BLOCK_SIZE];
	initialize_block(test_block, ALL_ONES);

	// Write to the INODE MAP
	bwrite(INODE_MAP, test_block);

	// Attempt to allocate
	int ialloc_num = ialloc();
	CTEST_ASSERT(ialloc_num == FAILURE, "Test no free inodes in inode map");

	// Reinitalize the test block to all ones
	initialize_block(test_block, ALL_ONES);
	// Set num as free
	set_free(test_block, num, FREE);

	// Write to the inode map
	bwrite(INODE_MAP, test_block);

	// Attempt to allocate
	ialloc_num = ialloc();
	CTEST_ASSERT(ialloc_num == num, "Test ialloc finds the free inode");

	image_close();
}

void test_alloc(void)
{
	int num = 35; // Random test value
	// int byte_num = num / BYTE;
	// int bit_num = num % BYTE;

	// Open the test image
	image_open("test_image", READ_WRITE);

	// Initialize a test block to all ones
	unsigned char test_block[BLOCK_SIZE];
	initialize_block(test_block, ALL_ONES);

	// Write the test block
	bwrite(BLOCK_MAP, test_block);
	
	// Attempt to allocate
	int alloc_num = alloc();
	CTEST_ASSERT(alloc_num == FAILURE, "Test no free blocks in block map");

	// Reinitalize the test block to all ones
	initialize_block(test_block, ALL_ONES);
	// Set num as free
	set_free(test_block, num, FREE);

	// Write to the block map
	bwrite(BLOCK_MAP, test_block);

	// Attempt to allocate
	alloc_num = alloc();
	CTEST_ASSERT(alloc_num == num, "Test alloc finds the free block");

	image_close();	
}

void test_mkfs(void)
{
	// Expect next free block to be 7 (0-6 marked as in use)
	int expected_next_free_block = 7;
	// Open the image file
	image_open("test_image", READ_WRITE);
	
	// Build the file system
	mkfs();
	
	// Create a block buffer
	unsigned char block[BLOCK_SIZE];

	// Read to the buffer
	bread(BLOCK_MAP, block);

	// Check what the next free block is
	int next_free = find_free(block);
	CTEST_ASSERT(next_free == expected_next_free_block, "Test expect next free block to be 7 after mkfs");
	image_close();
}


void test_find_incore(void)
{
	struct inode *free_inode = find_incore_free();
	free_inode->inode_num = 34;
	free_inode->ref_count = 1;
	struct inode *found_incore = find_incore(34);
	CTEST_ASSERT(memcmp(free_inode, found_incore, INODE_SIZE) == 0, "Test find free inode, then find by inode number");
}

void test_read_write_inode(void)
{
	int fake_inode_num = 215;
	image_open("test_image", READ_WRITE);
	struct inode *new_inode = find_incore_free();

	new_inode->inode_num = fake_inode_num;
	new_inode->size = 123;
	new_inode->owner_id = 1;
	new_inode->permissions = 2;
	new_inode->flags = 3;
	new_inode->link_count = 4;
	new_inode->block_ptr[0] = 5;

	write_inode(new_inode);
	struct inode inode_read_buffer = {0};
	read_inode(&inode_read_buffer, fake_inode_num);
	image_close();

	CTEST_ASSERT(new_inode->size == inode_read_buffer.size, "Test write/read size are equivalent");
	CTEST_ASSERT(new_inode->owner_id == inode_read_buffer.owner_id, "Test write/read owner_id are equivalent");
	CTEST_ASSERT(new_inode->permissions == inode_read_buffer.permissions, "Test write/read permissions are equivalent");
	CTEST_ASSERT(new_inode->flags == inode_read_buffer.flags, "Test write/read flags are equivalent");
	CTEST_ASSERT(new_inode->link_count == inode_read_buffer.link_count, "Test write/read link_count are equivalent");
	CTEST_ASSERT(new_inode->block_ptr[0] == inode_read_buffer.block_ptr[0], "Test write/read block_ptr[0] are equivalent");
}

void test_iget(void)
{
	unsigned int fake_inode_num = 44;
	struct inode *available_inode = find_incore_free();
	CTEST_ASSERT(available_inode->ref_count == 0, "Test ref_count is initially zero");

	available_inode->inode_num = fake_inode_num;
	struct inode *iget_inode = iget(fake_inode_num);
	printf("ref_count: %d\n", iget_inode->ref_count);

	CTEST_ASSERT(iget_inode->ref_count == 1, "Test ref_count is incremented after iget");
	CTEST_ASSERT(memcmp(available_inode, iget_inode, INODE_SIZE) == 0, "Test that iget retrieves the correct inode");

	clear_incore_inodes();
	iget_inode = iget(fake_inode_num);
	CTEST_ASSERT(iget_inode != NULL, "Test if incore inode not referenced, a new incore is returned");
	CTEST_ASSERT(iget_inode->inode_num == fake_inode_num, "Test if incore inode not referenced, the new inode has the correct inode_num");
	clear_incore_inodes();
}

void test_iput(void)
{
	image_open("test_image", READ_WRITE);
	unsigned int fake_inode_num = 79;
	struct inode *fake_inode = iget(fake_inode_num);
	iput(fake_inode);
	CTEST_ASSERT(fake_inode->ref_count == 0, "Test that ref_count is set back to zero after iput");
	struct inode *iget_inode = iget(fake_inode_num);
	CTEST_ASSERT(memcmp(fake_inode, iget_inode, INODE_SIZE) == 0, "Test that iput writes the inode, and that iget retrieves it correctly");
	image_close();
}

int main(void)
{
    CTEST_VERBOSE(1);

	// test_image_open_and_close();
	// test_get_block_location();
	// test_block_write_and_read();
	// test_set_free();
	// test_find_free();
	// test_ialloc();
	// test_alloc();
	// test_mkfs();
	// test_find_incore();
	// test_read_write_inode();
	// test_iget();
	test_iput();

    CTEST_RESULTS();

    CTEST_EXIT();
}

#else

int main(void)
{
    printf("Running normally!\n");
}

#endif
