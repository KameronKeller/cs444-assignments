#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "inode.h"
#include "directory.h"
#include "block.h"
#include "mkfs.h"
#include "pack.h"
#include "dirbasename.h"

struct directory *directory_open(int inode_num)
{
    // Get the inode for this file
    struct inode *directory_inode = iget(inode_num);

    // If iget fails, return NULL
    if (directory_inode == NULL) {
        return NULL;
    }

    // Malloc space for the directory
    struct directory *open_directory = malloc(sizeof(struct directory));

    // Initialize directory
    open_directory->inode = directory_inode;
    open_directory->offset = INITIAL_OFFSET;

    // Return the open directory
    return open_directory;
}

int directory_get(struct directory *dir, struct directory_entry *ent)
{
    // Check if the offset is valid
    unsigned int offset = dir->offset;
    unsigned int size = dir->inode->size;
    if (offset >= size) {
        return FAILURE;
    }

    // Calculate the block index and block number
    int data_block_index = offset / BLOCK_SIZE;
    short data_block_num = dir->inode->block_ptr[data_block_index] / BLOCK_SIZE;

    // Read the block to a block array
    unsigned char block[BLOCK_SIZE];
    bread(data_block_num, block);

    // Calculate the offset within the block
    int offset_in_block = offset % BLOCK_SIZE;

    // Read the inode number
    ent->inode_num = read_u16(block + offset_in_block);

    // Copy the file name
    strcpy(ent->name, (char *)block + offset + FILE_NAME_OFFSET);

    dir->offset = offset + FIXED_LENGTH_RECORD_SIZE;

    return SUCCESS;
}

void directory_close(struct directory *d)
{
    iput(d->inode);
    free(d);
}

int directory_make(char *path)
{
    char directory_path[1024];
    char directory_name[1024];
    get_dirname(path, directory_path);
    get_basename(path, directory_name);

    // Find the inode for the parent directory that will hold the new entry.
    struct inode *parent_inode = namei(directory_path);

    // Create a new inode for the new directory.
    struct inode *new_directory_inode = ialloc();

    // Create a new data block for the new directory entries.
    int directory_block = alloc();

    // Get the root inode
    // struct inode *parent_inode = namei(directory_path);

    // Create an empty block to store the directory information
	unsigned char block[BLOCK_SIZE];

    // Write the "." file to the block
	write_u16(block, new_directory_inode->inode_num);
	strcpy((char *)block + FILE_NAME_OFFSET, ".");

    // Write the ".." file to the block
	write_u16(block + FIXED_LENGTH_RECORD_SIZE, parent_inode->inode_num);
	strcpy((char *)block + FILE_NAME_OFFSET + FIXED_LENGTH_RECORD_SIZE, "..");

    // Initialize the root inode
	new_directory_inode->flags = DIRECTORY_FLAG;
	new_directory_inode->size = NEW_DIR_SIZE;
	new_directory_inode->block_ptr[0] = get_block_location(directory_block);

    // Write the block to disk
	bwrite(directory_block, block);

    // Find the block that will contain the new directory entry
    short parent_dir_data_block_num = parent_inode->block_ptr[0] / BLOCK_SIZE;
    bread(parent_dir_data_block_num, block);
    void *directory_data_location = block + parent_inode->size;

    // Write the inode number to the directory data block
    write_u16(directory_data_location, new_directory_inode->inode_num);

    // Copy the directory name to the data block
    strcpy((char *)directory_data_location + FILE_NAME_OFFSET, directory_name);

    // Write the block to disk
    bwrite(parent_dir_data_block_num, block);

    // Update the parent directories size
    parent_inode->size += FIXED_LENGTH_RECORD_SIZE;

    iput(new_directory_inode);
    iput(parent_inode);

    return 0;
}