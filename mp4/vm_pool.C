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
    no_of_allocated++;
    free_list[0].base_addr=base_address+Machine::PAGE_SIZE;
    free_list[0].size=size-Machine::PAGE_SIZE;;
    no_of_freed++;
    set_first_page=false;
    for(int i=0;i<256;i++){
        allocated_list[i].base_addr=0;
        allocated_list[i].size=0;
        free_list[i].base_addr=0;
        free_list[i].size=0;
    }
    Console::puts("Constructed VMPool object.\n");
}

unsigned long VMPool::allocate(unsigned long _size) {
    assert(false);
    Console::puts("Allocated region of memory.\n");
}

void VMPool::release(unsigned long _start_address) {
    assert(false);
    Console::puts("Released region of memory.\n");
}

bool VMPool::is_legitimate(unsigned long _address) {
    assert(false);
    Console::puts("Checked whether address is part of an allocated region.\n");
}

