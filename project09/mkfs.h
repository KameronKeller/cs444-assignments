#ifndef MKFS_H
#define MKFS_H

#define FOUR_MB_IMAGE 4096*1024
#define ZEROS 0
#define METADATA 7
#define DIRECTORY_FLAG 2
#define FIXED_LENGTH_RECORD_SIZE 32
#define ROOT_DIR_SIZE FIXED_LENGTH_RECORD_SIZE*2
#define FILE_NAME_OFFSET 2

void mkfs(void);

#endif
