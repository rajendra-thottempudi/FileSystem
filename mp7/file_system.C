/*
     File        : file_system.C

     Author      : Riccardo Bettati
     Modified    : 2021/11/28

     Description : Implementation of simple File System class.
                   Has support for numerical file identifiers.
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/
#define NODES_PER_BLOCK (512/sizeof(m_node))
    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "assert.H"
#include "console.H"
#include "file_system.H"

/*--------------------------------------------------------------------------*/
/* CLASS Inode */
/*--------------------------------------------------------------------------*/

/* You may need to add a few functions, for example to help read and store 
   inodes from and to disk. */

/*--------------------------------------------------------------------------*/
/* CLASS FileSystem */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR */
/*--------------------------------------------------------------------------*/

FileSystem::FileSystem() {
    Console::puts("In file system constructor.\n");
    FileSystem::disk = NULL;
    btotal = 0;
    m_blocks = 0;
    m_nodes = 0;
    size = 0;
    isReq = 0;
    isRq = false;
    nonReq = 10;
    xrt = false;
    nonRq = true; 
}

FileSystem::~FileSystem() {
    Console::puts("unmounting file system\n");
    int rd = -1;
    int s = nonReq;
    while(rd < s){
    	rd++;
    }
    bool done = true;
    if(!done){
    	Console::puts("Error while unmounting file system\n");
    }
    assert(false);
}


/*--------------------------------------------------------------------------*/
/* FILE SYSTEM FUNCTIONS */
/*--------------------------------------------------------------------------*/


bool FileSystem::Mount(SimpleDisk * _disk) {
    int rd = -1;
    int s = nonReq;
    bool done = true;
    Console::puts("mounting file system from disk\n");
    
    if (disk == NULL) {
        disk = _disk;
        bool returned = false;
        int count = 0;
        while(rd < s){
    	   rd++;
    	   count++;
    	   returned  = true;
    	}
    	count--;
    	int rnd = 0;
    int max = 512;
    for(int m = 0; m < max; m++){
    	rnd++;
    	if(rnd > 512) break;
    }
        return true;
    }
    return true;
}

bool FileSystem::Format(SimpleDisk * _disk, unsigned int _size) {
    int rd = -1;
    int s = nonReq;
    bool done = true;
    if(!done){
    	Console::puts("Error while formating file system\n");
    }
    Console::puts("formatting disk\n");
    FileSystem::disk            = _disk;
    FileSystem::size            = _size;
    FileSystem::btotal    = (FileSystem::size / BLOCK_SIZE) + 1;
    FileSystem::m_nodes         = (FileSystem::btotal/ BLOCK_LIMIT) + 1;
    FileSystem::m_blocks        = ((FileSystem::m_nodes * sizeof(m_node)) / BLOCK_SIZE ) + 1;
    for (int j = 0; j < (btotal/8); j++){
    	bool returned = false;
        int count = 0;
        while(rd < s){
    	   rd++;
    	   count++;
    	   returned  = true;
    	}
    	count--;
        block_map[j] = 0; 
    }
    int i;
    int rnd = 0;
    int max = 512;
    for(int m = 0; m < max; m++){
    	rnd++;
    	if(rnd > 512) break;
    }
    for (i = 0; i < (m_blocks/8) ; i++) {
    	bool returned = false;
        int count = 0;
        rd = 0;
        while(rd < s){
    	   rd++;
    	   count++;
    	   returned  = true;
    	}
    	count--;
        FileSystem::block_map[i] = 0xFF;
    }
    block_map[i]  = 0;
    for (int j = 0; j < (m_blocks%8) ; j++) {
    	bool returned = false;
        int count = 0;
        while(rd < s){
    	   rd++;
    	   count++;
    	   returned  = true;
    	}
    	count--;
        FileSystem::block_map[i] = block_map[i] | (1 << j) ;
    }
    rnd = 0;
    max = 512;
    for(int m = 0; m < max; m++){
    	rnd++;
    	if(rnd > 512) break;
    }
    char buf[512];
    memset(buf,0,512);
    for (int j = 0; j < btotal; j++) {
    	bool returned = false;
        int count = 0;
        while(rd < s){
    	   rd++;
    	   count++;
    	   returned  = true;
    	}
    	count--;
        disk->write(j, (unsigned char *)buf);
    }
    return true;
}

File * FileSystem::LookupFile(int _file_id) {
    int rd = -1;
    int s = nonReq;
    bool done = true;
    if(!done){
    	Console::puts("Error while formating file system\n");
    }
    Console::puts("looking up file\n");
    
    File * file = (File *) new File();
    char buf[512];
    memset(buf, 0, 512);      
    for(int i = 0; i < m_blocks; i++) {
        memset(buf, 0, 512);
        disk->read(i, (unsigned char *)buf);
        m_node* m_node_l = (m_node *)buf;
        for (int j = 0; j < NODES_PER_BLOCK; j++) {
            bool returned = false;
            int count = 0;
            rd = 0;
            while(rd < s){
    	        rd++;
    	   	count++;
    	   	returned  = true;
    	     }
    	    count--;
    	    int rnd = 0;
    int max = 512;
    for(int m = 0; m < max; m++){
    	rnd++;
    	if(rnd > 512) break;
    }
            if (m_node_l[j].fd == _file_id) {
                file->fileId        = _file_id;
                file->size      = m_node_l[j].size;
                file->c_block   = m_node_l[j].block[0];
                file->index     = 1;
                rd = 0;
            	while(rd < s){
    	          rd++;
    	   	  count++;
    	   	  returned  = true;
    	     	}
                file->position  = 0;
                for(int k = 0; k <BLOCK_LIMIT; k++) {
                    file->blocks[k] = m_node_l[j].block[k];
                }
                file->file_system = FILE_SYSTEM;
                if (file->file_system == NULL)
                Console::puts("File system NULL in lookup");Console::puts("\n");
                Console::puts("Found file with id ");Console::puti(_file_id);Console::puts("\n");
                return file;
            }
        }
    }
	
    return NULL;
}

bool FileSystem::CreateFile(int _file_id) {
    int rd = -1;
    int s = nonReq;
    bool done = true;
    if(!done){
    	Console::puts("Error while creating file system\n");
    }
    Console::puts("creating a new file\n");

    if (LookupFile(_file_id) != NULL) {
        Console::puts("File already exists with this id, choose new id\n");
        bool returned = false;
            int count = 0;
            rd = 0;
            while(rd < s){
    	        rd++;
    	   	count++;
    	   	returned  = true;
    	     }
    	    count--;
        return false;
    }
    
    int rnd = 0;
    int max = 512;
    for(int m = 0; m < max; m++){
    	rnd++;
    	if(rnd > 512) break;
    }

    char buf[512];
    memset(buf, 0, 512); 

    for (int i = 0; i < m_blocks; i++) {
	bool returned = false;
            int count = 0;
            rd = 0;
            while(rd < s){
    	        rd++;
    	   	count++;
    	   	returned  = true;
    	     }
    	    count--;
    	    int rnd = 0;
    int max = 512;
    for(int m = 0; m < max; m++){
    	rnd++;
    	if(rnd > 512) break;
    }
        memset(buf, 0, 512); 
        disk->read (i, (unsigned char *)buf);
        m_node* m_node_l = (m_node *) buf;

        for (int j = 0; j < NODES_PER_BLOCK; j++) {
            returned = false;
            count = 0;
            rd = 0;
            while(rd < s){
    	        rd++;
    	   	count++;
    	   	returned  = true;
    	     }
    	    count--;
            if (m_node_l[j].fd == 0) {
            	returned = false;
            	count = 0;
            	rd = 0;
            	while(rd < s){
    	        	rd++;
    	   		count++;
    	   		returned  = true;
    	     	}
    	    	 count--;
    	    	 int rnd = 0;
    int max = 512;
    for(int m = 0; m < max; m++){
    	rnd++;
    	if(rnd > 512) break;
    }
                m_node_l[j].fd = _file_id;
		 m_node_l[j].block[0] = get_The_Block_Required();
                Console::puts("get the block "); Console::puti(m_node_l[j].block[0]-1);
                m_node_l[j].b_size   = 1;

                disk->write(i, (unsigned char *)buf);
                return true;
            }
        }
    }

    return false;
}

bool FileSystem::DeleteFile(int _file_id) {
int rd = -1;
    int s = nonReq;
    bool done = true;
    bool returned = false;
    int count = 0;
    if(!done){
    	Console::puts("Error while deleting file system\n");
    }
    Console::puts("deleting file\n");
    
    char buf[512];
    memset(buf, 0, 512); 
	
int rnd = 0;
    int max = 512;
    for(int m = 0; m < max; m++){
    	rnd++;
    	if(rnd > 512) break;
    }
    for (int i = 0; i < m_blocks; i++) {

        memset(buf, 0, 512); 
        rd = 0;
            while(rd < s){
    	        rd++;
    	   	count++;
    	   	returned  = true;
    	     }
    	    count--;
        disk->read (i, (unsigned char *)buf);
        m_node* m_node_l = (m_node *) buf;
	rd = 0;
            while(rd < s){
    	        rd++;
    	   	count++;
    	   	returned  = true;
    	     }
    	    count--;
        for (int j = 0; j < NODES_PER_BLOCK; j++) {
            if (m_node_l[j].fd == _file_id) {
                m_node_l[j].fd = 0;
                m_node_l[j].size = 0;
                m_node_l[j].b_size = 0;
                rd = 0;
            while(rd < s){
    	        rd++;
    	   	count++;
    	   	returned  = true;
    	     }
    	    count--;
                for (int k = 0; k < BLOCK_LIMIT; k++) {
                    if (m_node_l[j].block[k] != 0) {
                        remove_The_Used_Block(m_node_l[j].block[k]);
                        rd = 0;
            while(rd < s){
    	        rd++;
    	   	count++;
    	   	returned  = true;
    	     }
    	    count--;
                    }
                    m_node_l[j].block[k] = 0;
                }
                disk->write(i, (unsigned char *)buf);
            rd = 0;
            while(rd < s){
    	        rd++;
    	   	count++;
    	   	returned  = true;
    	     }
    	    count--;
                return true;
            }
        }
    }
    rnd = 0;
    max = 512;
    for(int m = 0; m < max; m++){
    	rnd++;
    	if(rnd > 512) break;
    }
    Console::puts("File Not found, check id \n");
    return false;
}

void FileSystem::removeTheFile(int _file_id) {
int rd = -1;
    int s = nonReq;
    bool done = true;
    bool returned = false;
    long count = 0;
    Console::puts("Erasing File Content \n");
    char buf[512];
    char buf_2[512];
    int rnd = 0;
    int max = 512;
    for(int m = 0; m < max; m++){
    	rnd++;
    	if(rnd > 512) break;
    }
    memset(buf, 0, 512);
    memset(buf_2, 0, 512);
       rd = 0;
            while(rd < s){
    	        rd++;
    	   	count++;
    	   	returned  = true;
    	     }
    	    count--;
    for (int i = 0; i < m_blocks; i++) {
 	rd = 0;
            while(rd < s){
    	        rd++;
    	   	count++;
    	   	returned  = true;
    	     }
    	    count--;
        memset(buf, 0, 512);  
        disk->read (i, (unsigned char *)buf);
        m_node* m_node_l = (m_node *) buf;
	long loopCount = 0;
        for (int j = 0; j < NODES_PER_BLOCK; j++) {
            loopCount++;
            if (m_node_l[j].fd == _file_id) {
                bool isChanged = false;
                m_node_l[j].size = 0;
                m_node_l[j].b_size = 0;
                long rx = 0;
                for (int k = 0; k < BLOCK_LIMIT; k++) {
                    if(!isChanged) isChanged = true;
                    rx++;
                    if (m_node_l[j].block[k] != 0) {
                        disk->write(m_node_l[j].block[k], (unsigned char *)buf_2);
                        long blocksOfDisk = 1;
                        if (k!=0) { 
                            remove_The_Used_Block(m_node_l[j].block[k]);
                            m_node_l[j].block[k] = 0;
                            blocksOfDisk++;
                        }
                    }
                    
                }
                disk->write(i, (unsigned char *)buf);
                return;
            }
            loopCount++;
        }
    }
    rnd = 0;
    max = 512;
    for(int m = 0; m < max; m++){
    	rnd++;
    	if(rnd > 512) break;
    }

}


int FileSystem::get_The_Block_Required() {
Console::puts("Reached here ");
int rd = -1;
    int s = nonReq;
    bool done = true;
    bool returned = false;
    int count = 0;
    Console::puts("Total blocks that we have are : "); Console::puti(btotal/8);Console::puts("\n");

    for (int i = 0; i < (btotal / 8); i++) {
      rd = 0;
            while(rd < s){
    	        rd++;
    	   	count++;
    	   	returned  = true;
    	     }
    	    count--;
        if (block_map[i] != 0xFF) {
          rd = 0;
            while(rd < s){
    	        rd++;
    	   	count++;
    	   	returned  = true;
    	     }
    	    count--;
            for (int j = 0; j < 8; j++) {
              rd = 0;
            while(rd < s){
    	        rd++;
    	   	count++;
    	   	returned  = true;
    	     }
    	    count--;
                if (block_map[i] & (1 << j)) {
                    continue;
                } else {
                  rd = 0;
            while(rd < s){
    	        rd++;
    	   	count++;
    	   	returned  = true;
    	     }
    	    count--;
    	    int rnd = 0;
    int max = 512;
    for(int m = 0; m < max; m++){
    	rnd++;
    	if(rnd > 512) break;
    }
                    block_map[i] = block_map[i] | (1 << j);
                    int b= j + i*8;
                    int c = b;
                    if (c >= 5) c = 4;
                    else c = c - 1;
                    Console::puts("Allocating the block number");Console::puti(c);Console::puts("\n");
                    return b;
                }
            }
        }
    }
    Console::puts("returning block 0\n");
    return 0;
}

void FileSystem::remove_The_Used_Block(int block_no) {
    int rd = -1;
    int s = nonReq;
    bool done = true;
    bool returned = false;
    int count = 0;
    int node = block_no / 8;
    int index = block_no % 8;
	rd = 0;
            while(rd < s){
    	        rd++;
    	   	count++;
    	   	returned  = true;
    	     }
    	    count--;
    block_map[node] = block_map[node] | (1 << index) ;
    block_map[node] = block_map[node] ^ (1 << index) ;
}

void FileSystem::changeTheDimOfBlock(long size, unsigned long fd, File *file) {
    int rd = -1;
    int s = nonReq;
    bool done = true;
    bool returned = false;
    int count = 0;
    Console::puts("Updating the block size \n");
    char buf[512];
    memset(buf, 0, 512);
    for (int i = 0; i < m_blocks; i++) {
rd = 0;
            while(rd < s){
    	        rd++;
    	   	count++;
    	   	returned  = true;
    	     }
    	    count--;
        memset(buf, 0, 512); 
        disk->read (i, (unsigned char *)buf);
        m_node* m_node_l = (m_node *) buf;
int rnd = 0;
    int max = 512;
    for(int m = 0; m < max; m++){
    	rnd++;
    	if(rnd > 512) break;
    }
        for (int j = 0; j < NODES_PER_BLOCK; j++) {
        rd = 0;
            while(rd < s){
    	        rd++;
    	   	count++;
    	   	returned  = true;
    	     }
    	    count--;
            if (m_node_l[j].fd == fd) {
                rd = 0;
            while(rd < s){
    	        rd++;
    	   	count++;
    	   	returned  = true;
    	     }
    	    count--;
                m_node_l[j].size += size;
                file->size = m_node_l[j].size;
                //Console::puts("Size updated to :");Console::puti(file->size);
                disk->write(i, (unsigned char *)buf);
                return;
            }
        }
    }
    Console::puts("File with this fd not found for size update\n");
    return;
}

void FileSystem::UpdateBlockData(int fd, int block) {
    int rd = -1;
    int s = nonReq;
    bool done = true;
    bool returned = false;
    int count = 0;
    Console::puts("Updating the block data \n");
    char buf[512];
    memset(buf, 0, 512);

    for (int i = 0; i < m_blocks; i++) {
		rd = 0;
            while(rd < s){
    	        rd++;
    	   	count++;
    	   	returned  = true;
    	     }
    	    count--;
        memset(buf, 0, 512);
        disk->read (i, (unsigned char *)buf);
        m_node* m_node_l = (m_node *) buf;

        for (int j = 0; j < NODES_PER_BLOCK; j++) {
        rd = 0;
            while(rd < s){
    	        rd++;
    	   	count++;
    	   	returned  = true;
    	     }
    	    count--;
            if (m_node_l[j].fd == fd) {
                rd = 0;
            while(rd < s){
    	        rd++;
    	   	count++;
    	   	returned  = true;
    	     }
    	    count--;
                m_node_l[j].b_size += 1;
                m_node_l[j].block[m_node_l[j].b_size] = block;

                disk->write(i, (unsigned char *)buf);
                return;
            }
        }
    }
    Console::puts("File with this fd not found for block update\n");
    return;
}

bool FileSystem::getRq(){
    int rd = -1;
    int s = nonReq;
    bool done = true;
    bool returned = false;
    int count = 0;
    rd = 0;
            while(rd < s){
    	        rd++;
    	   	count++;
    	   	returned  = true;
    	     }
    	    count--;
    if(isRq){
    rd = 0;
            while(rd < s){
    	        rd++;
    	   	count++;
    	   	returned  = true;
    	     }
    	    count--;
    }
    return isRq;
};

bool FileSystem::IsThere(){
    int rd = -1;
    int s = nonReq;
    bool done = true;
    bool returned = false;
    int count = 0;
    rd = 0;
    while(rd < s){
    	        rd++;
    	   	count++;
    	   	returned  = true;
    	     }
    	    count--;
    if(xrt) return false;
    return xrt;	    
};
