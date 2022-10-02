#include "assert.H"
#include "exceptions.H"
#include "console.H"
#include "paging_low.H"
#include "page_table.H"

PageTable *PageTable::current_page_table = NULL;
unsigned int PageTable::paging_enabled = 0;
ContFramePool *PageTable::kernel_mem_pool = NULL;
ContFramePool *PageTable::process_mem_pool = NULL;
unsigned long PageTable::shared_size = 0;

void PageTable::init_paging(ContFramePool *_kernel_mem_pool,
                            ContFramePool *_process_mem_pool,
                            const unsigned long _shared_size)
{
   // assert(false);
   kernel_mem_pool = _kernel_mem_pool;
   process_mem_pool = _process_mem_pool;
   shared_size = _shared_size;
   Console::puts("Initialized Paging System\n");
}

PageTable::PageTable()
{
   // assert(false);
   page_directory = (unsigned long *)(kernel_mem_pool->get_frames(1) << 12);
   unsigned long *page_table = (unsigned long *)(kernel_mem_pool->get_frames(1) << 12); // we turn this to unsigned long* cus each PTE is 4B and unsigned long is also 4B
   unsigned long physical_address = 0;
   // Fill the first page table page to mark first 4MB in physical memory as "present"
   for (int i = 0; i < Machine::PT_ENTRIES_PER_PAGE; i++)
   {
      page_table[i] = physical_address | 0x3;
      physical_address += Machine::PAGE_SIZE;
   }
   // Fill the first PDE and mark otehr PDE as "not present"
   page_directory[0] = ((unsigned long)page_table) | 0x3;
   for (int i = 1; i < Machine::PT_ENTRIES_PER_PAGE; i++)
   {
      page_directory[i] = 0 | 0x2;
   }

   Console::puts("Constructed Page Table object\n");
}

void PageTable::load()
{
   // assert(false);
   current_page_table = this;
   write_cr3((unsigned long)page_directory);
   Console::puts("Loaded page table\n");
}

void PageTable::enable_paging()
{
   // assert(false);
   // Before enable_paging(),we need to make sure page directory already load into CR3
   assert((unsigned long)(current_page_table->page_directory) == read_cr3());
   paging_enabled = 1;
   write_cr0(read_cr0() | 0x80000000);
   Console::puts("Enabled paging\n");
}

void PageTable::handle_fault(REGS *_r)
{
   // assert(false);
   switch (_r->err_code & (0x7))
   {
   case 0x7:
      Console::puts("Error Code : User|Write|Protection Fault\n");
      break;
   case 0x6:
      Console::puts("Error Code : User|Write|Not Present\n");
      break;
   case 0x5:
      Console::puts("Error Code : User|Read|Protection Fault\n");
      break;
   case 0x4:
      Console::puts("Error Code : User|Read|Not Present\n");
      break;
   case 0x3:
      Console::puts("Error Code : Kernel|Write|Protection Fault\n");
      break;
   case 0x2:
      Console::puts("Error Code : Kernel|Write|Not Present\n");
      break;
   case 0x1:
      Console::puts("Error Code : Kernel|Read|Protection Fault\n");
      break;
   case 0x0:
      Console::puts("Error Code : Kernel|Read|Not Present\n");
      break;
   }

   if (_r->err_code & 0x1 == 0x0)
   { // Page not present
      unsigned long *current_page_directory = (unsigned long *)read_cr3();
      unsigned long frame_address = (process_mem_pool->get_frames(1)) << 12; //frame address allocated to store the new page
      unsigned long virtual_address = read_cr2();        //virtual address which triggers page fault
      unsigned long pde_index = (virtual_address & 0xFFC00000) >> 22;
      unsigned long pte_index = (virtual_address & 0x003FF000) >> 12; 
      bool new_table = false;                                                       

      if ((current_page_directory[pde_index] & 0x1) != 0x1) // PDE is not valid
      {
         current_page_directory[pde_index] = (kernel_mem_pool->get_frames(1) << 12); // Allocate frame for a new page table
         current_page_directory[pde_index] = current_page_directory[pde_index] | 0x3;   //Only kernel/supervisor has the right to modify this entry and this page table is valid now                
         new_table = true;
      }

      unsigned long *page_table = (unsigned long *)(current_page_directory[pde_index] & 0xFFFFF000); // Pointer to the page table 

      if (new_table) // If it is a new allocated page table 
      {
         for (unsigned int i = 0; i < 1024; i++) // Intialize every entry of page table
         {
            page_table[i] = 0 | 0x2; 
         }
      }
      page_table[pte_index] = frame_address | 0x3; 
   }
   else
   {
      Console::puts("Fault was cause by a protection fault, type of access need to be modified\n");
      assert(false);
   }
   Console::puts("handled page fault\n");
}
