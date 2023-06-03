#ifndef PROC_H
#define PROC_H

#include "spinlock.h"
#include "param.h"
#include "riscv.h"

// Saved registers for kernel context switches.
struct context {
  uint64 ra;
  uint64 sp;

  // callee-saved
  uint64 s0;
  uint64 s1;
  uint64 s2;
  uint64 s3;
  uint64 s4;
  uint64 s5;
  uint64 s6;
  uint64 s7;
  uint64 s8;
  uint64 s9;
  uint64 s10;
  uint64 s11;
};

// Per-CPU state.
struct cpu {
  struct proc *proc;          // The process running on this cpu, or null.
  struct context scheduler;   // swtch() here to enter scheduler().
  int noff;                   // Depth of push_off() nesting.
  int intena;                 // Were interrupts enabled before push_off()?
};

extern struct cpu cpus[NCPU];

// per-process data for the trap handling code in trampoline.S.
// sits in a page by itself just under the trampoline page in the
// user page table. not specially mapped in the kernel page table.
// the sscratch register points here.
// uservec in trampoline.S saves user registers in the trapframe,
// then initializes registers from the trapframe's
// kernel_sp, kernel_hartid, kernel_satp, and jumps to kernel_trap.
// usertrapret() and userret in trampoline.S set up
// the trapframe's kernel_*, restore user registers from the
// trapframe, switch to the user page table, and enter user space.
// the trapframe includes callee-saved user registers like s0-s11 because the
// return-to-user path via usertrapret() doesn't return through
// the entire kernel call stack.
struct trapframe {
  /*   0 */ uint64 kernel_satp;   // kernel page table
  /*   8 */ uint64 kernel_sp;     // top of process's kernel stack
  /*  16 */ uint64 kernel_trap;   // usertrap()
  /*  24 */ uint64 epc;           // saved user program counter
  /*  32 */ uint64 kernel_hartid; // saved kernel tp
  /*  40 */ uint64 ra;
  /*  48 */ uint64 sp;
  /*  56 */ uint64 gp;
  /*  64 */ uint64 tp;
  /*  72 */ uint64 t0;
  /*  80 */ uint64 t1;
  /*  88 */ uint64 t2;
  /*  96 */ uint64 s0;
  /* 104 */ uint64 s1;
  /* 112 */ uint64 a0;
  /* 120 */ uint64 a1;
  /* 128 */ uint64 a2;
  /* 136 */ uint64 a3;
  /* 144 */ uint64 a4;
  /* 152 */ uint64 a5;
  /* 160 */ uint64 a6;
  /* 168 */ uint64 a7;
  /* 176 */ uint64 s2;
  /* 184 */ uint64 s3;
  /* 192 */ uint64 s4;
  /* 200 */ uint64 s5;
  /* 208 */ uint64 s6;
  /* 216 */ uint64 s7;
  /* 224 */ uint64 s8;
  /* 232 */ uint64 s9;
  /* 240 */ uint64 s10;
  /* 248 */ uint64 s11;
  /* 256 */ uint64 t3;
  /* 264 */ uint64 t4;
  /* 272 */ uint64 t5;
  /* 280 */ uint64 t6;
};

/* La structure qui représente une VMA (Virtual Memory Area). */
struct vma {
  /* C'est une structure de liste chaînée : le pointeur next pointe sur la
   * prochaine VMA dans la liste. */
  struct vma * next;

  /* Le début et la fin de la VMA. */
  uint64 va_begin;
  uint64 va_end;

  /* Éventuellement, cette VMA peut être peuplée par le contenu d'un fichier. Si
   * c'est le cas, [file] donne le nom du fichier, [file_offset] le déplacement
   * dans le fichier, et [file_nbytes] le nombre d'octets à lire dans le
   * fichier.
   *
   * Par exemple, si [file_offset] vaut 0x300, [file_nbytes] vaut 0x60, [file]
   * vaut "toto", [va_begin] vaut 0x1000 et [va_end] vaut 0x1fff, alors on
   * chargera les octets 0x300-0x35f aux adresses virtuelles 0x1000-0x105f, et
   * les adresses virtuelles 0x1060-0x1fff seront remplies de 0.
   */
  char* file;
  uint64 file_offset;
  uint64 file_nbytes;

  /* Les permissions de cette VMA. (VMA_R, VMA_W, VMA_X) */
  unsigned char vma_flags;
};

#define VMA_R (1 << 1)
#define VMA_W (1 << 2)
#define VMA_X (1 << 3)

struct vma* add_memory_area(struct proc*, uint64, uint64);
struct vma* get_memory_area(struct proc*, uint64);
void print_memory_areas(struct proc*);
void print_memory_area(struct proc*, struct vma*);
uint64 max_addr_in_memory_areas(struct proc*);
void free_vma(struct vma*);

enum procstate { UNUSED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

// Per-process state
struct proc {
  struct spinlock lock;

  // p->lock must be held when using these:
  enum procstate state;        // Process state
  struct proc *parent;         // Parent process
  void *chan;                  // If non-zero, sleeping on chan
  int killed;                  // If non-zero, have been killed
  int xstate;                  // Exit status to be returned to parent's wait
  int pid;                     // Process ID

  int priority;                // Priority

  // these are private to the process, so p->lock need not be held.
  uint64 kstack;               // Virtual address of kernel stack
  // uint64 sz;                   // Size of process memory (bytes)
  struct spinlock vma_lock;
  struct vma * memory_areas;   // VMAs du processus
  struct vma * stack_vma;      // Une VMA particulière pour la pile
  struct vma * heap_vma;       // Une VMA particulière pour le tas
  pagetable_t pagetable;       // Page table
  struct trapframe *tf;        // data page for trampoline.S
  struct context context;      // swtch() here to run process
  struct file *ofile[NOFILE];  // Open files
  struct inode *cwd;           // Current directory
  char name[16];               // Process name (debugging)
  char* cmd;
};

struct list_proc {
  struct proc* p;
  struct list_proc* next;
};

#endif
