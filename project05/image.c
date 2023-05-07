#include <fcntl.h>
#include <unistd.h>

#include "image.h"

int image_fd;

int image_open(char *filename, int truncate)
{
    if (truncate) {
        image_fd = open(filename, O_TRUNC, 0600);
    } else {
    	image_fd = open(filename, O_RDWR | O_CREAT, 0600);
    }


    return image_fd;
}

int image_close(void)
{
	return close(image_fd);
}
