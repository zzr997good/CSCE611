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
   // assert(false);
   kernel_mem_pool = _kernel_mem_pool;
   process_mem_pool = _process_mem_pool;
   shared_size = _shared_size;
   Console::puts("Initialized Paging System\n");
}

PageTable::PageTable()
{
   // assert(false);
   page_directory = (unsigned long *)(process_mem_pool->get_frames(1) << 12);
   unsigned long *page_table = (unsigned long *)(process_mem_pool->get_frames(1) << 12); // we turn this to unsigned long* cus each PTE is 4B and unsigned long is also 4B
   unsigned long physical_address = 0;
   // Fill the first page table page to mark first 4MB in physical memory as "present"
   for (int i = 0; i < Machine::PT_ENTRIES_PER_PAGE; i++)
   {
      page_table[i] = physical_address | 0x3;
      physical_address += Machine::PAGE_SIZE;
   }
   // Fill the first PDE and mark otehr PDE as "not present"
   page_directory[0] = ((unsigned long)page_table) | 0x3;
   page_directory[Machine::PT_ENTRIES_PER_PAGE-1]=((unsigned long)page_directory)| 0x3;
   for (int i = 1; i < Machine::PT_ENTRIES_PER_PAGE-1; i++)
   {
      page_directory[i] = 0 | 0x2;
   }
    vm_pool_list_head = NULL;
    vm_pool_list_tail= NULL;
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

unsigned long * PageTable::PDE_address(unsigned long addr)
{
	unsigned long pde_index = (addr & 0xFFC00000) >> 22;
	return (unsigned long *)(0xFFFFF000|(pde_index << 2));                                           
	
}

unsigned long * PageTable::PTE_address(unsigned long addr)
{
	unsigned long pde_index = (addr & 0xFFC00000) >> 22;
	unsigned long pte_index = (addr & 0x003FF000) >> 12;
	return (unsigned long *)(0xFFC00000 | (pde_index << 12) | (pte_index << 2));       
	
}

void PageTable::handle_fault(REGS * _r)
{
    assert(false);
    Console::puts("handled page fault\n");
}

void PageTable::register_pool(VMPool * _vm_pool)
{
    //assert(false);
    //First, check whether _vm_pool is already registered
    VMPool *cur=vm_pool_list_head;
    while(cur!=NULL){
        if(cur==_vm_pool){
            Console::puts("VM pool is registered in the past\n");
            return;
        }
        cur=cur->next;
    }
    //If not, register it
    if(vm_pool_list_head==NULL){
        vm_pool_list_head=_vm_pool;
        vm_pool_list_tail=_vm_pool;
    }
    else{
        vm_pool_list_tail->next=_vm_pool;
        vm_pool_list_tail=_vm_pool;
    }
    Console::puts("registered VM pool\n");
}

void PageTable::free_page(unsigned long _page_no) {
    //assert(false);
    unsigned long addr=(_page_no<<12);
    //unsigned long* pde_addr=PDE_address(addr);
    unsigned long* pte_addr=PTE_address(addr);
    if(*pte_addr & 0x1){
        ContFramePool::release_frames(*pte_addr >> 12);
        *pte_addr = *pte_addr & 0xFFFFFFCE; //invalid, unused, undirty
        unsigned int cr3_read = read_cr3();
        write_cr3(cr3_read);
    }
    Console::puts("freed page\n");
}
