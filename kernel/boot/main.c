#include "riscv.h"
#include "lib/print.h"
#include "proc/proc.h"
#include "lib/lock.h"
#include "dev/uart.h"

volatile static int started = 0;

volatile static int sum = 0;

spinlock_t addl;

int main()
{
    int cpuid = r_tp();
    if(cpuid == 0) {
        uart_init();
        print_init();
        printf("cpu %d is booting!\n", cpuid);        
        __sync_synchronize();
        started = 1;
        initlock(&addl,"add");
        acquire(&addl);
        for(int i = 0; i < 1000000; i++)
            sum++;
        printf("cpu %d report: sum = %d\n", cpuid, sum);
        release(&addl);
    } else {
        while(started == 0);
        __sync_synchronize();
        printf("cpu %d is booting!\n", cpuid);
        acquire(&addl);
        for(int i = 0; i < 1000000; i++)
            sum++;
        printf("cpu %d report: sum = %d\n", cpuid, sum);
        release(&addl);
    }   
    while (1); 
}