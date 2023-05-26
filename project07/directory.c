#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "inode.h"
#include "directory.h"
#include "block.h"
#include "mkfs.h"
#include "pack.h"

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
    open_directory->offset = 0;

    // Return the open directory
    return open_directory;
}

int directory_get(struct directory *dir, struct directory_entry *ent)
{
    // Check if the offset is valid
    unsigned int offset = dir->offset;
    unsigned int size = dir->inode->size;
    if (offset >= size) {
        return -1;
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
    strcpy(ent->name, (char *)block + FILE_NAME_OFFSET);

    return 0;
}

void directory_close(struct directory *d)
{
    iput(d->inode);
    free(d);
}