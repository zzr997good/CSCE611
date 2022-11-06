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
  
}
