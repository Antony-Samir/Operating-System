//Verina Nashaat
//Hanaa Ayman
//Caroline Talaat
//Lydia George
//Kyrillos Nayer
//Antony Samir

#include <inc/memlayout.h>
#include <kern/kheap.h>
#include <kern/memory_manager.h>


//NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)
//struct to get start and the size of free spaces in the ram
struct Inf_Of_Space
{
	uint32 Start;
	uint32 Size;
};
struct Inf_Of_Space Allocated_Pages[((KERNEL_HEAP_MAX-KERNEL_HEAP_START)/PAGE_SIZE)];
struct Inf_Of_Space Free_Spaces[((KERNEL_HEAP_MAX-KERNEL_HEAP_START)/PAGE_SIZE)];
uint32 Caro = 0;// ay 5dma aho yarab n5ls bs
uint32 * Ptr_Page_Table;
void* kmalloc(unsigned int size)
{
	//TODO: [PROJECT 2019 - MS1 - [1] Kernel Heap] kmalloc()
	// Write your code here, remove the panic and write your code
	/////panic_into_prompt("kmalloc() is not implemented yet...!!");
	//NOTE: Allocation is based on Best FIT strategy
	//NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)
	//refer to the project presentation and documentation for details
	uint32 ptr;
	uint32 Free_Pages;
	uint32 Size_count = 0;
	uint32 indx = 0;
	if(size > (KERNEL_HEAP_MAX-KERNEL_HEAP_START))
	{
		return NULL;
	}
	//set the values
	for (ptr = KERNEL_HEAP_START; ptr <= KERNEL_HEAP_MAX; ptr = ptr+PAGE_SIZE)
	{
		Ptr_Page_Table = NULL;
		struct Frame_Info* Ptr_Frame_Info = get_frame_info(ptr_page_directory,(void *)ptr , &Ptr_Page_Table);
		 if(ptr == KERNEL_HEAP_MAX)
		 {
		 	Free_Spaces[indx].Size = Size_count;
			indx++;
			Size_count = 0;
			break ;
	   	 }
		 else if (Ptr_Frame_Info == NULL)
		 {
			if(Size_count != 0)
			{
				Size_count++;
			}
			else{
				Free_Spaces[indx].Start = ptr;
				Size_count++;
			}
		}
		else {
			Free_Spaces[indx].Size = Size_count;
			indx++;
			Size_count = 0;
		}
	}
	uint32 founded = 0;
	uint32 lydia; //offset ya gd3an //ay 5edma (kero)
	// Merciii bas yaret ta5ls b2t al function 3lshan al taslim bas :) (LYdia)
	size = ROUNDUP(size , PAGE_SIZE);
	// L pages elly 3ayza t fit
	uint32 Num_Of_Needed = size / PAGE_SIZE;
    Free_Pages =((KERNEL_HEAP_MAX-KERNEL_HEAP_START) / PAGE_SIZE) +1;
    for (int i = 0; i < indx; i++)
    {
	   if(Free_Spaces[i].Size == Num_Of_Needed)
	   {
		   lydia = i;
		   founded++;
		   break;
	   }
	   else if(Free_Spaces[i].Size < Free_Pages && Num_Of_Needed < Free_Spaces[i].Size)
	   {
		   Free_Pages = Free_Spaces[i].Size;
		   lydia = i;
		   founded++;
 	   }
    }
    if(founded == 0)
    {
	    return NULL;
    }
    ptr = Free_Spaces[lydia].Start;
    // Tony l error hnaa
    for(int i = 0; i < Num_Of_Needed; i++)
    {
	    Ptr_Page_Table = NULL;
	    struct Frame_Info *Ptr_Frame_Info = get_frame_info(ptr_page_directory ,(void *)ptr,&Ptr_Page_Table);
	    allocate_frame(&Ptr_Frame_Info);
	    map_frame(ptr_page_directory, Ptr_Frame_Info, (void *)ptr, PERM_WRITEABLE);
	    ptr += PAGE_SIZE;
    }
    //done (tony)
    uint32* Add  = (uint32*)Free_Spaces[lydia].Start;
    Allocated_Pages[Caro].Start = Free_Spaces[lydia].Start;
    Allocated_Pages[Caro].Size = Num_Of_Needed;
    Caro++;

    return (void *)Add ;

	//TODO: [PROJECT 2019 - BONUS1] Implement the first FIT strategy for Kernel allocation
	// Beside the Best FIT
	// use "isKHeapPlacementStrategyBESTFIT() ..." functions to check the current strategy
	//change this "return" according to your answer
}

void kfree(void* virtual_address)
{
	//TODO: [PROJECT 2019 - MS1 - [1] Kernel Heap] kfree()
	// Write your code here, remove the panic and write your code
	/////panic("kfree() is not implemented yet...!!");

	//you need to get the size of the given allocation using its address
	//refer to the project presentation and documentation for details

	uint32 var = 0;
	uint32 tony; // aho ya 3am
	for (int i = 0; i < Caro; i++)
	{

		if(virtual_address == (void *)Allocated_Pages[i].Start)
		{

		   tony = Allocated_Pages[i].Size;
		   while(tony > 0)
		   {
				unmap_frame(ptr_page_directory,(void *)virtual_address);
				virtual_address += PAGE_SIZE;
				tony--;

		   }
		   // remove it from the arr of free spaces
		   Allocated_Pages[i].Size = 0;
		   Allocated_Pages[i].Start = 0;
		   break;
		}
	}

}

unsigned int kheap_virtual_address(unsigned int physical_address)
{
	//TODO: [PROJECT 2019 - MS1 - [1] Kernel Heap] kheap_virtual_address()
	// Write your code here, remove the panic and write your code
	/////panic("kheap_virtual_address() is not implemented yet...!!");

	//return the virtual address corresponding to given physical_address
	//refer to the project presentation and documentation for details
	//change this "return" according to your answer

	uint32 var = 0;
	uint32 Frame_Num = (physical_address/ PAGE_SIZE);
	//kant out of range ya zaki
	for(long long i=KERNEL_HEAP_START ; i<KERNEL_HEAP_MAX ; i = i+(PAGE_SIZE*1024))
	{
		Ptr_Page_Table = NULL;
		get_page_table(ptr_page_directory, (void*)(uint32)i, &Ptr_Page_Table);
		if(Ptr_Page_Table != NULL)
		{
			for(int j = 0; j < 1024; j++)
			{
				// check if the page exist or not
				if((Ptr_Page_Table[j]>>12) == Frame_Num &&
						(Ptr_Page_Table[j]&PERM_PRESENT)!=0)
				{
					var = i + (j*PAGE_SIZE) ;
					break;
				}
			}
			if(var != 0)
			{
				break;
			}
		}
	}
	return var;
}

unsigned int kheap_physical_address(unsigned int virtual_address)
{
	//TODO: [PROJECT 2019 - MS1 - [1] Kernel Heap] kheap_physical_address()
	// Write your code here, remove the panic and write your code
	////panic("kheap_physical_address() is not implemented yet...!!");
	//return the physical address corresponding to given virtual_address
	//refer to the project presentation and documentation for details
	//change this "return" according to your answer

	int Phys_Add;
	Ptr_Page_Table = NULL;
	struct Frame_Info* Ptr_Frame_Info = get_frame_info(ptr_page_directory, (void*)virtual_address, &Ptr_Page_Table);
	if(Ptr_Frame_Info != NULL)
	{
		Phys_Add = to_physical_address(Ptr_Frame_Info);
	}
	else
	{
		Phys_Add = 0;
	}
	return Phys_Add;
}

//=================================================================================//
//============================== BONUS FUNCTION ===================================//
//=================================================================================//
// krealloc():

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to kmalloc().
//	A call with new_size = zero is equivalent to kfree().

void *krealloc(void *virtual_address, uint32 new_size)
{
	//TODO: [PROJECT 2019 - BONUS2] Kernel Heap Realloc
	// Write your code here, remove the panic and write your code

	return NULL;
	panic("krealloc() is not implemented yet...!!");

}
