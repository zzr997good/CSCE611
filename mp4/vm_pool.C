/*
 File: vm_pool.C
 
 Author:
 Date  :
 
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "vm_pool.H"
#include "console.H"
#include "utils.H"
#include "assert.H"
#include "simple_keyboard.H"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* METHODS FOR CLASS   V M P o o l */
/*--------------------------------------------------------------------------*/

VMPool::VMPool(unsigned long  _base_address,
               unsigned long  _size,
               ContFramePool *_frame_pool,
               PageTable     *_page_table) {
    //assert(false);
    base_address=_base_address;
    size=_size;
    frame_pool=_frame_pool;
    page_table=_page_table;
    no_of_allocated=0;
    no_of_freed=0;
    set_first_page=false;
    if(size<Machine::PAGE_SIZE){
        Console::puts("VMPool Initialization FAILED:VM pool size less than a page!.\n");
    	assert(false);
    }
    page_table->register_pool(this);
    //Store allocated info in the first page of VMpool
    allocated_list=(region_node *)base_address;
    free_list=((region_node*)base_address)+256; //1 PAGE=4096B, 1 region_node=8B, 1PAGE=512 region_node, 1 list= 256 region_node
    //Allocate the first page to store allocated info
    set_first_page=true;
    allocated_list[0].base_addr=base_address;
    allocated_list[0].size=Machine::PAGE_SIZE;
    free_list[0].base_addr=base_address+Machine::PAGE_SIZE;
    free_list[0].size=size-Machine::PAGE_SIZE;
    set_first_page=false;

    no_of_allocated++;
    no_of_freed++;
    for(int i=0;i<256;i++){
        allocated_list[i].base_addr=0;
        allocated_list[i].size=0;
        free_list[i].base_addr=0;
        free_list[i].size=0;
    }
    Console::puts("Constructed VMPool object.\n");
}

unsigned long VMPool::allocate(unsigned long _size) {
    //assert(false);
    //Allocate multiples of pages
    unsigned long _size_mul_of_pages=(_size+Machine::PAGE_SIZE-1)/Machine::PAGE_SIZE*Machine::PAGE_SIZE;
    bool found=false;
    int index;
    for(unsigned long i=0;i<no_of_freed;i++){
        if(free_list[i].size>=_size_mul_of_pages){
            index=i;
            found=true;
            break;
        }
    }
    if(found){
        allocated_list[no_of_allocated].base_addr=free_list[index].base_addr;
        allocated_list[no_of_allocated].size=_size_mul_of_pages;
        no_of_allocated++;

        //Split the free list node
        free_list[index].base_addr=free_list[index].base_addr+_size_mul_of_pages;
        free_list[index].size=free_list[index].size-_size_mul_of_pages;
        Console::puts("Allocated region of memory.\n");
        return allocated_list[no_of_allocated-1].base_addr;
    }
    else{
        Console::puts("Memory not enough in pool.\n");
        return 0;
    }
}

void VMPool::release(unsigned long _start_address) {
    //assert(false);
    bool found=false;
    int index;
    for(unsigned long i=0;i<no_of_allocated;i++){
        if(allocated_list[i].base_addr==_start_address){
            index=i;
            found=true;
            break;
        }
    }
    if(!found||allocated_list[index].size==0){
        Console::puts("This address is not allocated.\n");
        assert(false);
    }
    else{
        free_list[no_of_freed].base_addr=allocated_list[index].base_addr;
        free_list[no_of_freed].size=allocated_list[index].size;
        no_of_freed++;
        allocated_list[index].base_addr=0;
        allocated_list[index].size=0;
        unsigned long start_page_no=(_start_address&0xFFFFF000)>>12;
        unsigned long total_pages=free_list[no_of_freed-1].size/Machine::PAGE_SIZE;
        for(int i=0;i<total_pages;i++) page_table->free_page(start_page_no+i);
        Console::puts("Released region of memory.\n");
    }
}

bool VMPool::is_legitimate(unsigned long _address) {
    //assert(false);
    Console::puts("Checked whether address is part of an allocated region.\n");
        if((set_first_page == true) && (no_of_allocated == 0)&&_address>=base_address&&_address<base_address+Machine::PAGE_SIZE)                       //Only passing true legitimacy check if it's the first entry memory access
    {
    	return true;
    }
    
    for(unsigned long i = 0; i<no_of_allocated; i++)                                            //Check all the allocated memory regions
    {
    	if((allocated_list[i].base_addr <= _address) && (_address < (allocated_list[i].base_addr + allocated_list[i].size)))
    	{
    		return true;
    	}
    }
    return false;
}

