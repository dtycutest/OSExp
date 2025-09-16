#include <stdarg.h>

#include "lib/lock.h"
#include "riscv.h"
#include "lib/defs.h"
#include "proc/proc.h"
#include "dev/uart.h"

#define BACKSPACE 0x100
#define C(x)  ((x)-'@')  // Control-x

//
// send one character to the uart.
// called by printf(), and to echo input characters,
// but not from write().
//
void
consputc(int c)
{
  if(c == BACKSPACE){
    // if the user typed backspace, overwrite with a space.
    uart_putc_sync('\b'); uart_putc_sync(' '); uart_putc_sync('\b');
  } else {
    uart_putc_sync(c);
  }
}