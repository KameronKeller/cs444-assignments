#ifndef BLOCK_H
#define BLOCK_H

# define BLOCK_SIZE 4096 // 4 KB block size

unsigned char *bread(int block_num, unsigned char *block);
void bwrite(int block_num, unsigned char *block);

#endif
