// vm_interface.h
#ifndef VM_INTERFACE_H
#define VM_INTERFACE_H

#include <stdint.h>

#define CONST_PULL_SIZE 8
extern uint8_t const_pull[CONST_PULL_SIZE]; 

#define STACK_SIZE 255
// Programm stack
extern uint8_t stack[STACK_SIZE];

// Initialize stack and memory of VM
void Init();

// Execute bytecode sequence
void Execute(uint8_t* bc);

// Special getter for stack_pointer position
uint64_t GetSP();

#endif // VM_INTERFACE_H
