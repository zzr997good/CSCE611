/*
     File        : file.C

     Author      : Riccardo Bettati
     Modified    : 2021/11/28

     Description : Implementation of simple File class, with support for
                   sequential read/write operations.
*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

#define INODES_BLOCK_NO 0
#define FREELIST_BLOCK_NO 1
#define DISK_BLOCK_SIZE 512

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "assert.H"
#include "console.H"
#include "file.H"

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR/DESTRUCTOR */
/*--------------------------------------------------------------------------*/

File::File(FileSystem *_fs, int _id) {
    Console::puts("Opening file.\n");
    fs=_fs;
    inode=fs->LookupFile(_id);
    current_pos=0;
    fs->ReadBlockFromDisk(inode->block_no,block_cache);

    //assert(false);
}

File::~File() {
    Console::puts("Closing file.\n");
    /* Make sure that you write any cached data to disk. */
    /* Also make sure that the inode in the inode list is updated. */
    fs->WriteBlockToDisk(inode->block_no,block_cache);
    //Because the fs->inodes is private member, we have to use its friend class inode to write inode list to disk
    inode->WriteToDisk();
}

/*--------------------------------------------------------------------------*/
/* FILE FUNCTIONS */
/*--------------------------------------------------------------------------*/

int File::Read(unsigned int _n, char *_buf) {
    Console::puts("reading from file\n");
    //assert(false);
    unsigned int count = 0;
    while(!EoF() && (count<_n))
    {
    	_buf[count] = block_cache[current_pos];
    	count++;
    	current_pos++;
    }
    if(EoF()) Reset();
    return count;
}

int File::Write(unsigned int _n, const char *_buf) {
    Console::puts("writing to file\n");
    unsigned int count = 0;
    if(_n > SimpleDisk::BLOCK_SIZE)    //The maximum size is 512B
    _n = SimpleDisk::BLOCK_SIZE;
    while(count<_n)
    {
    	if(EoF()) Reset();
    	block_cache[current_pos] = _buf[count];
    	count++;
    	current_pos++;
    }
    if(current_pos+1>inode->size)   inode->size=current_pos+1;
    return count; 
    //assert(false);
}

void File::Reset() {
    Console::puts("resetting file\n");
    current_pos=0;
    //assert(false);
}

bool File::EoF() {
    Console::puts("checking for EoF\n");
    return current_pos==SimpleDisk::BLOCK_SIZE;
    //assert(false);
}
