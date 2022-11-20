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
extern MemPool *MEMORY_POOL;
//extern EOQTimer *timer;
/*--------------------------------------------------------------------------*/
/* METHODS FOR CLASS   S c h e d u l e r  */
/*--------------------------------------------------------------------------*/

Scheduler::Scheduler()
{
  // assert(false);
  head = nullptr;
  tail = nullptr;
  Console::puts("Constructed Scheduler.\n");
}

void Scheduler::yield()
{
  Console::puts("Virtual Scheduler::yield().... \n");
  // assert(false);
}

void Scheduler::resume(Thread *_thread)
{
  Console::puts("Virtual Scheduler::resume().... \n");
  // assert(false);
}

void Scheduler::add(Thread *_thread)
{
  Console::puts("Virtual Scheduler::resume().... \n");
  // assert(false);
}

void Scheduler::terminate(Thread *_thread)
{
  Console::puts("Virtual Scheduler::terminate().... \n");
  // assert(false);
}

FIFOScheduler::FIFOScheduler()
{
  Console::puts("Constructed FIFOScheduler.\n");
}

void FIFOScheduler::yield()
{
  Console::puts("----FIFOScheduler::yield()----\n");
  Console::puts("Disable Interrupts....\n");
  //Machine::disable_interrupts();
  tcb *next_thread = head;
  if (next_thread == nullptr)
  {
    Console::puts("The ready queue is empty!");
    assert(false);
  }
  if (head->next == nullptr)
  {
    Console::puts("Notice:Next thread is the last thread waiting for service!\n");
  }
  head = head->next;
  Console::puts("Dispatching Thread to:");
  Console::puti(next_thread->thread->ThreadId() + 1);
  Console::puts("\n");

  //Machine::enable_interrupts();
  Console::puts("Enable Interrupts....\n");
  Thread::dispatch_to(next_thread->thread);
  Console::puts("----FIFOScheduler::yield() Successfully----\n");
  MEMORY_POOL->release((unsigned long)next_thread);
}

void FIFOScheduler::resume(Thread *_thread)
{
  Console::puts("----FIFOScheduler::resume()----\n");
  tcb *last = (tcb *)(MEMORY_POOL->allocate(sizeof(tcb)));
  last->thread = _thread;
  last->next = nullptr;
  if (head == nullptr && tail == nullptr)
  {
    head = last;
    tail = last;
  }
  else
  {
    tail->next = last;
    tail = last;
  }
  Console::puts("Resume a thread:");
  Console::puti(_thread->ThreadId() + 1);
  Console::puts("\n");
  Console::puts("----FIFOScheduler::resume() Successfully----\n");
}

void FIFOScheduler::add(Thread *_thread)
{
  Console::puts("----FIFOScheduler::add()----\n");
  tcb *last = (tcb *)(MEMORY_POOL->allocate(sizeof(tcb)));
  last->thread = _thread;
  last->next = nullptr;
  if (head == nullptr && tail == nullptr)
  {
    head = last;
    tail = last;
  }
  else
  {
    tail->next = last;
    tail = last;
  }
  Console::puts("Add a new thread:");
  Console::puti(_thread->ThreadId() + 1);
  Console::puts("\n");
  Console::puts("----FIFOScheduler::add() Successfully----\n");
}

void FIFOScheduler::terminate(Thread *_thread)
{
  Console::puts("----FIFOScheduler::terminate()----\n");
  if (Thread::CurrentThread() == _thread)
  {
    // If the current running thread needs to be terminated
    yield();
  }
  // If the thread needed to be terminated is in ready queue
  else if (head->thread == _thread)
  {
    tcb *curr = head;
    head = head->next;
    Console::puts("Releasing tcb memory....\n");
    MEMORY_POOL->release((unsigned long)curr);
  }
  else
  {
    tcb *prev = head;
    while (prev->next->thread != _thread)
      prev = prev->next;
    tcb *curr = prev->next;
    prev->next = curr->next;
    Console::puts("Releasing tcb memory....\n");
    MEMORY_POOL->release((unsigned long)curr);
  }
  Console::puts("----FIFOScheduler::terminate() Successfully----\n");
}

// RRScheduler::RRScheduler()
// {
//   quantum_fired = false;
//   Console::puts("Constructed RRScheduler.\n");
// }

// void RRScheduler::yield()
// {
//   Console::puts("----RRScheduler::yield()----\n");
//   if (quantum_fired == false)
//   {
//     // This thread yields control to other thread voluntarily
//     Console::puts("yield voluntarily\n");
//   }
//   else
//   {
//     Console::puts("yield forcely by quantum fired\n");
//     quantum_fired = false;
//   }
//   tcb *next_thread = head;
//   if (next_thread == nullptr)
//   {
//     Console::puts("The ready queue is empty!");
//     assert(false);
//   }
//   if (head->next == nullptr)
//   {
//     Console::puts("Notice:Next thread is the last thread waiting for service!\n");
//   }
//   head = head->next;
//   Console::puts("Dispatching Thread to:");
//   Console::puti(next_thread->thread->ThreadId() + 1);
//   Console::puts("\n");
//   Thread::dispatch_to(next_thread->thread);
//   Console::puts("----RRScheduler::yield() Successfully----\n");
//   MEMORY_POOL->release((unsigned long)next_thread);
// }

// void RRScheduler::resume(Thread *_thread)
// {
//   Console::puts("----RRScheduler::resume()----\n");
//   tcb *last = (tcb *)(MEMORY_POOL->allocate(sizeof(tcb)));
//   last->thread = _thread;
//   last->next = nullptr;
//   if (head == nullptr && tail == nullptr)
//   {
//     head = last;
//     tail = last;
//   }
//   else
//   {
//     tail->next = last;
//     tail = last;
//   }
//   Console::puts("Resume a thread:");
//   Console::puti(_thread->ThreadId() + 1);
//   Console::puts("\n");
//   Console::puts("----RRScheduler::resume() Successfully----\n");
// }

// void RRScheduler::add(Thread *_thread)
// {
//   Console::puts("----RRScheduler::add()----\n");
//   tcb *last = (tcb *)(MEMORY_POOL->allocate(sizeof(tcb)));
//   last->thread = _thread;
//   last->next = nullptr;
//   if (head == nullptr && tail == nullptr)
//   {
//     head = last;
//     tail = last;
//   }
//   else
//   {
//     tail->next = last;
//     tail = last;
//   }
//   Console::puts("Add a new thread:");
//   Console::puti(_thread->ThreadId() + 1);
//   Console::puts("\n");
//   Console::puts("----RRcheduler::add() Successfully----\n");
// }

// void RRScheduler::terminate(Thread *_thread)
// {
//   Console::puts("----RRScheduler::terminate()----\n");
//   if (Thread::CurrentThread() == _thread)
//   {
//     // If the current running thread needs to be terminated
//     yield();
//   }
//   // If the thread needed to be terminated is in ready queue
//   else if (head->thread == _thread)
//   {
//     tcb *curr = head;
//     head = head->next;
//     Console::puts("Releasing tcb memory....\n");
//     MEMORY_POOL->release((unsigned long)curr);
//   }
//   else
//   {
//     tcb *prev = head;
//     while (prev->next->thread != _thread)
//       prev = prev->next;
//     tcb *curr = prev->next;
//     prev->next = curr->next;
//     Console::puts("Releasing tcb memory....\n");
//     MEMORY_POOL->release((unsigned long)curr);
//   }
//   Console::puts("----RRScheduler::terminate() Successfully----\n");
// }

// void RRScheduler::quantum_handler()
// {
//   Console::puts("----RRScheduler::quantum_handler()----\n");
// 	Console::puts("50 mS time quantum has passed; now yielding \n");
// 	quantum_fired = true;
// 	resume(Thread::CurrentThread());
// 	yield();
// }
