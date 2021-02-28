#include <stdio.h>
#include <string.h>
#include "vm_interface.h"

#include "opcodes.h"

uint8_t const_pull[CONST_PULL_SIZE]; 
uint8_t stack[STACK_SIZE] = {};

uint64_t sp;

uint64_t GetSP() {
    return sp;
}

// TODO - dynamic frame
#define FRAME_SIZE 10

#define LOG_ON
#define ASM

// Programm counter
uint8_t pc = 0;

// Constant pull for method
// TODO Get Const pool dynamicly

// Method frame for method
uint64_t frame[FRAME_SIZE];
// TODO Move to local variable
register uint64_t* curr_frame asm ("t1");
// Frame pointer
uint8_t fp = 0;

void Init() {
    // 1. Clean intruction stack
    sp = 0;
    memset(stack, 0, STACK_SIZE);

    // 2. TODO - fill const pull
    memset(const_pull, 0, CONST_PULL_SIZE);

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
#ifdef ASM
                uint8_t tmp_0;
                uint64_t tmp_1;
                asm ("addi %0, %1, 1" :"=r"(sp) :"r"(sp));
                asm ("lb  %0, 0(%1)" :"=r"(tmp_0) :"r"((uint64_t)const_pull));
                asm ("add %0, %1, %2" :"=r"(tmp_1) :"r"(sp), "r"((uint64_t)stack));
                asm ("sb  %0, 0(%1)" :  :"r"(tmp_0), "r"(tmp_1));
#else
                // C-code:
                ++sp;
                stack[sp] = const_pull[0];
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
                printf("Unsupported instruction with bc = %d at pc = %d \n", opcode, pc);
#endif
                break;
        }
        ++pc;
    }
};



