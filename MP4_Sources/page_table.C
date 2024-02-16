/*
    File: page_table.C
    Author: Rajendra Thottempudi
            Department of Computer Science
            Texas A&M University
    Date  : 09/30/2022
*/

#include "assert.H"
#include "exceptions.H"
#include "console.H"
#include "paging_low.H"
#include "page_table.H"

PageTable * PageTable::current_page_table = NULL;
unsigned int PageTable::paging_enabled = 0;
ContFramePool * PageTable::kernel_mem_pool = NULL;
ContFramePool * PageTable::process_mem_pool = NULL;
unsigned long PageTable::shared_size = 0;
VMPool * PageTable::vm_pool_list[];



void PageTable::init_paging(ContFramePool * _kernel_mem_pool,
                            ContFramePool * _process_mem_pool,
                            const unsigned long _shared_size)
{
    kernel_mem_pool = _kernel_mem_pool;
    process_mem_pool = _process_mem_pool;
    shared_size = shared_size;
    Console::puts("Initialized Paging System for the process\n");
}

PageTable::PageTable()
{
    //Assigning a frame for this process's page directory
    page_directory = (unsigned long *) ((kernel_mem_pool->get_frames(1)) * PAGE_SIZE);
    unsigned long *page_tble = (unsigned long *) ((kernel_mem_pool->get_frames(1)) * PAGE_SIZE);
    unsigned long adres = 0;
    unsigned long page_temp = 0;
    unsigned int pgDirIdx, pgTbleIdx;
    for ( pgTbleIdx = 0; pgTbleIdx < 1024; pgTbleIdx++)
    {
        page_tble[pgTbleIdx] = adres | 3; 
        adres = adres + 4096;	
        page_temp++;
    }
    page_directory[0] = (unsigned long) page_tble;
    //marking it as present
    page_directory[0] = page_directory[0] | 3; 

    // marking rest of the page table entries as absent
    while(pgDirIdx < 1024){
    	page_directory[pgDirIdx] = 0 | 2;   
      	page_temp++;
      	pgDirIdx++;
    }
    
    page_directory[1023] = (unsigned long) page_directory | 3;
    Console::puts("We have initialized the page table object with the respected variables\n");
}


void PageTable::load()
{
    current_page_table = this;
    //adding the corresponding page directory into the CR3 register for context
    write_cr3( (unsigned long) page_directory ); 
    Console::puts("Loaded page table with the corresponding context\n");
}

void PageTable::enable_paging()
{
    // Enabling the paging on the system
    write_cr0 (read_cr0() | 0x80000000);
    // Changing the state of the paging_enabled variable to 1 to indicate the enabled state
    paging_enabled = 1;
    Console::puts("Enabled paging in the system\n");
}

void PageTable::handle_fault(REGS * _r)
{
    unsigned long   page_fault_adres = read_cr2();
    unsigned long * page_directory = ((unsigned long *) read_cr3());
    unsigned long * page_directory_entry;
    unsigned long * page_table_entry;
    unsigned long   pdDirIdx = page_fault_adres >> 22;
    unsigned long   pgTbleIdx;
    unsigned long   nxtIdxPgTble;
    unsigned long * pgTble;
    unsigned long * nxtPgTble;
    unsigned long * pgTbleAdres;
    unsigned long   page_entry;
    unsigned long * page;

    unsigned int flag = 0;
    unsigned int pool_count = 0;
    int i,j;
    for (j = 0; j < 512; j++)
    {
        if (vm_pool_list[j] != 0)
            pool_count++;
    }

	for (i = 0; i < pool_count; i++) 
    {
		if (vm_pool_list[i] -> is_legitimate(page_fault_adres) == true) 
        {
			flag = 1;
			break;
		}
	}
	if (flag == 0)
    {
		Console::puts("Error, Invalid Address\n");
		assert(false);
	}

    if (_r->err_code & 0xFFFFFFFE) 
    {
        page_directory_entry = (unsigned long *)((0xFFFFF000 | (page_fault_adres >> 20)) & 0xFFFFFFFC);
        if ((*page_directory_entry & 0x1) == 0) 
        {
            pgTble = (unsigned long *) ((process_mem_pool->get_frames(1)) * PAGE_SIZE); 
            *page_directory_entry = ((unsigned long) pgTble) | 3;
        }

        page_table_entry = (unsigned long *)((0xFFC00000 | (((page_fault_adres >> 10) & 0x003FF000) | ((page_fault_adres >> 10) & 0x00000FFC))) & 0xFFFFFFFC);
        if ((*page_table_entry & 0x1) == 0)
        {
            page = (unsigned long *) ((process_mem_pool->get_frames(1)) * PAGE_SIZE);
            *page_table_entry = ((unsigned long) page) | 3;
        }

    }
    Console::puts("The page table fault has been handled\n");
}

void PageTable::register_pool(VMPool * _vm_pool)
{
    int k,j;
    for (k = 0; k < 512; k++)
    {	
    	int j = j + 1;
        if (vm_pool_list[k] == 0)
            break;
    }
    vm_pool_list[k] = _vm_pool;
    Console::puts("registered the VM pool\n");
}

void PageTable::free_page(unsigned long _page_no) 
{
    unsigned long * pgTbleEntry;
    unsigned long   frameNo;
    unsigned long   nxtFrame;
    pgTbleEntry = (unsigned long *)((0xFFC00000 | (((_page_no >> 10) & 0x003FF000) | ((_page_no >> 10) & 0x00000FFC))) & 0xFFFFFFFC);
    if ((*pgTbleEntry & 0x1))
    {
        frameNo = *pgTbleEntry >> 12;
        nxtFrame = frameNo;
        process_mem_pool->release_frames(frameNo);
        *pgTbleEntry = 0 | 2;
    }

    write_cr3((unsigned long) page_directory);
    Console::puts("freed the page\n");
}
