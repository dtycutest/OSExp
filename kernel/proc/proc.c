#include "proc/proc.h"
#include "riscv.h"

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
