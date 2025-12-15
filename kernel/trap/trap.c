#include "lib/print.h"
#include "dev/timer.h"
#include "dev/uart.h"
// #include "dev/plic.h"
// #include "trap/trap.h"
// #include "proc/cpu.h"
#include "memlayout.h"
#include "defs.h"
#include "riscv.h"
#include "proc/proc.h"

// 中断信息
// static char* interrupt_info[16] = {
//     "U-mode software interrupt",      // 0
//     "S-mode software interrupt",      // 1
//     "reserved-1",                     // 2
//     "M-mode software interrupt",      // 3
//     "U-mode timer interrupt",         // 4
//     "S-mode timer interrupt",         // 5
//     "reserved-2",                     // 6
//     "M-mode timer interrupt",         // 7
//     "U-mode external interrupt",      // 8
//     "S-mode external interrupt",      // 9
//     "reserved-3",                     // 10
//     "M-mode external interrupt",      // 11
//     "reserved-4",                     // 12
//     "reserved-5",                     // 13
//     "reserved-6",                     // 14
//     "reserved-7",                     // 15
// };

// // 异常信息
// static char* exception_info[16] = {
//     "Instruction address misaligned", // 0
//     "Instruction access fault",       // 1
//     "Illegal instruction",            // 2
//     "Breakpoint",                     // 3
//     "Load address misaligned",        // 4
//     "Load access fault",              // 5
//     "Store/AMO address misaligned",   // 6
//     "Store/AMO access fault",         // 7
//     "Environment call from U-mode",   // 8
//     "Environment call from S-mode",   // 9
//     "reserved-1",                     // 10
//     "Environment call from M-mode",   // 11
//     "Instruction page fault",         // 12
//     "Load page fault",                // 13
//     "reserved-2",                     // 14
//     "Store/AMO page fault",           // 15
// };

// in trap.S
// 内核中断处理流程
extern void kernelvec();

// 初始化trap中全局共享的东西
void trap_kernel_init()
{
    timer_create();
}

// 各个核心trap初始化
void trap_kernel_inithart()
{
    w_stvec((uint64)kernelvec);
}

// 外设中断处理 (基于PLIC)
int external_interrupt_handler()
{
    uint64 scause = r_scause();

  if((scause & 0x8000000000000000L) &&
     (scause & 0xff) == 9){
    // this is a supervisor external interrupt, via PLIC.

    // irq indicates which device interrupted.
    int irq = plic_claim();
    if(irq == UART0_IRQ){
      // int trap_id = scause & 0xf; 
      // printf("%s\n",interrupt_info[trap_id]);
      uartintr();
    } else if(irq == VIRTIO0_IRQ){
      virtio_disk_intr();
    } else if(irq){
      printf("unexpected interrupt irq=%d\n", irq);
    }

    // the PLIC allows each device to raise at most one
    // interrupt at a time; tell the PLIC the device is
    // now allowed to interrupt again.
    if(irq)
      plic_complete(irq);

    return 1;
  } else if(scause == 0x8000000000000001L){
    // software interrupt from a machine-mode timer interrupt,
    // forwarded by timervec in kernelvec.S.

    // int trap_id = scause & 0xf; 
    // printf("%s\n",interrupt_info[trap_id]);

    if(cpuid() == 0){
      timer_interrupt_handler();
      // printf("timer ticks: %d\n",timer_get_ticks());
    }
    
    // acknowledge the software interrupt by clearing
    // the SSIP bit in sip.
    w_sip(r_sip() & ~2);

    return 2;
  } else {
    return 0;
  }
}

// 时钟中断处理 (基于CLINT)
void timer_interrupt_handler()
{
    timer_update();
}

// 在kernel_vector()里面调用
// 内核态trap处理的核心逻辑
void trap_kernel_handler()
{
    int which_dev=0;
  
    uint64 sepc = r_sepc();          // 记录了发生异常时的pc值
    uint64 sstatus = r_sstatus();    // 与特权模式和中断相关的状态信息
    uint64 scause = r_scause();      // 引发trap的原因
    // uint64 stval = r_stval();        // 发生trap时保存的附加信息(不同trap不一样)
    
    // printf("kerneltrap: scause=%p\n", scause);

    // 确认trap来自S-mode且此时trap处于关闭状态
    if((sstatus & SSTATUS_SPP) == 0)
        panic("kerneltrap: not from supervisor mode");
    if(intr_get() != 0)
        panic("kerneltrap: interrupts enabled");

    // 处理设备中断
    if((which_dev = external_interrupt_handler()) == 0){
      // 如果不是设备中断，那就是内核错误
      printf("scause %p\n", scause);
      printf("sepc=%p stval=%p\n", r_sepc(), r_stval());
      panic("kerneltrap");
    }

    // 内核中的进程调度
    // 如果这是定时器中断，则让出 CPU。
    // 允许在内核执行过程中进行进程切换
    if(which_dev == 2 && myproc() != 0 && myproc()->state == RUNNING)
      yield();

    w_sepc(sepc);
    w_sstatus(sstatus);
}

extern char trampoline[], uservec[], userret[];

void
usertrap(void)
{
  int which_dev = 0;

  if((r_sstatus() & SSTATUS_SPP) != 0)
    panic("usertrap: not from user mode");

  // send interrupts and exceptions to kerneltrap(),
  // since we're now in the kernel.
  w_stvec((uint64)kernelvec);

  struct proc *p = myproc();
  
  // save user program counter.
  p->trapframe->epc = r_sepc();
  
  if(r_scause() == 8){
    // system call
    // printf("[usertrap] user made a syscall (ECALL)\n");
    // int num = p->trapframe->a7;
    // printf("pid %d syscall %d\n", p->pid, num);

    if(killed(p))
      exit(-1);

    // sepc points to the ecall instruction,
    // but we want to return to the next instruction.
    p->trapframe->epc += 4;

    // an interrupt will change sepc, scause, and sstatus,
    // so enable only now that we're done with those registers.
    intr_on();

    syscall();
  } 
  else if((which_dev = external_interrupt_handler()) != 0){
    // ok
  } else {
    printf("usertrap(): unexpected scause %p pid=%d\n", r_scause(), p->pid);
    printf("            sepc=%p stval=%p\n", r_sepc(), r_stval());
    setkilled(p);
  }

  if(killed(p))
    exit(-1);

  // give up the CPU if this is a timer interrupt.
  if(which_dev == 2)
    yield();

  usertrapret();
}

void
usertrapret(void)
{
  struct proc *p = myproc();

  intr_off();

  // send syscalls, interrupts, and exceptions to uservec in trampoline.S
  uint64 trampoline_uservec = TRAMPOLINE + (uservec - trampoline);
  w_stvec(trampoline_uservec);

  // set up trapframe values that uservec will need when
  // the process next traps into the kernel.
  p->trapframe->kernel_satp = r_satp();         // kernel page table
  p->trapframe->kernel_sp = p->kstack + PGSIZE; // process's kernel stack
  p->trapframe->kernel_trap = (uint64)usertrap;
  p->trapframe->kernel_hartid = r_tp();         // hartid for cpuid()


  // set S Previous Privilege mode to User.
  unsigned long x = r_sstatus();
  x &= ~SSTATUS_SPP; // clear SPP to 0 for user mode
  x |= SSTATUS_SPIE; // enable interrupts in user mode
  w_sstatus(x);

  // set S Exception Program Counter to the saved user pc.
  w_sepc(p->trapframe->epc);

  // tell trampoline.S the user page table to switch to.
  uint64 satp = MAKE_SATP(p->pagetable);

  // jump to userret in trampoline.S at the top of memory, which 
  // switches to the user page table, restores user registers,
  // and switches to user mode with sret.
  uint64 trampoline_userret = TRAMPOLINE + (userret - trampoline);

  ((void (*)(uint64))trampoline_userret)(satp);
}
