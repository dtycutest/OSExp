#ifndef __VMEM_H__
#define __VMEM_H__

#include "common.h"

/*
    我们使用RISC-V体系结构中的SV39作为虚拟内存的设计规范

    satp寄存器: MODE(4) + ASID(16) + PPN(44)
    MODE控制虚拟内存模式 ASID与Flash刷新有关 PPN存放页表基地址

    基础页面 4KB
    
    VA和PA的构成:
    VA: VPN[2] + VPN[1] + VPN[0] + offset    9 + 9 + 9 + 12 = 39 (使用uint64存储) => 最大虚拟地址为512GB 
    PA: PPN[2] + PPN[1] + PPN[0] + offset   26 + 9 + 9 + 12 = 56 (使用uint64存储)
    
    为什么是 "9" : 4KB / uint64 = 512 = 2^9 所以一个物理页可以存放512个页表项
    我们使用三级页表对应三级VPN, VPN[2]称为顶级页表、VPN[1]称为次级页表、VPN[0]称为低级页表

    PTE定义:
    reserved + PPN[2] + PPN[1] + PPN[0] + RSW + D A G U X W R V  共64bit
       10        26       9        9       2    1 1 1 1 1 1 1 1
    
    需要关注的部分:
    V : valid
    X W R : execute write read (全0意味着这是页表所在的物理页)
    U : 用户态是否可以访问
    PPN区域 : 存放物理页号

*/


// void   vm_print(pagetable_t pgtbl);
// pte_t* vm_getpte(pagetable_t pgtbl, uint64 va, int alloc);
// void   vm_mappages(pagetable_t pgtbl, uint64 va, uint64 pa, uint64 len, int perm);
// void   vm_unmappages(pagetable_t pgtbl, uint64 va, uint64 len, int freeit);

// void   kvminit();
// void   kvminithart();

// // void            kvminit(void);
// // void            kvminithart(void);
// void            kvmmap(pagetable_t, uint64, uint64, uint64, int);
// // int             mappages(pagetable_t, uint64, uint64, uint64, int);
// pagetable_t     uvmcreate(void);
// void            uvmfirst(pagetable_t, uchar *, uint);
// uint64          uvmalloc(pagetable_t, uint64, uint64, int);
// uint64          uvmdealloc(pagetable_t, uint64, uint64);
// int             uvmcopy(pagetable_t, pagetable_t, uint64);
// void            uvmfree(pagetable_t, uint64);
// // void            uvmunmap(pagetable_t, uint64, uint64, int);
// void            uvmclear(pagetable_t, uint64);
// // pte_t *         walk(pagetable_t, uint64, int);
// uint64          walkaddr(pagetable_t, uint64);
// int             copyout(pagetable_t, uint64, char *, uint64);
// int             copyin(pagetable_t, char *, uint64, uint64);
// int             copyinstr(pagetable_t, char *, uint64, uint64);

#endif