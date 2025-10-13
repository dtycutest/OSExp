#include "riscv.h"
#include "param.h"

__attribute__ ((aligned (16))) uint8 CPU_stack[4096 * NCPU];

void main();

void start()
{
    unsigned long x = r_mstatus();
    x &= ~MSTATUS_MPP_MASK;  // 清除MPP位域
    x |= MSTATUS_MPP_S;      // 设置MPP为管理者模式
    w_mstatus(x);

    w_mepc((uint64)main);

    // 暂时禁用分页机制
    w_satp(0);

    // 将所有中断和异常委托给管理者模式处理
    w_medeleg(0xffff);  // 异常委托
    w_mideleg(0xffff);  // 中断委托
    // 启用管理者模式的外部中断、定时器中断和软件中断
    w_sie(r_sie() | SIE_SEIE | SIE_STIE | SIE_SSIE);

    // 配置物理内存保护(PMP)，给予管理者模式访问全部物理内存的权限
    w_pmpaddr0(0x3fffffffffffffull);  // 设置PMP地址范围
    w_pmpcfg0(0xf);                   // 设置PMP配置(读写执行权限)

    int id = r_mhartid();
    w_tp(id);

    // 切换到管理者模式并跳转到main()函数
    asm volatile("mret");
}