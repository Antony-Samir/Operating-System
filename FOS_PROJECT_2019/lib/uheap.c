#include <inc/lib.h>

// malloc()
//	This function use NEXT FIT strategy to allocate space in heap
//  with the given size and return void pointer to the start of the allocated space

//	To do this, we need to switch to the kernel, allocate the required space
//	in Page File then switch back to the user again.
//
//	We can use sys_allocateMem(uint32 virtual_address, uint32 size); which
//		switches to the kernel mode, calls allocateMem(struct Env* e, uint32 virtual_address, uint32 size) in
//		"memory_manager.c", then switch back to the user mode here
//	the allocateMem function is empty, make sure to implement it.

struct Addr_Inf
{
	uint32 Size;
	uint32 Var_Addr;
};

struct Addr_Inf addr_inf[(USER_HEAP_MAX - USER_HEAP_START) / PAGE_SIZE];
bool Tracker[(USER_HEAP_MAX - USER_HEAP_START) / PAGE_SIZE] = { 0 };

uint32 Next_Fit_Addr = USER_HEAP_START;

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//

void* malloc(uint32 size)
{
	//TODO: [PROJECT 2019 - MS2 - [4] User Heap] malloc() [User Side]
	// Write your code here, remove the panic and write your code


	// Steps:
	//	1) Implement NEXT FIT strategy to search the heap for suitable space
	//		to the required allocation size (space should be on 4 KB BOUNDARY)
	//	2) if no suitable space found, return NULL
	//	 Else,
	//	3) Call sys_allocateMem to invoke the Kernel for allocation
	// 	4) Return pointer containing the virtual address of allocated space,
	//

	//This function should find the space of the required range
	// ******** ON 4KB BOUNDARY ******************* //

	//Use sys_isUHeapPlacementStrategyNEXTFIT()
	//to check the current strategy

	size = ROUNDUP(size, PAGE_SIZE);
	uint32 Add;
	uint32 Temp;
	uint32 C_Size = 0;
	uint32 X;
	bool Flag = 0;
	bool Is_Found = 0;
	//Step 1 Next Fit
	if (sys_isUHeapPlacementStrategyNEXTFIT())
	{
		X = (USER_HEAP_MAX - USER_HEAP_START) / PAGE_SIZE;
		Add = Next_Fit_Addr;
		while (X != 0)
		{
			if (Tracker[(Add - USER_HEAP_START) / PAGE_SIZE] == 0 && Flag == 1)
			{
				C_Size += PAGE_SIZE;
				X--;
				Add += PAGE_SIZE;
			}
			else if (Tracker[(Add - USER_HEAP_START) / PAGE_SIZE] == 0 && Flag == 0)
			{
				Flag = 1;
				Temp = Add;
				C_Size += PAGE_SIZE;
				X--;
				Add += PAGE_SIZE;
			}
			else
			{
				if (C_Size >= size)
				{
					Next_Fit_Addr = Temp;
					Is_Found = 1;
					break;
				}
				X -= Tracker[(Add - USER_HEAP_START) / PAGE_SIZE];
				Add += PAGE_SIZE * Tracker[(Add - USER_HEAP_START) / PAGE_SIZE];
				Flag = 0;
				C_Size = 0;
			}
			if (C_Size >= size)
			{
				Next_Fit_Addr = Temp;
				Is_Found = 1;
				break;
			}
			if (Add >= USER_HEAP_MAX)
			{
				C_Size = 0;
				Flag = 0;
				Add = USER_HEAP_START;
			}
		}
		if (Is_Found == 0 && C_Size >= size)
			Next_Fit_Addr = Temp;
		else if (Is_Found == 0)
			return NULL;
	}
	Add = (Next_Fit_Addr - USER_HEAP_START) / PAGE_SIZE;
	Tracker[Add] = size / PAGE_SIZE;
	uint32 ret = Next_Fit_Addr;
	sys_allocateMem(Next_Fit_Addr, size);
	addr_inf[(Next_Fit_Addr - USER_HEAP_START) / PAGE_SIZE].Size = size;
	addr_inf[(Next_Fit_Addr - USER_HEAP_START) / PAGE_SIZE].Var_Addr = Next_Fit_Addr;
	if (Next_Fit_Addr + size >= USER_HEAP_MAX)
		Next_Fit_Addr = USER_HEAP_START;
	return (void*)ret;

}

// free():
//	This function frees the allocation of the given virtual_address
//	To do this, we need to switch to the kernel, free the pages AND "EMPTY" PAGE TABLES
//	from page file and main memory then switch back to the user again.
//
//	We can use sys_freeMem(uint32 virtual_address, uint32 size); which
//		switches to the kernel mode, calls freeMem(struct Env* e, uint32 virtual_address, uint32 size) in
//		"memory_manager.c", then switch back to the user mode here
//	the freeMem function is empty, make sure to implement it.

void free(void* virtual_address)
{
	//TODO: [PROJECT 2019 - MS2 - [4] User Heap] free() [User Side]
	// Write your code here, remove the panic and write your code


	uint32 Temp;
	uint32 Size;
	for (int i = 0; i < (USER_HEAP_MAX - USER_HEAP_START) / PAGE_SIZE; i++)
	{
		if ((void*)addr_inf[i].Var_Addr == virtual_address)
		{
			Temp = (addr_inf[i].Var_Addr - USER_HEAP_START) / PAGE_SIZE;
			Tracker[Temp] = 0;
			Size=addr_inf[i].Size;
			addr_inf[i].Var_Addr = 0;
			addr_inf[i].Size = 0;
			break;
		}
	}
	sys_freeMem((uint32)virtual_address, Size);


	//you shold get the size of the given allocation using its address
	//you need to call sys_freeMem()
	//refer to the project presentation and documentation for details

}

//==================================================================================//
//============================== BONUS FUNCTIONS ===================================//
//==================================================================================//

//===============
// [2] realloc():
//===============

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to malloc().
//	A call with new_size = zero is equivalent to free().

//  Hint: you may need to use the sys_moveMem(uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
//		which switches to the kernel mode, calls moveMem(struct Env* e, uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
//		in "memory_manager.c", then switch back to the user mode here
//	the moveMem function is empty, make sure to implement it.

void* realloc(void* virtual_address, uint32 new_size)
{
	//TODO: [PROJECT 2019 - BONUS3] User Heap Realloc [User Side]
	// Write your code here, remove the panic and write your code
	panic("realloc() is not implemented yet...!!");

	return NULL;
}



//==================================================================================//
//============================= OTHER FUNCTIONS ====================================//
//==================================================================================//

void expand(uint32 newSize)
{
}

void shrink(uint32 newSize)
{
}

void freeHeap(void* virtual_address)
{
	return;
}


//=============
// [1] sfree():
//=============
//	This function frees the shared variable at the given virtual_address
//	To do this, we need to switch to the kernel, free the pages AND "EMPTY" PAGE TABLES
//	from main memory then switch back to the user again.
//
//	use sys_freeSharedObject(...); which switches to the kernel mode,
//	calls freeSharedObject(...) in "shared_memory_manager.c", then switch back to the user mode here
//	the freeSharedObject() function is empty, make sure to implement it.

void sfree(void* virtual_address)
{
	//[] Free Shared Variable [User Side]
	// Write your code here, remove the panic and write your code
	//panic("sfree() is not implemented yet...!!");

	//	1) you should find the ID of the shared variable at the given address
	//	2) you need to call sys_freeSharedObject()

}

void* smalloc(char* sharedVarName, uint32 size, uint8 isWritable)
{
	//[[6] Shared Variables: Creation] smalloc() [User Side]
	// Write your code here, remove the panic and write your code
	panic("smalloc() is not implemented yet...!!");

	// Steps:
	//	1) Implement FIRST FIT strategy to search the heap for suitable space
	//		to the required allocation size (space should be on 4 KB BOUNDARY)
	//	2) if no suitable space found, return NULL
	//	 Else,
	//	3) Call sys_createSharedObject(...) to invoke the Kernel for allocation of shared variable
	//		sys_createSharedObject(): if succeed, it returns the ID of the created variable. Else, it returns -ve
	//	4) If the Kernel successfully creates the shared variable, return its virtual address
	//	   Else, return NULL

	//This function should find the space of the required range
	// ******** ON 4KB BOUNDARY ******************* //

	//Use sys_isUHeapPlacementStrategyFIRSTFIT() to check the current strategy

	return 0;
}

void* sget(int32 ownerEnvID, char* sharedVarName)
{
	//[[6] Shared Variables: Get] sget() [User Side]
	// Write your code here, remove the panic and write your code
	panic("sget() is not implemented yet...!!");

	// Steps:
	//	1) Get the size of the shared variable (use sys_getSizeOfSharedObject())
	//	2) If not exists, return NULL
	//	3) Implement FIRST FIT strategy to search the heap for suitable space
	//		to share the variable (should be on 4 KB BOUNDARY)
	//	4) if no suitable space found, return NULL
	//	 Else,
	//	5) Call sys_getSharedObject(...) to invoke the Kernel for sharing this variable
	//		sys_getSharedObject(): if succeed, it returns the ID of the shared variable. Else, it returns -ve
	//	6) If the Kernel successfully share the variable, return its virtual address
	//	   Else, return NULL
	//

	//This function should find the space for sharing the variable
	// ******** ON 4KB BOUNDARY ******************* //

	//Use sys_isUHeapPlacementStrategyFIRSTFIT() to check the current strategy

	return NULL;
}

