#ifndef __LOCK_H__
#define __LOCK_H__

#include "common.h"

struct spinlock {
  unsigned int locked;       // Is the lock held?

  // For debugging:
  char *name;        // Name of lock.
  struct cpu *cpu;   // The cpu holding the lock.
};
typedef struct spinlock spinlock_t;

// void push_off();
// void pop_off();

// void initlock(spinlock_t* lk, char* name);
// void acquire(spinlock_t* lk);
// void release(spinlock_t* lk);
// int holding(spinlock_t* lk); 

#endif