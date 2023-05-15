#ifndef FREE_H
#define FREE_H

#define BYTE 8
#define FREE 0
#define IN_USE 1
#define FAILURE -1

int find_low_clear_bit(unsigned char x);
int calculate_block_number(int index, int bit_num);
void set_free(unsigned char *block, int num, int set);
int find_free(unsigned char *block);

#endif
