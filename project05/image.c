int image_fd;

int image_open(char *filename, int truncate)
{
	open();

    // O_RDWR: Open for both reading and writing.
    // O_CREAT: Create the file if it doesn't exist.
    // O_TRUNC: If you specify this, it will truncate the file length to 0.
    // For the third argument to open(), pass octal file permissions; 0600 is probably what you want..
    

}

int image_close(void)
{
	close();
}
