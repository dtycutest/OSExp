#include "userlib.h"
#include "sys.h"

int main(int argc, char* argv[])
{
    syscall(SYS_print,"TEST START:");
    for(int i=0;argv[i];i++)
    {
        syscall(SYS_print,argv[i]);
    }
    syscall(SYS_print,"TEST END:");
    while(1);
    return 0;
}