#include <fcntl.h>
#include <unistd.h>

int image_fd;

int image_open(char *filename, int truncate)
{
    if (truncate) {
        image_fd = open(filename, O_TRUNC, 0600);
    } else {
    	image_fd = open(filename, O_RDWR, 0600);
    }

    // O_RDWR: Open for both reading and writing.
    // O_CREAT: Create the file if it doesn't exist.
    // O_TRUNC: If you specify this, it will truncate the file length to 0.
    // For the third argument to open(), pass octal file permissions; 0600 is probably what you want..
    return image_fd;
}

int image_close(void)
{
	return close(image_fd);
}
