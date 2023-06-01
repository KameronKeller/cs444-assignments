#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ctest.h"
#include "image.h"
#include "block.h"
#include "free.h"
#include "inode.h"
#include "mkfs.h"
#include "pack.h"
#include "directory.h"
#include "ls.h"

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
	struct inode *ialloc_inode = ialloc();
	CTEST_ASSERT(ialloc_inode == NULL, "Test no free inodes in inode map");

	// Reinitalize the test block to all ones
	initialize_block(test_block, ALL_ONES);
	// Set num as free
	set_free(test_block, num, FREE);

	// Write to the inode map
	bwrite(INODE_MAP, test_block);

	// Attempt to allocate
	ialloc_inode = ialloc();
	int ialloc_num = ialloc_inode->inode_num;
	CTEST_ASSERT(ialloc_num == num, "Test ialloc finds the free inode");

	// Mark all incores in use
	mark_all_incore_in_use();
	// Attemp to allocate
	ialloc_inode = ialloc();
	// Verify no incore inode is located
	CTEST_ASSERT(ialloc_inode == NULL, "Test no free incore inodes");

	image_close();
}

void test_alloc(void)
{
	int num = 35; // Random test value

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
	// Expect next free block to be 8 (0-6 marked as in use, 7 is the root directory)
	int expected_next_free_block = 8;
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
	CTEST_ASSERT(next_free == expected_next_free_block, "Test expect next free block to be 8 after mkfs");

	// Read the root directory block
	bread(7, block);

	// Unpack the first file's values
	int inode_number = read_u16(block);
	char file_name[3]; // size of 3 to allow room for `..\0`
	memcpy(file_name, block + FILE_NAME_OFFSET, 3);

	CTEST_ASSERT(inode_number == 0, "Test the root inode number is 0");
	CTEST_ASSERT(strcmp(file_name, ".") == 0, "Test the first directory entry is '.'");

	// Unpack the second file's values
	inode_number = read_u16(block + FIXED_LENGTH_RECORD_SIZE);
	memcpy(file_name, block + FIXED_LENGTH_RECORD_SIZE + FILE_NAME_OFFSET, 3);
	CTEST_ASSERT(inode_number == 0, "Test the root inode number is 0");
	CTEST_ASSERT(strcmp(file_name, "..") == 0, "Test the first directory entry is '..'");


	image_close();
}


void test_find_incore(void)
{
	unsigned int fake_inode_num = 34;
	// Clear all incore inodes
	clear_incore_inodes();
	// Find a free incore
	struct inode *free_inode = find_incore_free();
	// Set the inode_num and ref_count
	free_inode->inode_num = fake_inode_num;
	free_inode->ref_count = 1;
	// Test finding the incore inode
	struct inode *found_incore = find_incore(fake_inode_num);
	CTEST_ASSERT(memcmp(free_inode, found_incore, INODE_SIZE) == 0, "Test find free inode, then find by inode number");

	// Mark all incores as in use
	mark_all_incore_in_use();
	// Attempt to find a free incore
	free_inode = find_incore_free();
	// Verify none are found
	CTEST_ASSERT(free_inode == NULL, "Test no free incore inodes");

	// Clear all inodes
	clear_incore_inodes();

	// Look for the specified inode
	free_inode = find_incore(fake_inode_num);

	// Verify none found because ref_count is 0 on all incores
	CTEST_ASSERT(free_inode == NULL, "Test no incore inode found in use with the given inode number");	
}

void test_read_write_inode(void)
{
	unsigned int fake_inode_num = 215;
	image_open("test_image", READ_WRITE);
	// Find a free incore inode
	struct inode *new_inode = find_incore_free();

	// Fill with fake data
	new_inode->inode_num = fake_inode_num;
	new_inode->size = 123;
	new_inode->owner_id = 1;
	new_inode->permissions = 2;
	new_inode->flags = 3;
	new_inode->link_count = 4;
	new_inode->block_ptr[0] = 5;

	// Write the inode to file
	write_inode(new_inode);
	// Create a read buffer
	struct inode inode_read_buffer = {0};
	// Read the inode back to the buffer
	read_inode(&inode_read_buffer, fake_inode_num);
	image_close();

	// Test that inode read matches the inode written to disk
	CTEST_ASSERT(new_inode->size == inode_read_buffer.size, "Test write/read size are equivalent");
	CTEST_ASSERT(new_inode->owner_id == inode_read_buffer.owner_id, "Test write/read owner_id are equivalent");
	CTEST_ASSERT(new_inode->permissions == inode_read_buffer.permissions, "Test write/read permissions are equivalent");
	CTEST_ASSERT(new_inode->flags == inode_read_buffer.flags, "Test write/read flags are equivalent");
	CTEST_ASSERT(new_inode->link_count == inode_read_buffer.link_count, "Test write/read link_count are equivalent");
	CTEST_ASSERT(new_inode->block_ptr[0] == inode_read_buffer.block_ptr[0], "Test write/read block_ptr[0] are equivalent");
}

void test_iget(void)
{
	image_open("test_image", READ_WRITE);
	unsigned int fake_inode_num = 44;

	// find an available inode
	struct inode *available_inode = find_incore_free();

	// Verify the reference count is initially zero
	CTEST_ASSERT(available_inode->ref_count == 0, "Test ref_count is initially zero");

	// Assign an inode number to the inode and use iget to get the inode
	available_inode->inode_num = fake_inode_num;
	struct inode *iget_inode = iget(fake_inode_num);

	// Verify the reference count is incremented
	CTEST_ASSERT(iget_inode->ref_count == 1, "Test ref_count is incremented after iget");

	// Verify the correct inode is retrieved
	CTEST_ASSERT(memcmp(available_inode, iget_inode, INODE_SIZE) == 0, "Test that iget retrieves the correct inode");

	// Reset the incore inodes
	clear_incore_inodes();

	// Attempt to get the specified inode
	iget_inode = iget(fake_inode_num);

	// Verify an incore inode is returned
	CTEST_ASSERT(iget_inode != NULL, "Test if incore inode not referenced, a new incore is returned");
	CTEST_ASSERT(iget_inode->inode_num == fake_inode_num, "Test if incore inode not referenced, the new inode has the correct inode_num");

	unsigned int new_inode_num = 256;

	// Mark all inodes in use
	mark_all_incore_in_use();

	// Attempt to get an inode that doesn't exit
	iget_inode = iget(new_inode_num);
	CTEST_ASSERT(iget_inode == NULL, "Test the inode number doesn't exist and there are no free incore inodes");

	// Reset all incore inodes to 0
	clear_incore_inodes();
	image_close();
}

void test_iput(void)
{
	image_open("test_image", READ_WRITE);
	// Get the specified inode
	unsigned int fake_inode_num = 79;
	struct inode *fake_inode = iget(fake_inode_num);

	// Test putting the inode back
	iput(fake_inode);

	// Verify ref count is decremented
	CTEST_ASSERT(fake_inode->ref_count == 0, "Test that ref_count is set back to zero after iput");

	// Compare the original inode and the gotten inode
	struct inode *iget_inode = iget(fake_inode_num);
	CTEST_ASSERT(memcmp(fake_inode, iget_inode, INODE_SIZE) == 0, "Test that iput writes the inode, and that iget retrieves it correctly");
	image_close();
}

void test_directory(void)
{
	int root_directory = 0;
	
	// Open a new image and make the file system
	image_open("test_image", READ_WRITE);
	mkfs();

	// Create the entry struct
	struct directory_entry *ent = malloc(sizeof(struct directory_entry));

	// Open the directory
	struct directory *dir = directory_open(root_directory);

	// Read the directory
	int directory_get_return_value = directory_get(dir, ent);

	// Test the functions
	char *file_name = ent->name;
	CTEST_ASSERT(directory_get_return_value == 0, "Test a successful directory get returns 0");
	CTEST_ASSERT(strcmp(file_name, ".") == 0, "Test the directory entry is '.'");
	CTEST_ASSERT(ent->inode_num == 0, "Test directory entry inode number is 0");

	// Close the directory and free variables
	directory_close(dir);
	free(ent);
	
	// Close the image
	image_close();
}

void test_directory_failures(void)
{
	int root_directory = 0;

	// Open a new image and make the file system
	image_open("test_image", READ_WRITE);
	mkfs();

	// Mark all incore inodes in use to cause iget to fail
	mark_all_incore_in_use();
	struct directory *dir = directory_open(256);

	// Test that NULL is returned
	CTEST_ASSERT(dir == NULL, "Test if iget fails, NULL is returned");

	// Reset the incore inodes
	clear_incore_inodes();


	// Create a directory entry struct
	struct directory_entry *ent = malloc(sizeof(struct directory_entry));

	// Open the directory
	dir = directory_open(root_directory);

	// Set the offset to an impossible value
	dir->offset = 999999999;

	// Test that -1 is returned
	int directory_get_return_value = directory_get(dir, ent);
	CTEST_ASSERT(directory_get_return_value == -1, "Test a failed directory get returns -1");

	// Close the directory and free variables
	directory_close(dir);
	free(ent);
	
	// Close the image
	image_close();
}

void ls_output(void)
{
	image_open("test_image", READ_WRITE);
	mkfs();
	printf("\n==== Test output of ls() ====\n");

	ls(0);

	image_close();
}

void test_namei(void)
{
	unsigned int root_inode = 0;
	image_open("test_image", READ_WRITE);
	mkfs();

	// namei only returns root node currently
	struct inode *in = namei("/");
	CTEST_ASSERT(in->inode_num == root_inode, "Test namei returns root inode number");

	image_close();
}

void test_directory_make_failures(void)
{
	image_open("test_image", READ_WRITE);
	mkfs();

	// Create an invalid directory name
	int inode_num = directory_make("/foo/");
	CTEST_ASSERT(inode_num == -1, "Test a directory path can't end with /");

	// Create an invalid directory name
	inode_num = directory_make("foo");
	CTEST_ASSERT(inode_num == -1, "Test a directory path must begin with with /");

	// Copy the block map as a backup
	unsigned char original_state[BLOCK_SIZE];
	bread(BLOCK_MAP, original_state);

	// Create an empty block and fill with 1's
	unsigned char test_block[BLOCK_SIZE];
	memset(test_block, 255, BLOCK_SIZE);

	// Overwrite the block map
	bwrite(BLOCK_MAP, test_block);
	inode_num = directory_make("/baz");
	CTEST_ASSERT(inode_num == -1, "Test no available data blocks results in failure");

	// Restore the block map backup
	bwrite(BLOCK_MAP, original_state);

	image_close();
}

void test_directory_make_success(void)
{
	unsigned int expected_inode_number = 1;
	image_open("test_image", READ_WRITE);
	mkfs();

	// Create a new directory
	int directory_creation_status = directory_make("/foo");
	CTEST_ASSERT(directory_creation_status == 0, "Test a successful directory creation returns 0");
	
	// Open the directory
	struct directory *dir = directory_open(expected_inode_number);
	struct directory_entry *ent = malloc(sizeof(struct directory_entry));
	
	// Verify the entry is accurent
	directory_get(dir, ent);
	CTEST_ASSERT(strcmp(ent->name, ".") == 0, "Test a created directory returns 0 with directory get");
	CTEST_ASSERT(ent->inode_num == expected_inode_number, "Test the retrieved directory entry matches the inode number returned from directory make");

	directory_make("/foo/bar");
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
	test_find_incore();
	test_read_write_inode();
	test_iget();
	test_iput();
	test_directory();
	test_directory_failures();
	test_namei();
	test_directory_make_failures();
	test_directory_make_success();
	ls_output();

    CTEST_RESULTS();

    CTEST_EXIT();
}

#else

int main(void)
{
    printf("Running normally!\n");
}

#endif
