#include "common.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"

volatile static int started = 0;

int main()
{
    int cpuid = r_tp();

    if(cpuid == 0) {
        consoleinit();
        print_init();
        kinit();
        kvminit();
        kvminithart();
        procinit();
        trap_kernel_init();
        trap_kernel_inithart();
        plicinit();
        plicinithart();
        binit();             // 缓冲区缓存初始化
        iinit();             // inode表初始化
        fileinit();          // 文件表初始化
        virtio_disk_init();  // 虚拟硬盘初始化
        intr_on();

        printf("cpu %d is booting!\n", cpuid);

        proc_make_first();

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
  // 所有CPU都进入调度器，开始调度用户进程
  scheduler();       
}
