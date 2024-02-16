/*
    File: kernel.C

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 2021/11/28


    This file has the main entry point to the operating system.

    MAIN FILE FOR MACHINE PROBLEM "FILE SYSTEM"

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

#define MB * (0x1 << 20)
#define KB * (0x1 << 10)

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "machine.H"         /* LOW-LEVEL STUFF   */
#include "console.H"
#include "gdt.H"
#include "idt.H"             /* EXCEPTION MGMT.   */
#include "irq.H"
#include "exceptions.H"     
#include "interrupts.H"

#include "assert.H"

#include "simple_timer.H"    /* TIMER MANAGEMENT  */

#include "frame_pool.H"      /* MEMORY MANAGEMENT */
#include "mem_pool.H"

#include "simple_disk.H"     /* DISK DEVICE */

#include "file_system.H"     /* FILE SYSTEM */
#include "file.H"

/*--------------------------------------------------------------------------*/
/* MEMORY MANAGEMENT */
/*--------------------------------------------------------------------------*/

/* -- A POOL OF FRAMES FOR THE SYSTEM TO USE */
FramePool * SYSTEM_FRAME_POOL;

/* -- A POOL OF CONTIGUOUS MEMORY FOR THE SYSTEM TO USE */
MemPool * MEMORY_POOL;

typedef long unsigned int size_t;

//replace the operator "new"
void * operator new (size_t size) {
    unsigned long a = MEMORY_POOL->allocate((unsigned long)size);
    return (void *)a;
}

//replace the operator "new[]"
void * operator new[] (size_t size) {
    unsigned long a = MEMORY_POOL->allocate((unsigned long)size);
    return (void *)a;
}

//replace the operator "delete"
void operator delete (void * p, size_t s) {
    MEMORY_POOL->release((unsigned long)p);
}


//replace the operator "delete[]"
void operator delete[] (void * p) {
    MEMORY_POOL->release((unsigned long)p);
}

/*--------------------------------------------------------------------------*/
/* DISK */
/*--------------------------------------------------------------------------*/

/* -- A POINTER TO THE SYSTEM DISK */
SimpleDisk * SYSTEM_DISK;

#define SYSTEM_DISK_SIZE (10 MB)

/*--------------------------------------------------------------------------*/
/* FILE SYSTEM */
/*--------------------------------------------------------------------------*/

/* -- A POINTER TO THE SYSTEM FILE SYSTEM */
FileSystem * FILE_SYSTEM;

/*--------------------------------------------------------------------------*/
/* CODE TO EXERCISE THE FILE SYSTEM */
/*--------------------------------------------------------------------------*/
void exercise_file_system(FileSystem * _file_system) {
    
    const char * STRING1 = "01234567890123456789";
    const char * STRING2 = "abcdefghijabcdefghij";
    
    /* -- Create two files -- */
    
    assert(_file_system->CreateFile(1));
    assert(_file_system->CreateFile(2));
    
    /* -- "Open" the two files -- */
    
    File * file1 = _file_system->LookupFile(1);
    assert(file1 != NULL);
    
    File * file2 = _file_system->LookupFile(2);
    assert(file2 != NULL);

    char test[600];
    for (int i = 0; i < 600; i++) {
        test[i] = 't';
     }
    
    /* -- Write into File 1 -- */
    file1->Rewrite();
    file1->Write(20, STRING1);
    file1->Write(600, test);
    
    /* -- Write into File 2 -- */
    
    file2->Rewrite();
    file2->Write(20, STRING2);
    
    /* -- "Close" files -- */
    delete file1;
    delete file2;
    
    /* -- "Open files again -- */
    file1 = _file_system->LookupFile(1);
    file2 = _file_system->LookupFile(2);
    
    /* -- Read from File 1 and check result -- */
    file1->Reset();
    char result1[30];
    assert(file1->Read(20, result1) == 20);
    for(int i = 0; i < 20; i++) {
        assert(result1[i] == STRING1[i]);
    }
    memset(test, 0, 600);
    assert (file1->Read(600, test) == 600);
    for (int j = 0; j < 600; j++) {
        
        assert(test[j] == 't');
    } 
    
    
    /* -- Read from File 2 and check result -- */
    file2->Reset();
    char result2[30];
    assert(file2->Read(20, result2) == 20);
    for(int i = 0; i < 20; i++) {
        assert(result2[i] == STRING2[i]);
    }
    
    /* -- "Close" files again -- */
    delete file1;
    delete file2;
    
    /* -- Delete both files -- */
    assert(_file_system->DeleteFile(1));
    assert(_file_system->DeleteFile(2));
    
}

/*--------------------------------------------------------------------------*/
/* MAIN ENTRY INTO THE OS */
/*--------------------------------------------------------------------------*/

int main() {

    GDT::init();
    Console::init();
    IDT::init();
    ExceptionHandler::init_dispatcher();
    IRQ::init();
    InterruptHandler::init_dispatcher();

    Console::output_redirection(true);

    /* -- EXAMPLE OF AN EXCEPTION HANDLER -- */

    class DBZ_Handler : public ExceptionHandler {
      public:
      virtual void handle_exception(REGS * _regs) {
        Console::puts("DIVISION BY ZERO!\n");
        for(;;);
      }
    } dbz_handler;

    ExceptionHandler::register_handler(0, &dbz_handler);
    FramePool system_frame_pool;
    SYSTEM_FRAME_POOL = &system_frame_pool;
   
    /* ---- Create a memory pool of 256 frames. */
    MemPool memory_pool(SYSTEM_FRAME_POOL, 256);
    MEMORY_POOL = &memory_pool;

    SimpleTimer timer(100); /* timer ticks every 10ms. */
    InterruptHandler::register_handler(0, &timer);
    /* The Timer is implemented as an interrupt handler. */

    /* -- DISK DEVICE -- */

    SYSTEM_DISK = new SimpleDisk(DISK_ID::MASTER, SYSTEM_DISK_SIZE);

    FILE_SYSTEM = new FileSystem();

    Console::puts("Hello World!\n");
       
    assert(FILE_SYSTEM->Format(SYSTEM_DISK, (128 KB)));
    
    assert(FILE_SYSTEM->Mount(SYSTEM_DISK)); // 'connect' disk to file system.

    for(int j = 0;; j++) {
        exercise_file_system(FILE_SYSTEM);
    }

    /* -- AND ALL THE REST SHOULD FOLLOW ... */
 
    assert(false); /* WE SHOULD NEVER REACH THIS POINT. */

    return 1;
}
