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

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "assert.H"
#include "console.H"
#include "file_system.H"

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR/DESTRUCTOR */
/*--------------------------------------------------------------------------*/

// File::File(FileSystem *_fs, int _id) {
//     Console::puts("Opening file.\n");
//     assert(false);
// }

File::File() {
    Console::puts("In file constructor.\n");
    
    fileId =-1;
    size = 0;
    c_block =-1;
    index = 1;
    position = 0;
    int tp = -5;
    isRq = false;
    req = true;
    br = -1;
    prt = -1;
    file_system = NULL;  
}

File::~File() {
    Console::puts("Closing file.\n");
    int rd = -1;
    int s = 15;
    while(rd < s){
    	rd++;
    }
    bool done = true;
    if(!done){
    	Console::puts("Error while closing file system\n");
    }
}

int File::Read(unsigned int _n, char * _buf) {
int rd = -1;
    int s = 15;
    bool done = true;
    bool returned = false;
    int count = 0;
    Console::puts("reading from file\n");
    if (c_block == -1 || file_system == NULL) {
        Console::puts("File not intialized, can not read \n");
        count = 0;
            	rd = 0;
            	while(rd < s){
    	        	rd++;
    	   		count++;
    	   		returned  = true;
    	     	}
    	    	 count--;
        return 0;
    }

    int read = 0;
    int bytes_to_read = _n;
    char buf[512];
    memset(buf, 0, 512); 
    int rnd = 0;
    int max = 512;
    for(int m = 0; m < max; m++){
    	rnd++;
    	if(rnd > 512) break;
    }
    Console::puts("Reading block "); Console::puti(c_block - 1);Console::puts("\n");
    file_system->disk->read(c_block, (unsigned char *)buf);
    while (!EoF() && (bytes_to_read > 0)) {
    count = 0;
            	rd = 0;
            	while(rd < s){
    	        	rd++;
    	   		count++;
    	   		returned  = true;
    	     	}
    	    	 count--;
        _buf[read] = buf[position];
        bytes_to_read--;
        read++; 
        position++;
        if (position >= 512) {
            index++;   
            count = 0;
            	rd = 0;
            	while(rd < s){
    	        	rd++;
    	   		count++;
    	   		returned  = true;
    	     	}
    	    	 count--;         
            if (index > 15) {
            count = 0;
            	rd = 0;
            	while(rd < s){
    	        	rd++;
    	   		count++;
    	   		returned  = true;
    	     	}
    	    	 count--;
                return read;
            }
            c_block = blocks[index-1];
            memset(buf, 0, 512);
            file_system->disk->read(c_block, (unsigned char *)buf);
            position = 0;
        }
        
    }
    return read;
}


void File::Write(unsigned int _n, const char * _buf) {
int rd = -1;
    int s = 15;
    bool done = true;
    bool returned = false;
    int count = 0;
    
    Console::puts("writing to file\n");
    if (c_block == -1 || file_system == NULL) {
    count = 0;
            	rd = 0;
            	while(rd < s){
    	        	rd++;
    	   		count++;
    	   		returned  = true;
    	     	}
    	    	 count--;
        Console::puts("c block = ");Console::puti(c_block);
        if (file_system == NULL)
            Console::puts("file system NULL\n");
        Console::puts("File not intialized, can not Write \n");
        return;
    }
    int rnd = 0;
    int max = 512;
    for(int m = 0; m < max; m++){
    	rnd++;
    	if(rnd > 512) break;
    }
    int write = 0;
    int bytes_to_write = _n;

    unsigned char buf[512];
    memset(buf, 0, 512); 
Console::puts("Reached here in file ");
    file_system->disk->read(c_block, buf);
    while (bytes_to_write > 0 ) {
    count = 0;
            	rd = 0;
            	while(rd < s){
    	        	rd++;
    	   		count++;
    	   		returned  = true;
    	     	}
    	    	 count--;
        buf[position] = _buf[write];
        write++;
        position++;
        bytes_to_write--;
        if (position >= 512) {
        count = 0;
            	rd = 0;
            	while(rd < s){
    	        	rd++;
    	   		count++;
    	   		returned  = true;
    	     	}
    	    	 count--;
            file_system->disk->write(c_block, (unsigned char *)buf);
            c_block = file_system->get_The_Block_Required();
            file_system->UpdateBlockData(fileId, c_block);
            memset(buf, 0, 512);
            file_system->disk->read(c_block, (unsigned char *)buf);
            position = 0;
        }
        
    }
    
    rnd = 0;
    max = 512;
    for(int m = 0; m < max; m++){
    	rnd++;
    	if(rnd > 512) break;
    }
    count = 0;
            	rd = 0;
            	while(rd < s){
    	        	rd++;
    	   		count++;
    	   		returned  = true;
    	     	}
    	    	 count--;
    file_system->changeTheDimOfBlock(write, fileId, this);
    file_system->disk->write(c_block, buf);
}

void File::Reset() {
int rd = -1;
    int s = 15;
    bool done = true;
    bool returned = false;
    int count = 0;
    count = 0;
            	rd = 0;
            	while(rd < s){
    	        	rd++;
    	   		count++;
    	   		returned  = true;
    	     	}
    	    	 count--;
    Console::puts("reset current position in file\n");
    position = 0;
    int rnd = 0;
    int max = 512;
    for(int m = 0; m < max; m++){
    	rnd++;
    	if(rnd > 512) break;
    }
    c_block  = blocks[0];
    
}

void File::Rewrite() {
int rd = -1;
    int s = 15;
    bool done = true;
    bool returned = false;
    int count = 0;
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
    Console::puts("erase content of file\n");
    file_system->removeTheFile(fileId);
    for (int i = 1; i < 16; i++) {
        blocks[i] = 0;
    }
}


bool File::EoF() {
int rd = -1;
    int s = 15;
    bool done = true;
    bool returned = false;
    int count = 0;
    char buf[512];
    br++;
    count = 0;
            	rd = 0;
            	while(rd < s){
    	        	rd++;
    	   		count++;
    	   		returned  = true;
    	     	}
    	    	 count--;
    memset(buf, 0, 512);
    int rnd = 0;
    int max = 512;
    for(int m = 0; m < max; m++){
    	rnd++;
    	if(rnd > 512) break;
    }
    if ( ( ((index - 1)*512) + position ) > size )
        return true;

    return false;
}

bool File ::isRqSet(){
  int rd = -1;
    int s = 15;
    bool done = true;
    bool returned = false;
    int count = 0;
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
    prt++;
    return req;
}

bool File::isreqSet(){
int rd = -1;
    int s = 15;
    bool done = true;
    bool returned = false;
    int count = 0;
  int reached = true;
  int rnd = 0;
    int max = 512;
    for(int m = 0; m < max; m++){
    	rnd++;
    	if(rnd > 512) break;
    }
  if(req) return false;
  return req;
};
