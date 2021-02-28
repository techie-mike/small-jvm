#include <stdio.h>
#include <string.h>
#include "vm_interface.h"

#include "opcodes.h"

uint64_t const_pull[CONST_PULL_SIZE]; 
uint64_t stack[STACK_SIZE] = {};

void Execute_iconst_i(uint8_t i);

uint64_t sp;

uint64_t GetSP() {
    return sp;
}

void MoveSP(int i) {
    if(i >= 0) {
        sp += (uint64_t) i;
    }
    else {
        i = -i;
        assert(sp >= (uint64_t) i);
        sp -= (uint64_t) i;
    }
}

// TODO - dynamic frame
#define FRAME_SIZE 10

#define LOG_ON

// Programm counter
uint8_t pc = 0;

// Constant pull for method
// TODO Get Const pool dynamicly

// Method frame for method
uint64_t frame[FRAME_SIZE];
// TODO Move to local variable
uint64_t* curr_frame;

// Frame pointer
uint8_t fp = 0;

void Init() {
  // 1. Clean intruction stack
  sp = 0;
  memset(stack, 0, STACK_SIZE * sizeof(stack[0]));

  // 2. TODO - fill const pull
  memset(const_pull, 0, CONST_PULL_SIZE * sizeof(const_pull[0]));

  // 3. Clean Frame
  // TODO - support dynamic frames 
  curr_frame = frame;

  // 4. Initialize pc
  pc = 0;
};


// Execute bytecode sequence
void Execute(uint8_t* bc) {
  // TODO Stack owerflow
  // TODO Garbage collection
  while (1) {
    uint8_t opcode = bc[pc];

    switch (opcode) {
      case(iconst_0) : {
             // Operand Stack:
             // ... ->
             // ..., <i>
             Execute_iconst_i(0u);
             break;
           }

      case(iconst_1) : {
             Execute_iconst_i(1u);
             break;    
           }

      case(iconst_2) : {
             Execute_iconst_i(2u);
             break;
           }           

      case(iconst_3) : {
             Execute_iconst_i(3u);
             break;
           }           

      case(iconst_4) : {
             Execute_iconst_i(4u);
             break;
           }           

      case(iconst_5) : {
             Execute_iconst_i(5u);
             break;
           }           

      case(iand) : {
               // Operand Stack:
               // ..., value_1, value_2, ->
               // ..., result
#ifdef ASM
               // TODO This command in ASM
               uint64_t value_1 = 0, value_2 = 0, result = 0;
               uint64_t offset = 0;

               // Load value_1
               asm ("mul %0, %1, %2" : "=r"(offset) : "r"(sp), "r"(sizeof(stack[0])));
               asm ("add %0, %1, %2" : "=r"(value_1)  : "r"(offset), "r"(stack));

               asm ("ld %0, 0(%1)" : "=r"(value_1): "r"(value_1));

               asm ("li %0, 1" : "=r"(offset));
               asm ("sub %0, %1, %2" :"=r"(sp) :"r"(sp), "r"(offset));

               // Load value_2
               asm ("mul %0, %1, %2" : "=r"(offset) : "r"(sp), "r"(sizeof(stack[0])));
               asm ("add %0, %1, %2" : "=r"(value_2)  : "r"(offset), "r"(stack));

               asm ("ld %0, 0(%1)" : "=r"(value_2): "r"(value_2));

               asm ("and %0, %1, %2" : "=r"(result) : "r"(value_1), "r"(value_2));

               // Write result
               asm ("mul %0, %1, %2" : "=r"(offset) : "r"(sp), "r"(sizeof(stack[0])));
               asm ("add %0, %1, %2" : "=r"(offset)  : "r"(offset), "r"(stack));

               asm ("sd  %0, 0(%1)"  :  : "r"(result), "r"(offset));

#else  
               // C-code:
               uint64_t value_1 = 0, value_2 = 0, result = 0;

               // "stack" type is "unit64_t"
               value_1 = (uint64_t) stack[sp - 1];
               value_2 = (uint64_t) stack[sp];
               result = value_1 & value_2;
               sp -= 1;
               stack[sp] = result;

#endif
	       break;
             }
      case(iadd) : {
#ifdef ASM
               uint64_t a1, a2;
               uint64_t p1, p2;
               asm ("add %0, %1, %2" :"=r"(p2) :"r"(sp * 8), "r"((uint64_t)stack));
               asm ("lw  %0, 0(%1)" :"=r"(a2) :"r"(p2));
               asm ("addi %0, %1, -1" :"=r"(sp) :"r"(sp));
               asm ("add %0, %1, %2" :"=r"(p1) :"r"(sp * 8), "r"((uint64_t)stack));
               asm ("lw  %0, 0(%1)" :"=r"(a1) :"r"(p1));
               asm ("addw %0, %1, %2" :"=r"(a1) :"r"(a1), "r"(a2));
               asm ("sw %0, 0(%1)" :  :"r"(a1), "r"(p1));
#else
               --sp;
               stack[sp] = (stack[sp] + stack[sp + 1]) % UINT32_MAX;
#endif
               break;
             }

      case(return_) : {
            // TODO support frame removing and return from methods
            if (fp == 0) {
              return;
            } else {
              assert(fp < FRAME_SIZE);
              --fp;
            }
            break;
          }
      default:
#ifdef LOG_ON
          printf("Unsupported instruction with bc = %d at pc = %d \n\r", opcode, pc);
#endif
          break;
    }
    ++pc;
  }
  assert(sp == GetSP());
};


void Execute_iconst_i(uint8_t i) {
#ifdef ASM
  // ASM CODE DIDN'T TEST, AFTER SWITCH ON "uint64_t"
  uint64_t tmp_0 = 0;
  uint64_t tmp_1 = 0;
  uint64_t offset = 0;

  asm ("addi %0, %1, 1" : "=r"(sp) :"r"(sp));
  asm ("mul %0, %1, %2" : "=r"(i) : "r"(i), "r"(sizeof(const_pull[0])));
  asm ("add %0, %1, %2" : "=r"(tmp_0) :"r"(const_pull), "r"(i));

  asm ("ld  %0, 0(%1)" : "=r"(tmp_0) : "r"(tmp_0));

  asm ("mul %0, %1, %2" : "=r"(offset) : "r"(sp), "r"(sizeof(stack[0])));
  asm ("add %0, %1, %2" : "=r"(tmp_1)  : "r"(offset), "r"(stack));
  asm ("sd  %0, 0(%1)"  :  : "r"(tmp_0), "r"(tmp_1));

#else
  // C-code:
  ++sp;
  stack[sp] = const_pull[i];
#endif
}

