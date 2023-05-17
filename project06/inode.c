#include "inode.h"
#include "block.h"
#include "free.h"
#include <stdio.h>

static struct inode incore[MAX_SYS_OPEN_FILES] = {0};

int ialloc(void)
{
	// Create a buffer block
	unsigned char block[BLOCK_SIZE];
	// Read to the block
	bread(INODE_MAP, block);
	// Find the free location
	int num = find_free(block);

	// If no free location, return 0
	if (num == FAILURE) {
		return FAILURE;
	} else {
		// mark num as IN_USE
		set_free(block, num, IN_USE);
		// Write to the block
		bwrite(INODE_MAP, block);
		return num;
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
