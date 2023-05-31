#include "inode.h"
#include "block.h"
#include "free.h"
#include "pack.h"
#include <string.h>
#include <stdio.h>

static struct inode incore[MAX_SYS_OPEN_FILES] = {0};

void clear_incore_inodes(void)
{
	for (int i = 0; i < MAX_SYS_OPEN_FILES; i++) {
		// printf("ref::: %d\n", incore[i].ref_count);
		incore[i].ref_count = 0;
	}
}

void mark_all_incore_in_use(void)
{
	for (int i = 0; i < MAX_SYS_OPEN_FILES; i++) {
		incore[i].ref_count = 1;
	}
}

void initialize_incore_inode(struct inode *in, int inode_num)
{
	// Set all values to 0
	in->size = 0;
	in->owner_id = 0;
	in->permissions = 0;
	in->flags = 0;
    for (int i = 0; i < INODE_PTR_COUNT; i++) {
    	in->block_ptr[i] = 0;
    }
    // Set the inode number
    in->inode_num = inode_num;
}

struct inode *ialloc(void)
{
	// Create a buffer block
	unsigned char block[BLOCK_SIZE];
	// Read to the block
	bread(INODE_MAP, block);
	// Find the free location
	int num = find_free(block);

	// If no free location, return NULL
	if (num == FAILURE) {
		return NULL;
	} else {
		// mark num as IN_USE
		set_free(block, num, IN_USE);
		// Write to the block
		bwrite(INODE_MAP, block);
	    // Get an in-core version of the inode (iget())
		struct inode *incore_inode = iget(num);
	    // If not found:
	    if (incore_inode == NULL) {
	    	return NULL;
	    } else {
		    // Initialize the inode:
	    	initialize_incore_inode(incore_inode, num);
		    // Save the inode to disk (write_inode())
	    	write_inode(incore_inode);
		    // Return the pointer to the in-core inode.
	    	return incore_inode;
	    }
	}
}

struct inode *find_incore_free(void)
{
	// Loop over the incore array
	for (int i = 0; i < MAX_SYS_OPEN_FILES; i++) {
		// For each incore, check if reference count is 0
		struct inode *temp_incore = &incore[i];

		// If it is, return it
		if (temp_incore->ref_count == 0) {
			return temp_incore;
		}
	}

	// Otherwise return NULL
	return NULL;
}

struct inode *find_incore(unsigned int inode_num)
{
	// Loop over the incore array
	for (int i = 0; i < MAX_SYS_OPEN_FILES; i++) {
		// For each incore, check if reference count is 0
		struct inode *temp_incore = &incore[i];

		// If it is, return it
		if (temp_incore->inode_num == inode_num && temp_incore->ref_count != 0) {
			return temp_incore;
		}
	}

	// Otherwise return NULL
	return NULL;
}

void read_inode(struct inode *in, int inode_num)
{
	// Calculate the block/offset/offset in bytes
	int block_num = inode_num / INODES_PER_BLOCK + INODE_FIRST_BLOCK;
	int block_offset = inode_num % INODES_PER_BLOCK;
	int block_offset_bytes = block_offset * INODE_SIZE;

	// Create a buffer to read to
	unsigned char read_buffer[BLOCK_SIZE];
	// Read the block to the buffer
	bread(block_num, read_buffer);

	// Read from the buffer and store in in the inode
    in->size = read_u32(read_buffer + block_offset_bytes);
    in->owner_id = read_u16(read_buffer + block_offset_bytes + OWNER_ID_OFFSET);
    in->permissions = read_u8(read_buffer + block_offset_bytes + PERMISSIONS_OFFSET);
    in->flags = read_u8(read_buffer + block_offset_bytes + FLAGS_OFFSET);
    in->link_count = read_u8(read_buffer + block_offset_bytes + LINK_COUNT_OFFSET);

    // Read all of the block pointers to the inode
    int block_pointer_address = BLOCK_POINTER_OFFSET;
    for (int i = 0; i < INODE_PTR_COUNT; i++) {
    	in->block_ptr[i] = read_u16(read_buffer + block_offset_bytes + block_pointer_address);
    	block_pointer_address += INODE_BLOCK_PTR_SIZE;
    }

}

void write_inode(struct inode *in)
{
	// Get the inode number
	int inode_num = in->inode_num;

	// Calculate the block/offset/offset in bytes
	int block_num = inode_num / INODES_PER_BLOCK + INODE_FIRST_BLOCK;
	int block_offset = inode_num % INODES_PER_BLOCK;
	int block_offset_bytes = block_offset * INODE_SIZE;

	// Create a buffer to write to
	unsigned char write_buffer[BLOCK_SIZE];

	// Write to the buffer
    write_u32(write_buffer + block_offset_bytes, in->size);
    write_u16(write_buffer + block_offset_bytes + OWNER_ID_OFFSET, in->owner_id);
    write_u8(write_buffer + block_offset_bytes + PERMISSIONS_OFFSET, in->permissions);
    write_u8(write_buffer + block_offset_bytes + FLAGS_OFFSET, in->flags);
    write_u8(write_buffer + block_offset_bytes + LINK_COUNT_OFFSET, in->link_count);

    // For each block pointer, write to the buffer
    int block_pointer_address = BLOCK_POINTER_OFFSET;
    for (int i = 0; i < INODE_PTR_COUNT; i++) {
    	write_u16(write_buffer + block_offset_bytes + block_pointer_address, in->block_ptr[i]);
    	block_pointer_address += INODE_BLOCK_PTR_SIZE;
    }

    // Write the block to disk
	bwrite(block_num, write_buffer);
}

struct inode *iget(int inode_num)
{

    // Search for the inode number in-core (find_incore())
    struct inode *incore_inode = find_incore(inode_num);
    // If found:
    if (incore_inode != NULL) {
	    // Increment the ref_count
    	incore_inode->ref_count++;
	    // Return the pointer
    	return incore_inode;
    } else {
	    // Find a free in-core inode (find_incore_free())
    	struct inode *available_incore = find_incore_free();
	    // If none found:
    	if (available_incore == NULL) {
    		return NULL;
    	}
	    // Read the data from disk into it (read_inode())
    	read_inode(available_incore, inode_num);
	    // Set the inode's ref_count to 1
    	available_incore->ref_count = 1;
	    // Set the inode's inode_num to the inode number that was passed in
    	available_incore->inode_num = inode_num;
	    // Return the pointer to the inode
    	return available_incore;
    }

}

void iput(struct inode *in)
{
    // If ref_count on in is already 0:
	if (in->ref_count == 0) {
		return;
	} else {
	    // Decrement ref_count
		in->ref_count--;
	    // If ref_count is 0:
		if (in->ref_count == 0) {
		    // Save the inode to disk (write_inode())
			write_inode(in);
		}
	}

}

struct inode *namei(char *path)
{
	return iget(ROOT_INODE_NUM);
}
