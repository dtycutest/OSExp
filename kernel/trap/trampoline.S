        #
        # low-level code to handle traps from user space into
        # the kernel, and returns from kernel to user.
        #
        # the kernel maps the page holding this code
        # at the same virtual address (TRAMPOLINE)
        # in user and kernel space so that it continues
        # to work when it switches page tables.
        # kernel.ld causes this code to start at 
        # a page boundary.
        #

# #include "riscv.h"
# #include "memlayout.h"
.equ TRAMPOLINE, 0x3ffffff000
.equ TRAPFRAME,  0x3fffffe000

.section trampsec
.globl trampoline
trampoline:
.align 4
.globl uservec
uservec:    
	#
        # trap.c sets stvec to point here, so
        # traps from user space start here,
        # in supervisor mode, but with a
        # user page table.
        #

        # save user a0 in sscratch so
        # a0 can be used to get at TRAPFRAME.
        csrw sscratch, a0

        # each process has a separate p->trapframe memory area,
        # but it's mapped to the same virtual address
        # (TRAPFRAME) in every process's user page table.
        li a0, TRAPFRAME
        
        # save the user registers in TRAPFRAME
        sd ra, 40(a0)
        sd sp, 48(a0)
        sd gp, 56(a0)
        sd tp, 64(a0)
        sd t0, 72(a0)
        sd t1, 80(a0)
        sd t2, 88(a0)
        sd s0, 96(a0)
        sd s1, 104(a0)
        sd a1, 120(a0)
        sd a2, 128(a0)
        sd a3, 136(a0)
        sd a4, 144(a0)
        sd a5, 152(a0)
        sd a6, 160(a0)
        sd a7, 168(a0)
        sd s2, 176(a0)
        sd s3, 184(a0)
        sd s4, 192(a0)
        sd s5, 200(a0)
        sd s6, 208(a0)
        sd s7, 216(a0)
        sd s8, 224(a0)
        sd s9, 232(a0)
        sd s10, 240(a0)
        sd s11, 248(a0)
        sd t3, 256(a0)
        sd t4, 264(a0)
        sd t5, 272(a0)
        sd t6, 280(a0)

	# save the user a0 in p->trapframe->a0
        csrr t0, sscratch
        sd t0, 112(a0)

        # initialize kernel stack pointer, from p->trapframe->kernel_sp
        ld sp, 8(a0)

        # make tp hold the current hartid, from p->trapframe->kernel_hartid
        ld tp, 32(a0)

        # load the address of usertrap(), from p->trapframe->kernel_trap
        ld t0, 16(a0)


        # fetch the kernel page table address, from p->trapframe->kernel_satp.
        ld t1, 0(a0)

        # wait for any previous memory operations to complete, so that
        # they use the user page table.
        sfence.vma zero, zero

        # install the kernel page table.
        csrw satp, t1

        # flush now-stale user entries from the TLB.
        sfence.vma zero, zero

        # jump to usertrap(), which does not return
        jr t0

.globl userret
userret:
        # 用户返回函数 - 从内核返回到用户空间
        # userret(pagetable)
        # 由 trap.c 中的 usertrapret() 调用
        # 从内核切换到用户。
        # a0: 用户页表，用于 satp。
        #
        # 此时的状态：
        # - 我们在内核中，使用内核页表
        # - 所有内核工作已完成
        # - 需要恢复用户状态并返回用户空间
        #

        # 第一步：切换回用户页表
        # 切换到用户页表。
        # 内存屏障确保切换是原子的
        sfence.vma zero, zero
        csrw satp, a0
        sfence.vma zero, zero

        # 第二步：准备恢复用户寄存器
        # 重新加载 TRAPFRAME 地址到 a0
        li a0, TRAPFRAME

        # 第三步：恢复所有用户寄存器
        # 从 TRAPFRAME 恢复除 a0 之外的所有寄存器
        # 注意：a0 最后恢复，因为我们还需要用它来访问 TRAPFRAME
        ld ra, 40(a0)
        ld sp, 48(a0)
        ld gp, 56(a0)
        ld tp, 64(a0)
        ld t0, 72(a0)
        ld t1, 80(a0)
        ld t2, 88(a0)
        ld s0, 96(a0)
        ld s1, 104(a0)
        ld a1, 120(a0)
        ld a2, 128(a0)
        ld a3, 136(a0)
        ld a4, 144(a0)
        ld a5, 152(a0)
        ld a6, 160(a0)
        ld a7, 168(a0)
        ld s2, 176(a0)
        ld s3, 184(a0)
        ld s4, 192(a0)
        ld s5, 200(a0)
        ld s6, 208(a0)
        ld s7, 216(a0)
        ld s8, 224(a0)
        ld s9, 232(a0)
        ld s10, 240(a0)
        ld s11, 248(a0)
        ld t3, 256(a0)
        ld t4, 264(a0)
        ld t5, 272(a0)
        ld t6, 280(a0)

	# 第四步：恢复用户 a0 寄存器
	# 恢复用户 a0
        # 这是最后恢复的寄存器，因为我们需要用它来访问 TRAPFRAME
        ld a0, 112(a0)
        
        # 第五步：返回用户模式
        # 返回到用户模式和用户 pc。
        # usertrapret() 设置了 sstatus 和 sepc。
        # sret 指令会：
        # 1. 恢复之前的特权级别（用户模式）
        # 2. 跳转到 sepc 寄存器指向的地址（用户程序的下一条指令）
        # 3. 根据 sstatus 设置中断状态
        sret
