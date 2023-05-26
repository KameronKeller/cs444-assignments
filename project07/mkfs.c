#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "image.h"
#include "block.h"
#include "mkfs.h"
#include "inode.h"
#include "pack.h"


void mkfs(void)
{
	unsigned char initialize_data[FOUR_MB_IMAGE];
	memset(initialize_data, 0, FOUR_MB_IMAGE);

	write(image_fd, initialize_data, FOUR_MB_IMAGE);

	for (int i = 0; i < METADATA; i++) {
		alloc();
	}

	// Allocate the root inode and directory block
	struct inode *root_inode = ialloc();
	int directory_block = alloc();

	// Initialize the root inode
	root_inode->flags = DIRECTORY_FLAG;
	root_inode->size = ROOT_DIR_SIZE;
	root_inode->block_ptr[0] = get_block_location(directory_block);

	// Create an empty block to store the directory information
	unsigned char block[BLOCK_SIZE];

	// Write the "." file to the block
	write_u16(block, root_inode->inode_num);
	strcpy((char *)block + FILE_NAME_OFFSET, ".");

	// Write the ".." file to the block
	write_u16(block + FIXED_LENGTH_RECORD_SIZE, root_inode->inode_num);
	strcpy((char *)block + FILE_NAME_OFFSET + FIXED_LENGTH_RECORD_SIZE, "..");

	// Write the block to disk
	bwrite(directory_block, block);

	// Write the inode to disk
	iput(root_inode);

}
