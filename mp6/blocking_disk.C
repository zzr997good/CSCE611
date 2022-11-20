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
  Console::puts("Device is ready, performing the read operation\n "); 
  int i;
  unsigned short tmpw;
  for (i = 0; i < 256; i++) {
    tmpw = Machine::inportw(0x1F0);
    _buf[i*2]   = (unsigned char)tmpw;
    _buf[i*2+1] = (unsigned char)(tmpw >> 8);
  }

}

void BlockingDisk::write(unsigned long _block_no, unsigned char * _buf) {
  issue_operation(DISK_OPERATION::WRITE,_block_no);
  wait_until_ready();
  Console::puts("Device is ready, performing the write operation\n "); 
  int i; 
  unsigned short tmpw;
  for (i = 0; i < 256; i++) {
    tmpw = _buf[2*i] | (_buf[2*i+1] << 8);
    Machine::outportw(0x1F0, tmpw);
  }
}
