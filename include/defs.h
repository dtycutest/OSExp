#include "common.h"
#include "riscv.h"
// struct buf;
// struct context;
// struct file;
// struct inode;
// struct pipe;
// struct proc;
struct spinlock;
// struct sleeplock;
// struct stat;
// struct superblock;


//console.c
void            consputc(int);

// string.c
int             memcmp(const void*, const void*, uint);
void*           memmove(void*, const void*, uint);
void*           memset(void*, int, uint);
char*           safestrcpy(char*, const char*, int);
int             strlen(const char*);
int             strncmp(const char*, const char*, uint);
char*           strncpy(char*, const char*, int);

// uart.c
// void            uartinit(void);
// void            uartintr(void);
// void            uartputc(int);
void            uart_putc_sync(int);
// int             uartgetc(void);

//print.c
void print_init(void);
void printf(char* fmt, ...);
void panic(char* warning);
void assert(bool condition, const char* warning);

// proc.c
int             cpuid(void);
// void            exit(int);
// int             fork(void);
// int             growproc(int);
// void            proc_mapstacks(pagetable_t);
// pagetable_t     proc_pagetable(struct proc *);
// void            proc_freepagetable(pagetable_t, uint64);
// int             kill(int);
// int             killed(struct proc*);
// void            setkilled(struct proc*);
struct cpu*     mycpu(void);
// struct cpu*     getmycpu(void);
// struct proc*    myproc();
// void            procinit(void);
// void            scheduler(void) __attribute__((noreturn));
// void            sched(void);
// void            sleep(void*, struct spinlock*);
// void            userinit(void);
// int             wait(uint64);
// void            wakeup(void*);
// void            yield(void);
// int             either_copyout(int user_dst, uint64 dst, void *src, uint64 len);
// int             either_copyin(void *dst, int user_src, uint64 src, uint64 len);
// void            procdump(void);

// spinlock.c
void            acquire(struct spinlock*);
int             holding(struct spinlock*);
void            initlock(struct spinlock*, char*);
void            release(struct spinlock*);
void            push_off(void);
void            pop_off(void);

// kalloc.c
void            kinit(void);
void*           kalloc(bool in_kernel);
void            kfree(void* page, bool in_kernel);

// vm.c
// void            vm_print(pagetable_t pgtbl);
// void            kvminit(void);
// void            kvminithart(void);
// void            kvmmap(pagetable_t, uint64, uint64, uint64, int);
// int             mappages(pagetable_t, uint64, uint64, uint64, int);
// pagetable_t     uvmcreate(void);
// void            uvmfirst(pagetable_t, uchar *, uint);
// uint64          uvmalloc(pagetable_t, uint64, uint64, int);
// uint64          uvmdealloc(pagetable_t, uint64, uint64);
// int             uvmcopy(pagetable_t, pagetable_t, uint64);
// void            uvmfree(pagetable_t, uint64);
// void            uvmunmap(pagetable_t, uint64, uint64, int);
// void            uvmclear(pagetable_t, uint64);
// pte_t *         walk(pagetable_t, uint64, int);
// uint64          walkaddr(pagetable_t, uint64);
// int             copyout(pagetable_t, uint64, char *, uint64);
// int             copyin(pagetable_t, char *, uint64, uint64);
// int             copyinstr(pagetable_t, char *, uint64, uint64);

//timer.c
void   timerinit();       // 时钟初始化(in M-mode)

void   timer_create();     // 时钟创建
void   timer_update();     // 时钟更新(ticks++)
uint64 timer_get_ticks();  // 获取时钟的tick


//trap.c
void trap_kernel_init();
void trap_kernel_inithart();
void trap_kernel_handler();

// 辅助函数: 外设中断和时钟中断处理

void external_interrupt_handler();
void timer_interrupt_handler();


//plic.h
void plicinit(void);          // 设置中断优先级
void plicinithart(void);      // 使能中断开关
int  plic_claim(void);         // 获取中断号
void plic_complete(int irq);   // 告知中断响应完成

// uart.c
void            uartinit(void);
void            uartintr(void);
// void            uartputc(int);
void            uartputc_sync(int);
int             uartgetc(void);