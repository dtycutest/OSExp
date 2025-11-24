#include "common.h"
#include "riscv.h"
struct buf;
struct context;
struct file;
struct inode;
struct pipe;
struct proc;
struct spinlock;
struct sleeplock;
struct stat;
struct superblock;


// console.c
void            consoleinit(void);
void            consoleintr(int);
void            consputc(int);

// string.c
int             memcmp(const void*, const void*, uint);
void*           memmove(void*, const void*, uint);
void*           memset(void*, int, uint);
char*           safestrcpy(char*, const char*, int);
int             strlen(const char*);
int             strncmp(const char*, const char*, uint);
char*           strncpy(char*, const char*, int);

//print.c
void print_init(void);
void printf(char* fmt, ...);
void panic(char* warning);
void assert(bool condition, const char* warning);

// proc.c
void proc_make_first(void);

int             cpuid(void);
void            exit(int);
int             fork(void);
int             growproc(int);
void            proc_mapstacks(pagetable_t);
// pagetable_t     proc_pagetable(struct proc *);
void            proc_freepagetable(pagetable_t, uint64);
int             kill(int);
int             killed(struct proc*);
void            setkilled(struct proc*);
struct cpu*     mycpu(void);
// struct cpu*     getmycpu(void);
struct proc*    myproc();
void            procinit(void);
// void            scheduler(void) __attribute__((noreturn));
// void            sched(void);
void            sleep(void*, struct spinlock*);
// void            userinit(void);
int             wait(uint64);
void            wakeup(void*);
// void            yield(void);
int             either_copyout(int user_dst, uint64 dst, void *src, uint64 len);
int             either_copyin(void *dst, int user_src, uint64 src, uint64 len);
void            procdump(void);

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
//kvm:
void   vm_print(pagetable_t pgtbl);
pte_t* vm_getpte(pagetable_t pgtbl, uint64 va, int alloc);
void   vm_mappages(pagetable_t pgtbl, uint64 va, uint64 pa, uint64 len, int perm);
void   vm_unmappages(pagetable_t pgtbl, uint64 va, uint64 len, int freeit);
void   kvminit();
void   kvminithart();
void            kvmmap(pagetable_t, uint64, uint64, uint64, int);
//uvm:
int             mappages(pagetable_t, uint64, uint64, uint64, int);
pagetable_t     uvmcreate(void);
void            uvmfirst(pagetable_t, uchar *, uint);
uint64          uvmalloc(pagetable_t, uint64, uint64, int);
uint64          uvmdealloc(pagetable_t, uint64, uint64);
int             uvmcopy(pagetable_t, pagetable_t, uint64);
void            uvmfree(pagetable_t, uint64);
void            uvmunmap(pagetable_t, uint64, uint64, int);
void            uvmclear(pagetable_t, uint64);
pte_t *         walk(pagetable_t, uint64, int);
uint64          walkaddr(pagetable_t, uint64);
int             copyout(pagetable_t, uint64, char *, uint64);
int             copyin(pagetable_t, char *, uint64, uint64);
int             copyinstr(pagetable_t, char *, uint64, uint64);

//timer.c
void   timerinit();       // 时钟初始化(in M-mode)

void   timer_create();     // 时钟创建
void   timer_update();     // 时钟更新(ticks++)
uint64 timer_get_ticks();  // 获取时钟的tick


//trap.c
void trap_kernel_init();
void trap_kernel_inithart();
void trap_kernel_handler();
void            usertrapret(void);

// 辅助函数: 外设中断和时钟中断处理

void external_interrupt_handler();
void timer_interrupt_handler();


//plic.c
void plicinit(void);          // 设置中断优先级
void plicinithart(void);      // 使能中断开关
int  plic_claim(void);         // 获取中断号
void plic_complete(int irq);   // 告知中断响应完成

// uart.c
void            uartinit(void);
void            uartintr(void);
void            uartputc(int);
void            uartputc_sync(int);
int             uartgetc(void);

// syscall.c
void            argint(int, int*);
int             argstr(int, char*, int);
void            argaddr(int, uint64 *);
int             fetchstr(uint64, char*, int);
int             fetchaddr(uint64, uint64*);
void            syscall();
// number of elements in fixed-size array
#define NELEM(x) (sizeof(x)/sizeof((x)[0]))


// scheduler.c
void            scheduler(void) __attribute__((noreturn));
void            sched(void);
void            yield(void);


// swtch.S
void            swtch(struct context*, struct context*);


// bio.c
void            binit(void);
struct buf*     bread(uint, uint);
void            brelse(struct buf*);
void            bwrite(struct buf*);
void            bpin(struct buf*);
void            bunpin(struct buf*);

// file.c
struct file*    filealloc(void);
void            fileclose(struct file*);
struct file*    filedup(struct file*);
void            fileinit(void);
int             fileread(struct file*, uint64, int n);
int             filestat(struct file*, uint64 addr);
int             filewrite(struct file*, uint64, int n);

// fs.c
void            fsinit(int);
int             dirlink(struct inode*, char*, uint);
struct inode*   dirlookup(struct inode*, char*, uint*);
struct inode*   ialloc(uint, short);
struct inode*   idup(struct inode*);
void            iinit();
void            ilock(struct inode*);
void            iput(struct inode*);
void            iunlock(struct inode*);
void            iunlockput(struct inode*);
void            iupdate(struct inode*);
int             namecmp(const char*, const char*);
struct inode*   namei(char*);
struct inode*   nameiparent(char*, char*);
int             readi(struct inode*, int, uint64, uint, uint);
void            stati(struct inode*, struct stat*);
int             writei(struct inode*, int, uint64, uint, uint);
void            itrunc(struct inode*);

// log.c
void            initlog(int, struct superblock*);
void            log_write(struct buf*);
void            begin_op(void);
void            end_op(void);

// virtio_disk.c
void            virtio_disk_init(void);
void            virtio_disk_rw(struct buf *, int);
void            virtio_disk_intr(void);

// sleeplock.c
void            acquiresleep(struct sleeplock*);
void            releasesleep(struct sleeplock*);
int             holdingsleep(struct sleeplock*);
void            initsleeplock(struct sleeplock*, char*);

// pipe.c
int             pipealloc(struct file**, struct file**);
void            pipeclose(struct pipe*, int);
int             piperead(struct pipe*, uint64, int);
int             pipewrite(struct pipe*, uint64, int);