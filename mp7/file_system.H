/* 
    File: file_system.H

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 21/11/28

    Description: Simple File System.
    

*/

#ifndef _FILE_SYSTEM_H_ // include file only once
#define _FILE_SYSTEM_H_

#define MB * (0x1 << 20)

#define BLOCK_SIZE 512
#define BLOCK_LIMIT 16
#define DISK_SIZE   (5 MB)
#define MAX_BLOCKS (DISK_SIZE / BLOCK_SIZE)
/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/
#include "file.H"
#include "simple_disk.H"

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

class FileSystem;

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

class Inode
{
  friend class FileSystem; // The inode is in an uncomfortable position between
  friend class File;       // File System and File. We give both full access
                           // to the Inode.

private:
  long id; // File "name"
  long xr;
  int req;
  unsigned long nonreq;

  /* You will need additional information in the inode, such as allocation 
     information. */

  FileSystem *fs; // It may be handy to have a pointer to the File system.
                  // For example when you need a new block or when you want
                  // to load or save the inode list. (Depends on your
                  // implementation.)

  /* You may need a few additional functions to help read and store the 
     inodes from and to disk. */
};

/*--------------------------------------------------------------------------*/
/* FORWARD DECLARATIONS */
/*--------------------------------------------------------------------------*/

/* (none) */

/*--------------------------------------------------------------------------*/
/* F i l e S y s t e m  */
/*--------------------------------------------------------------------------*/

typedef struct tar {
    long fd;
    int isReq;
    bool isRq;
    long block[BLOCK_LIMIT];
    long size;
    bool nonRq; 
    long b_size; 
    int nonReq;     
}m_node;
class File;
class FileSystem
{

  friend class Inode;

private:
  /* -- DEFINE YOUR FILE SYSTEM DATA STRUCTURES HERE. */

  
  unsigned int sizee;

    unsigned char block_map[512];
    unsigned long btotal;
    int isReq;
    bool isRq;
    int nonReq;
    bool nonRq; 

    unsigned long m_blocks;
    long xr;
    int req;
    bool xrt;
    unsigned long nonreq;
    unsigned long m_nodes;   
    unsigned long size;

public:
SimpleDisk *disk;
  FileSystem();
  /* Just initializes local data structures. Does not connect to disk yet. */

  ~FileSystem();
  /* Unmount file system if it has been mounted. */

  bool Mount(SimpleDisk *_disk);
  /* Associates this file system with a disk. Limit to at most one file system per disk.
     Returns true if operation successful (i.e. there is indeed a file system on the disk.) */

  //static bool Format(SimpleDisk *_disk, unsigned int _size);
  bool Format(SimpleDisk *_disk, unsigned int _size);
  /* Wipes any file system from the disk and installs an empty file system of given size. */

//   Inode *LookupFile(int _file_id);
  File *LookupFile(int _file_id);
  /* Find file with given id in file system. If found, return its inode. 
       Otherwise, return null. */

  bool CreateFile(int _file_id);
  /* Create file with given id in the file system. If file exists already,
     abort and return false. Otherwise, return true. */

  bool DeleteFile(int _file_id);
  /* Delete file with given id in the file system; free any disk block occupied by the file. */

   int get_The_Block_Required();
   bool getRq();
   void remove_The_Used_Block(int block_no);
   bool IsThere();
   void changeTheDimOfBlock(long size, unsigned long fd, File *file);
   bool Isxrt();
   void removeTheFile(int _file_id);
   void UpdateBlockData(int fd, int block);
};
#endif
