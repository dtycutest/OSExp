// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "common.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"
#include "lib/lock.h"
#include "mem/pmem.h"

void freerange(void *pa_start, void *pa_end,bool in_kernel);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

typedef struct run {
  struct run *next;
} page_node_t;

// 许多物理页构成一个可分配的区域 
typedef struct alloc_region { 
 uint64 begin; // 起始物理地址
 uint64 end; // 终止物理地址
 spinlock_t lock; // 自旋锁(保护下面两个变量)
 uint32 allocable; // 可分配页面数
 page_node_t* freelist; // 可分配链的链头节点 
} alloc_region_t; 
// 内核和用户可分配的物理页分开
static alloc_region_t kern_region, user_region; 
static uint64 KERN_PG_END=(uint64)end+KERNEL_PAGES*PGSIZE;

// struct {
//   struct spinlock lock;
//   struct run *freelist;
// } kmem;

void
kinit()
{
  initlock(&kern_region.lock, "kern_mem");
  freerange(end, (void*)(KERN_PG_END-1),true);
  initlock(&user_region.lock, "user_mem");
  freerange((void*)(KERN_PG_END), (void*)(PHYSTOP),false);
}

void
freerange(void *pa_start, void *pa_end,bool in_kernel)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  if(in_kernel){
    kern_region.begin=PGROUNDUP((uint64)pa_start);
    kern_region.begin=PGROUNDUP((uint64)pa_end);
    kern_region.allocable=0;
  }
  else{
    user_region.begin=PGROUNDUP((uint64)pa_start);
    user_region.begin=PGROUNDUP((uint64)pa_end);
    user_region.allocable=0;
  }
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE){
    kfree(p,in_kernel);
  }
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void* pa, bool in_kernel)
{
  if(in_kernel){
    struct run *r;

    if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= KERN_PG_END)
        panic("kfree kernel");
    

    // Fill with junk to catch dangling refs.
    memset(pa, 1, PGSIZE);

    r = (struct run*)pa;

    acquire(&kern_region.lock);
    r->next = kern_region.freelist;
    kern_region.freelist = r;
    kern_region.allocable++;
    release(&kern_region.lock);
  }
  else{
    struct run *r;

    if(((uint64)pa % PGSIZE) != 0 || (uint64)pa < KERN_PG_END || (uint64)pa >= PHYSTOP)
        panic("kfree user");

    // Fill with junk to catch dangling refs.
    memset(pa, 1, PGSIZE);

    r = (struct run*)pa;

    acquire(&user_region.lock);
    r->next = user_region.freelist;
    user_region.freelist = r;
    user_region.allocable++;
    release(&user_region.lock);
  }
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(bool in_kernel)
{
  if(in_kernel){
    struct run *r;

    acquire(&kern_region.lock);
    r = kern_region.freelist;
    if(r){
        kern_region.freelist = r->next;
        kern_region.allocable--;
    }
    release(&kern_region.lock);

    if(r)
        memset((char*)r, 5, PGSIZE); // fill with junk
    return (void*)r;
  }
  else{
    struct run *r;

    acquire(&user_region.lock);
    r = user_region.freelist;
    if(r){
        user_region.freelist = r->next;
        user_region.allocable--;
    }
    release(&user_region.lock);

    if(r)
        memset((char*)r, 5, PGSIZE); // fill with junk
    return (void*)r;
  }
}
