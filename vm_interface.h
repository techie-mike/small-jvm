// vm_interface.h
#ifndef VM_INTERFACE_H
#define VM_INTERFACE_H

#include <stdint.h>
#include <assert.h>

#define TEST

#define CONST_PULL_SIZE 8

#define STACK_SIZE 255

#define LOG_ON


class JavaVM {
    uint64_t const_pull_[CONST_PULL_SIZE]; 
    uint64_t stack_[STACK_SIZE] = {};

    uint64_t sp_;

    // Programm counter
    uint8_t pc_ = 0;


    // TODO - dynamic frame
    #define FRAME_SIZE 10

    // Constant pull for method
    // TODO Get Const pool dynamicly

    // Method frame for method
    uint64_t frame_[FRAME_SIZE];
    // TODO Move to local variable
    uint64_t* curr_frame_;

    // Frame pointer
    uint8_t fp_ = 0;


    //===========FUNCTION=FOR=USER===========
public:
    JavaVM();
    ~JavaVM() {};

    // Execute bytecode sequence
    void Execute(uint8_t* bc);

    // Special getter for stack_pointer position
    uint64_t GetSP();

    // For replacing stack point explicitly (for tests)
    void MoveSP(int i);

    // TODO rewrite define in command of VM
    // It is ONLY for test system without this commands
    uint64_t RetStackVal(uint64_t pos);
    void SetStackVal(uint64_t pos, uint64_t val);
    int FillConstPull(uint8_t num_pull, uint64_t value);

    //===========FUNCTION=FOR=USER===========

private:
    void Execute_iand();
    void Execute_iconst_i(uint8_t i);
    void Execute_iadd();

};

#endif // VM_INTERFACE_H
