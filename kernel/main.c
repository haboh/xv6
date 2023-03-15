#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"
#include "sleeplocks_container.h"

volatile static int started = 0;
struct sleeplocks_container_t sleeplocks_container; 

// start() jumps here in supervisor mode on all CPUs.
void
main()
{
  if(cpuid() == 0){
    consoleinit();
    printfinit();
    printf("\n");
    printf("xv6 kernel is booting\n");
    printf("\n");
    kinit();         // physical page allocator
    kvminit();       // create kernel page table
    kvminithart();   // turn on paging
    procinit();      // process table
    trapinit();      // trap vectors
    trapinithart();  // install kernel trap vector
    plicinit();      // set up interrupt controller
    plicinithart();  // ask PLIC for device interrupts
    binit();         // buffer cache
    iinit();         // inode table
    fileinit();      // file table
    virtio_disk_init(); // emulated hard disk
    userinit();      // first user process
   
    // sleeplocks_container init
    initlock(&sleeplocks_container.locker, "sleeplocks container locker");
    for (int i = 0; i < SLEEPLOCKS_CONTAINER_SIZE; i++) {
	initsleeplock(sleeplocks_container.locks + i, "sleeplocks container");
	sleeplocks_container.busy_locks[i] = 0;
    } 
    
    __sync_synchronize();
    started = 1;
  } else {
    while(started == 0)
      ;
    __sync_synchronize();
    printf("hart %d starting\n", cpuid());
    kvminithart();    // turn on paging
    trapinithart();   // install kernel trap vector
    plicinithart();   // ask PLIC for device interrupts
  
  
  } 
  scheduler();        
}
