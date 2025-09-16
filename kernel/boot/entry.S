# kernel.ld 将_entry作为整个OS的起点放置到0x80000000处
# qemu会自动跳转到0x80000000处并开始执行
# 注意: 此时是M-mode

.section .text
.global _entry
_entry:
        # CPU_stack 定义于start.c中
        # sp = CPU_stack + ((hartid + 1) * 4096)
        # 将sp置于当前CPU的内核栈的栈顶
        la sp, CPU_stack

        #清空.bss段
        la t0,edata
        la t1,end
bss_loop:
        beq t0,t1,bss_done
        sd zero,0(t0)
        addi t0,t0,8
        j bss_loop
bss_done:
        li a0, 4096
        csrr a1, mhartid
        addi a1, a1, 1
        mul a0, a0, a1
        add sp, sp, a0
        # 跳转到start
        call start
spin:
        j spin
