#ifndef IMAGE_H
#define IMAGE_H

#define TRUNCATE 1 // Flag for truncating the file
#define READ_WRITE 0 // Flag for reading, writing, and/or creating the file if it doesn't exit

int image_open(char *filename, int truncate);
int image_close(void);

extern int image_fd;

#endif
