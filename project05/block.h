#ifndef BLOCK_H
#define BLOCK_H

#include <unistd.h>

#define BLOCK_SIZE 4096 // 4 KB block size
#define BLOCK_MAP 2
#define FAILURE -1

off_t get_block_location(int block_num);
unsigned char *bread(int block_num, unsigned char *block);
void bwrite(int block_num, unsigned char *block);
int alloc(void);

#endif
