/*
     File        : blocking_disk.c

     Author      : 
     Modified    : 

     Description : 

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "assert.H"
#include "utils.H"
#include "console.H"
#include "blocking_disk.H"
#include "scheduler.H"
#include "mem_pool.H"
#include "thread.H" 

extern MemPool * MEMORY_POOL;
extern Scheduler * SYSTEM_SCHEDULER;
/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR */
/*--------------------------------------------------------------------------*/

BlockingDisk::BlockingDisk(DISK_ID _disk_id, unsigned int _size) 
  : SimpleDisk(_disk_id, _size) {
    saved_request = (rw_request_node*)(MEMORY_POOL->allocate(sizeof(rw_request_node)));
    Console::puts("BlockingDisk constructed.\n");
}

/*--------------------------------------------------------------------------*/
/* SIMPLE_DISK FUNCTIONS */
/*--------------------------------------------------------------------------*/
void BlockingDisk::wait_until_ready(){
  while(!is_ready()){
      SYSTEM_SCHEDULER->resume(Thread::CurrentThread());                  //Add the thread to teh ready queue again
      Console::puts("Device is not ready, voluntarily yielding thread\n ");  
      SYSTEM_SCHEDULER->yield();
  }
}

void BlockingDisk::read(unsigned long _block_no, unsigned char * _buf) {
  issue_operation(DISK_OPERATION::READ, _block_no);
  wait_until_ready();

}


void BlockingDisk::write(unsigned long _block_no, unsigned char * _buf) {
  // -- REPLACE THIS!!!
  SimpleDisk::write(_block_no, _buf);
}
