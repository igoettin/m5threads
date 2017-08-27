/*
    m5threads, a pthread library for the M5 simulator
    Copyright (C) 2009, Stanford University

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*/


#ifndef __SPINLOCK_X86_H__
#define __SPINLOCK_X86_H__

// routines from /usr/src/linux/include/asm-x86/spinlock.h

//********************
//*PROVIDED FUNCTIONS*
//********************
/*
static __inline__ void spin_lock (volatile int* lock) {
    char oldval;
    __asm__ __volatile__
        (
         //Label 1 
         "\n1:\t" \
         //Takes a byte of %1 and compares it to 0 (can replace with cmp)
         "cmpb $0,%1\n\t" \ 
         //Jumps to 1 if $0 >u %1 where  >u is an unsigned greater than
         //1b refers to the label before the instruction
         "ja 1b\n\t" \ 
         //Swaps the bytes in %b0 and %1. The b at the end of xchg swaps bytes. (can replace with xch)
         "xchgb %b0, %1\n\t" \
         //Compares bytes in $0 and %0
         "cmpb $0,%0\n" \
         // Jumps to 1 if $0 > %0 (unsigned). 1b refers to the label before the instruction
         "ja 1b\n\t"
         //= in each means that the operand is output
         //q means the MQ register; a temp storage register whose contents can be transferred
         //to or from the accumulator.
         //m means a memory operand
         //"0" indicates that %0 is R and W and takes 1 as the input variable
         //"memory" indicates the code is changed memory"
         :"=q"(oldval), "=m"(*lock) //Input operands
         : "0"(1) //Output operands
         : "memory"); //Constraints/clobbered registers
}

static __inline__ void spin_unlock (volatile int* lock) {
	__asm__ __volatile__
        ("movb $0,%0" \
         :"=m" (*lock) : : "memory");
}

static __inline__ int trylock (volatile int* lock) {
    char oldval;
    __asm__ __volatile__
        (
         "xchgb %b0,%1"
         :"=q" (oldval),
          "=m" (*lock)
         :"0" (1) 
         : "memory");
    return oldval == 0;
}

*/

//**********************
//*****MY FUNCTIONS*****
//********************** 
//Lock var: 1 = locked, 0 = unlocked.

static __inline__ void spin_lock (volatile int * lock) {
  __asm__ __volatile__
  (
    "\n1: \n\t" 
    "movl $1, %%eax\n\t"  //Set EAX to 1.
    "xchg %%eax, %0\n\t" //Swap EAX with the lock variable
    "test %%eax, %%eax\n\t"  //Test EAX with itself (performs bitwise &). Sets the zero flag if EAX == 0. If EAX is 0 then the lock was unlocked and we just locked it. 
    "jnz  1b\n"  //Jumps to 1 if zero flag is not set. Lock was just locked, need to spin until it's unlocked.
    :"=m"(*lock)
    :
    : "memory"
  );
}



static __inline__ void spin_unlock(volatile int* lock){
  int oldVal;
  __asm__ __volatile__
  (
    "movl $0, %1\n\t" //Give oldVal 0
    "xchg %1, %0\n\t" //Swap oldVal and lock variable
    : "=m"(*lock), "=r"(oldVal)
    :
    : "memory"
  );
}



static __inline__ int trylock (volatile int* lock) {
  char oldVal;
  __asm__ __volatile__
    (
      "xchg %0, %1" //Give the oldvar variable the lock variable's value
      :"=q"(oldVal), "=m"(*lock)
      :"0"(1)
      :"memory"
    );
  return oldVal == 0;
}


#endif  // __SPINLOCK_X86_H__
