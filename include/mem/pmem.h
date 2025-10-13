#ifndef __PMEM_H__
#define __PMEM_H__

#include "common.h"

// 来自kernel.ld
// extern char KERNEL_DATA[];
// extern char ALLOC_BEGIN[];
// extern char ALLOC_END[];

// void  kinit(void);
// void* kalloc(bool in_kernel);
// void  kfree(void* page, bool in_kernel);

#define KERNEL_PAGES 4096

#endif