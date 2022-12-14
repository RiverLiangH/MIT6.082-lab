// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct kmem{
  struct spinlock lock;
  struct run *freelist;
};

struct kmem kemArray[NCPU];//lab8 added

void
kinit()
{
	for(int i=0;i<NCPU;i++)
	{
		initlock(&(kemArray[i].lock),"kmem");
	}//lab8-1
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  //lab8-1
  push_off();
  int cpuId = cpuid();
  acquire(&(kemArray[cpuId].lock));
  r->next = kemArray[cpuId].freelist;
  kemArray[cpuId].freelist = r;
  release(&(kemArray[cpuId].lock));
  pop_off();
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  //lab8-1
  push_off();
  int cpuId = cpuid();
  acquire(&(kemArray[cpuId].lock));
  r = kemArray[cpuId].freelist;
  if(r)
    kemArray[cpuId].freelist = r->next;
  else{
	  for(int i=(cpuId+1)%NCPU, j=0;j<NCPU-1;i = (i+1)%NCPU, j++){
		  if(kemArray[i].freelist){
			  acquire(&(kemArray[i].lock));
			  r = kemArray[i].freelist;
			  kemArray[i].freelist = r->next;
			  release(&(kemArray[i].lock));
			  break;
		  }
	  }

  }
  release(&(kemArray[cpuId].lock));
  pop_off();
  //end

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}
