#include "proc/proc.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "proc/proc.h"

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

// void
// proc_mapstacks(pagetable_t kpgtbl)
// {
//   struct proc *p;
  
//   for(p = proc; p < &proc[NPROC]; p++) {
//     char *pa = kalloc();
//     if(pa == 0)
//       panic("kalloc");
//     uint64 va = KSTACK((int) (p - proc));
//     kvmmap(kpgtbl, va, (uint64)pa, PGSIZE, PTE_R | PTE_W);
//   }
// }
