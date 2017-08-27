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

#ifndef  __SPINLOCK_RISCV_H__
#define  __SPINLOCK_RISCV_H__

#include<string.h>

static __inline__ void spin_lock (volatile int * lock) {
  
    __asm__ __volatile__
    (
      //s2 holds oldVal, s3 holds temp, s4 holds lock
      "\t2:\n\t"
      "addi s2, x0, 1\n\t" //Sets oldVal to 1
      "add  s3, s2, x0\n\t" //Move oldVal into temp
      "add  s2, %0, x0\n\t" //Move lock into oldVal
      "add  %0, s3, x0\n\t" //Move oldVal into lock from temp
      "bne %0, x0, 2\n\t"
      :"=r"(*lock)
      :"0"(*lock)
      :
    );
}



static __inline__ void spin_unlock(volatile int* lock){
  int oldVal, temp;
  __asm__ __volatile__
  (
    "addi s2, x0, 0\n\t" //Set oldVal to 0
    "add  s3, s2, x0\n\t" //Set the temp var to hold the oldVal
    "add  s2, %0, x0\n\t" //Set oldVal to the lock
    "add  %0, s3, x0\n\t" //Set the lock to be the temp that was holding oldVal
    :"=r"(*lock)
    :"0"(*lock)
    :
  );
}



static __inline__ int trylock (volatile int* lock) {
  int oldVal, temp;
  __asm__ __volatile__
    (
    "sub  %1, %0, x0\n\t" //Set the temp var to hold the oldVal
    "sub  %0, %2, x0\n\t" //Set oldVal to the lock
    "sub  %2, %1, x0\n\t" //Set the lock to be the temp that was holding oldVal
    :"=r"(oldVal), "=r"(temp), "=r"(*lock)
    :"0"(oldVal), "1"(temp),"2"(*lock)
    :
    );
  return oldVal == 0;
}


#endif 
