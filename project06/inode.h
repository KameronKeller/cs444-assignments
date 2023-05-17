#ifndef INODE_H
#define INODE_H

#define INODE_MAP 1
#define FAILURE -1

#define INODE_PTR_COUNT 16

#define MAX_SYS_OPEN_FILES 64

#define INODE_SIZE 64

struct inode {
    unsigned int size;
    unsigned short owner_id;
    unsigned char permissions;
    unsigned char flags;
    unsigned char link_count;
    unsigned short block_ptr[INODE_PTR_COUNT];

    unsigned int ref_count;  // in-core only
    unsigned int inode_num;
};

int ialloc(void);
struct inode *find_incore_free(void);
struct inode *find_incore(unsigned int inode_num);

#endif
