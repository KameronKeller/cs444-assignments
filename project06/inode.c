#include "inode.h"
#include "block.h"
#include "free.h"

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
