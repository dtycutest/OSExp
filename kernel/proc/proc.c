#include "proc/proc.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "proc/proc.h"
#include "memlayout.h"
#include "proc/initcode.h"

static cpu_t cpus[NCPU];

cpu_t* mycpu(void)
{
    int id = cpuid();
    struct cpu *c = &cpus[id];
    return c;
}

int cpuid(void) 
{
    int id = r_tp();
    return id;
}

// Return the current struct proc *, or zero if none.
struct proc*
myproc(void)
{
  push_off();
  struct cpu *c = mycpu();
  struct proc *p = c->proc;
  pop_off();
  return p;
}

// in trampoline.S
extern char trampoline[];

// in swtch.S
extern void swtch(context_t* old, context_t* new);

// in trap_user.c
extern void usertrapret();


// 第一个进程
proc_t proc[NPROC];
#define trampoline_pa  ((uint64)trampoline)
#define PTE_U_RWX (PTE_U | PTE_R | PTE_W | PTE_X) // adapt to your PTE宏

uint64 alloc_phys_page(bool in_kernel)
{
  void* p=kalloc(in_kernel);
  memset(p,0,PGSIZE);
  return (uint64)p;
}

pagetable_t proc_pgtbl_init(proc_t* p) {
  pagetable_t pagetable = uvmcreate(); 
  // if (!pagetable) panic("proc_pgtbl_init: pagetable_create failed");

  uint64 tramp_pa = (uint64)trampoline;
  if (mappages(pagetable, TRAMPOLINE, PGSIZE, tramp_pa, PTE_R | PTE_X) < 0)
    panic("proc_pgtbl_init: map TRAMPOLINE failed");
  
  if (mappages(pagetable, TRAPFRAME, PGSIZE, (uint64)p->trapframe,  PTE_R | PTE_W) < 0)
    panic("proc_pgtbl_init: map TRAPFRAME failed");




  uint64 ustack_pa = alloc_phys_page(0);
  if (!ustack_pa) panic("proc_pgtbl_init: alloc ustack failed");
  if (mappages(pagetable, USTACK_BASE, PGSIZE, ustack_pa, PTE_U | PTE_R | PTE_W) < 0)
    panic("proc_pgtbl_init: map USTACK failed");

  // if (mappages(pagetable, UART0, PGSIZE, UART0, PTE_U | PTE_R | PTE_W) < 0)
  //   panic("proc_pgtbl_init: map UART0 into user pagetable failed");

  return pagetable;
}

struct proc* allocproc(void)
{
    struct proc *p = &proc[0];  
    memset(p, 0, sizeof(*p));

    p->pid=1;

    p->kstack=KSTACK((int)(p-proc));

    p->trapframe = (struct trapframe *)kalloc(0);
    p->sz=PGSIZE;

    p->pagetable = proc_pgtbl_init(p);
    printf("p->pagetable:%p\n",p->pagetable);

    // 初始化 context 结构
    memset(&p->context, 0, sizeof(p->context));

    p->context.ra = (uint64)usertrapret; 
    p->context.sp = (uint64)p->kstack + PGSIZE; 

    return p;
}

void proc_make_first(void) {
  // 1) allocate proc struct 
  proc_t *p = allocproc(); 

  uvmfirst(p->pagetable, initcode, initcode_len);
  p->trapframe->epc = UCODE;
  p->trapframe->sp  = USTACK_TOP; 


  p->heap_top = HEAP_TOP;
  p->ustack_pages=1;

  struct cpu *c = mycpu();
  c->proc = p;

  swtch(&c->context, &p->context);
}


// Allocate a page for each process's kernel stack.
// Map it high in memory, followed by an invalid
// guard page.
void
proc_mapstacks(pagetable_t kpgtbl)
{
  struct proc *p;
  
  for(p = proc; p < &proc[NPROC]; p++) {
    char *pa = kalloc(1);
    if(pa == 0)
      panic("kalloc");
    uint64 va = KSTACK((int) (p - proc));
    kvmmap(kpgtbl, va, (uint64)pa, PGSIZE, PTE_R | PTE_W);
  }
}