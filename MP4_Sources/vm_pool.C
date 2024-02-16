/*
 File: vm_pool.C
 
 Author: Rajendra Thottempudi
 Date  : 10/20/2022
 
 */
#include "vm_pool.H"
#include "console.H"
#include "utils.H"
#include "assert.H"
#include "simple_keyboard.H"
#include "page_table.H"

/*--------------------------------------------------------------------------*/
/* METHODS FOR CLASS   V M P o o l */
/*--------------------------------------------------------------------------*/

VMPool::VMPool(unsigned long  _base_address,
               unsigned long  _size,
               ContFramePool *_frame_pool,
               PageTable     *_page_table) 
{
    int i;
    base_adres = _base_address;
    size = _size;
    frame_pool = _frame_pool;
    page_table = _page_table;
    page_table -> register_pool(this);

    memory_region * region_list = (memory_region *)base_adres;
    region_list[0].start_adres = _base_address;
    region_list[0].size = Machine::PAGE_SIZE;
    for (i = 1; i < 512; i++)
    {
        region_list[i].start_adres = 0;
        region_list[i].size = 0;
    }

    Console::puts("Constructed VMPool object.\n");
}

unsigned long VMPool::allocate(unsigned long _size) 
{
    int i,j;
    j = 0;
    int allocated_size = Machine::PAGE_SIZE;
    
    if (_size > Machine::PAGE_SIZE)
        allocated_size = Machine::PAGE_SIZE * (_size/Machine::PAGE_SIZE) + ((_size % Machine::PAGE_SIZE == 0) ? 0 : Machine::PAGE_SIZE);

    memory_region * region_list = (memory_region *)base_adres;
    for (i = 1; i < 512; i++)
    {
        if (region_list[i].size == 0)
        {
            region_list[i].start_adres = region_list[i-1].start_adres + region_list[i-1].size;
            region_list[i].size = allocated_size;
            break;
        }
    }

    Console::puts("Allocated region of memory.\n");
    j = i;
    return region_list[j].start_adres;
}

void VMPool::release(unsigned long _start_address)
{
    memory_region * region_list = (memory_region *)base_adres;
    int i,j,k;
    for (i = 1; i < 512; i++)
    {
        if (region_list[i].start_adres == _start_address)
        {
            Console::puts("The address has been found"); Console::puts("\n"); 
            break;
        }

    }
    int x = i;
    for (k = Machine::PAGE_SIZE; k <= region_list[x].size; k += Machine::PAGE_SIZE)
    page_table -> free_page(region_list[x].start_adres + k);

    while (x < 511)
    {
        if (region_list[x + 1].size != 0)
        {
            region_list[x].start_adres = region_list[x + 1].start_adres;
            region_list[x].size = region_list[x + 1].size;
        }
        else
        {
            region_list[x].start_adres = 0;
            region_list[x].size = 0;
        }
        x++;
    }

    Console::puts("Released region of memory.\n");
}

bool VMPool::is_legitimate(unsigned long _address) 
{
    if (_address == base_adres)
    {
        return true;
    }
    int k,i;
    bool flag = false;
    memory_region * region_list = (memory_region *) (base_adres);
	
    for (k = 0; k < 512; k++,i++)
    {
        if ((_address >= region_list[k].start_adres) && (_address <= region_list[k].start_adres + region_list[k].size))
        {
            flag = true;
            break;
        }
    }

    return flag;
    
    Console::puts("Checked whether address is part of an allocated region.\n");
}

