#ifndef MKFS_H
#define MKFS_H

#define FOUR_MB_IMAGE 4096*1024
#define ZEROS 0
#define METADATA 7

void initialize_file(unsigned char *block, int value, int size);
void mkfs(void);

#endif
