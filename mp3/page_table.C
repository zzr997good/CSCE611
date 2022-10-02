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



void PageTable::init_paging(ContFramePool * _kernel_mem_pool,
                            ContFramePool * _process_mem_pool,
                            const unsigned long _shared_size)
{
   //assert(false);
   kernel_mem_pool=_kernel_mem_pool;
   process_mem_pool=_process_mem_pool;
   shared_size=_shared_size;
   Console::puts("Initialized Paging System\n");
}

PageTable::PageTable()
{
   //assert(false);
   page_directory=(unsigned long *)(kernel_mem_pool->get_frames(1)<<PHYSICAL_ADDRESS_START);
   unsigned long* page_table=(unsigned long *)(kernel_mem_pool->get_frames(1)<<PHYSICAL_ADDRESS_START);//we turn this to unsigned long* cus each PTE is 4B and unsigned long is also 4B
   unsigned long physical_address=0;
   //Fill the first page table page to mark first 4MB in physical memory as "present"
   for(int i=0;i<Machine::PT_ENTRIES_PER_PAGE;i++){
      page_table[i]=physical_address|S_W_P;
      physical_address+=Machine::PAGE_SIZE;
   }
   //Fill the first PDE and mark otehr PDE as "not present"
   page_directory[0]=((unsigned long)page_table)|S_W_P;
   for(int i=1;i<Machine::PT_ENTRIES_PER_PAGE;i++){
      page_directory[i]=0|S_W_NP;
   }

   Console::puts("Constructed Page Table object\n");
}


void PageTable::load()
{
   //assert(false);
   current_page_table=this;
   write_cr3((unsigned long)page_directory);
   Console::puts("Loaded page table\n");
}

void PageTable::enable_paging()
{
   assert(false);
   Console::puts("Enabled paging\n");
}

void PageTable::handle_fault(REGS * _r)
{
  assert(false);
  Console::puts("handled page fault\n");
}

