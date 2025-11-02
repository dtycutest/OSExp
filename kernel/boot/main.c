#include "common.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"
#include "mem/vmem.h"

volatile static int started = 0;

int main()
{
    int cpuid = r_tp();

    if(cpuid == 0) {

        uartinit();
        print_init();
        kinit();
        kvminit();
        kvminithart();
        trap_kernel_init();
        trap_kernel_inithart();
        plicinit();
        plicinithart();
        intr_on();

        printf("cpu %d is booting!\n", cpuid);
        __sync_synchronize();
        started = 1;
    } else {

        while(started == 0);
        __sync_synchronize();
        kvminithart();
        trap_kernel_inithart();
        plicinithart();
        printf("cpu %d is booting!\n", cpuid);
        
    }
    while (1);    
}
