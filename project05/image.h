#ifndef IMAGE_H
#define IMAGE_H

int image_open(char *filename, int truncate);
int image_close(void);

extern int image_fd;

#endif
