#define INITIAL_OFFSET 0
#define FAILURE -1
#define SUCCESS 0
#define MAX_FILE_NAME_LENGTH 16

struct directory {
    struct inode *inode;
    unsigned int offset;
};

struct directory_entry {
    unsigned int inode_num;
    char name[MAX_FILE_NAME_LENGTH];
};

struct directory *directory_open(int inode_num);
int directory_get(struct directory *dir, struct directory_entry *ent);
void directory_close(struct directory *d);