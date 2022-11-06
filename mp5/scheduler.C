/*
 File: scheduler.C
 
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

#include "scheduler.H"
#include "thread.H"
#include "console.H"
#include "utils.H"
#include "assert.H"
#include "simple_keyboard.H"
#include "mem_pool.H"

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
extern MemPool * MEMORY_POOL;
/*--------------------------------------------------------------------------*/
/* METHODS FOR CLASS   S c h e d u l e r  */
/*--------------------------------------------------------------------------*/

Scheduler::Scheduler() {
  //assert(false);
  head=nullptr;
  tail=nullptr;
  Console::puts("Constructed Scheduler.\n");
}

void Scheduler::yield() {
  Console::puts("Virtual Scheduler::yield().... \n");
  //assert(false);
}

void Scheduler::resume(Thread * _thread) {
  Console::puts("Virtual Scheduler::resume().... \n");
  //assert(false);
}

void Scheduler::add(Thread * _thread) {
  Console::puts("Virtual Scheduler::resume().... \n");
  //assert(false);
}

void Scheduler::terminate(Thread * _thread) {
  Console::puts("Virtual Scheduler::terminate().... \n");
  //assert(false);
}

FIFOScheduler::FIFOScheduler(){
   Console::puts("Constructed FIFOScheduler.\n"); 
}

void FIFOScheduler::yield(){
  Console::puts("----FIFOScheduler::yield()----\n");
  Console::puts("Disable Interrupts....\n");
  Machine::disable_interrupts();
  tcb* next=head;
  if(next==nullptr){
    Console::puts("The ready queue is empty!");
    assert(false);
  }
  if(head->next==nullptr){
    Console::puts("Notice:Next thread is the last thread waiting for service!\n");
  }
  head=head->next;
  Machine::enable_interrupts();
  Console::puts("Enable Interrupts....\n");
  Thread::dispatch_to(next->thread);
  Console::puts("Dispatching Tread to:");
  Console::puti(next->thread->ThreadId()+1);
  Console::puts("....\n");
  Console::puts("----FIFOScheduler::yield() Successfully----\n");
  MEMORY_POOL->release((unsigned long)next); 
}

void FIFOScheduler::resume(Thread* _thread){
  Console::puts("----FIFOScheduler::resume()----\n");
  add(_thread);
  Console::puts("Resume a new thread:\n");
  Console::puti(_thread->ThreadId()+1);
  Console::puts("....\n");
  Console::puts("----FIFOScheduler::resume() Successfully----\n");
}

void FIFOScheduler::add(Thread* _thread){
  Console::puts("----FIFOScheduler::add()----\n");
  tcb * last=(tcb*)(MEMORY_POOL->allocate(sizeof(tcb)));
  last->thread=_thread;
  last->next=nullptr;
  if(head==nullptr&&tail==nullptr) {
    head=last;
    tail=last;
  }
  else{
    tail->next=last;
    tail=last;
  }
  Console::puts("Add a new thread:");
  Console::puti(_thread->ThreadId()+1);
  Console::puts("....\n");
  Console::puts("----FIFOScheduler::add() Successfully----\n");
}
