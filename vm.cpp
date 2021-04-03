#include <cstdio>
#include <cstring>
#include "vm_interface.h"

#include "opcodes.h"


JavaVM::JavaVM() {
    // 1. Clean intruction stack
    sp_ = 0;
    memset(stack_, 0, STACK_SIZE * sizeof(stack_[0]));

    // 2. TODO - fill const pull
    memset(const_pull_, 0, CONST_PULL_SIZE * sizeof(const_pull_[0]));

    // 3. Clean Frame
    // TODO - support dynamic frames 
    curr_frame_ = frame_;

    // 4. Initialize pc
    pc_ = 0;
}

void JavaVM::Execute(uint8_t* bc) {
    // TODO Stack owerflow
    // TODO Garbage collection
    while (1) {
        uint8_t opcode = bc[pc_];

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
                Execute_iand();
	            break;
            }
            case(iadd) : {
                Execute_iadd();
                break;
            }

            case(return_) : {
                // TODO support frame removing and return from methods
                if (fp_ == 0) {
                return;
                } else {
                    assert(fp_ < FRAME_SIZE);
                    --fp_;
                }
                break;
            }

            default:
#ifdef LOG_ON
            printf("Unsupported instruction with bc = %d at pc = %d \n\r", opcode, pc_);
#endif
            break;
        }
        ++pc_;
    }
    assert(sp_ == GetSP());
}

void JavaVM::Execute_iand() {
    // Operand Stack:
    // ..., value_1, value_2, ->
    // ..., result

#ifdef ASM
    uint64_t value_1 = 0, value_2 = 0, result = 0;
    uint64_t offset = 0;

    // Load value_1
    asm ("mul %0, %1, %2" : "=r"(offset) : "r"(sp_), "r"(sizeof(stack_[0])));
    asm ("add %0, %1, %2" : "=r"(value_1)  : "r"(offset), "r"(stack_));

    asm ("ld %0, 0(%1)" : "=r"(value_1): "r"(value_1));

    asm ("li %0, 1" : "=r"(offset));
    asm ("sub %0, %1, %2" :"=r"(sp_) :"r"(sp_), "r"(offset));

    // Load value_2
    asm ("mul %0, %1, %2" : "=r"(offset) : "r"(sp_), "r"(sizeof(stack_[0])));
    asm ("add %0, %1, %2" : "=r"(value_2)  : "r"(offset), "r"(stack_));

    asm ("ld %0, 0(%1)" : "=r"(value_2): "r"(value_2));

    asm ("and %0, %1, %2" : "=r"(result) : "r"(value_1), "r"(value_2));

    // Write result
    asm ("mul %0, %1, %2" : "=r"(offset) : "r"(sp_), "r"(sizeof(stack_[0])));
    asm ("add %0, %1, %2" : "=r"(offset)  : "r"(offset), "r"(stack_));

    asm ("sd  %0, 0(%1)"  :  : "r"(result), "r"(offset));

#else
    // C-code:
    uint64_t value_1 = 0, value_2 = 0, result = 0;

    // "stack" type is "unit64_t"
    value_1 = (uint64_t) stack_[sp_ - 1];
    value_2 = (uint64_t) stack_[sp_];
    result = value_1 & value_2;
    sp_ -= 1;
    stack_[sp_] = result;

#endif
}

void JavaVM::Execute_iconst_i(uint8_t i) {
#ifdef ASM
  // ASM CODE DIDN'T TEST, AFTER SWITCH ON "uint64_t"
  uint64_t tmp_0 = 0;
  uint64_t tmp_1 = 0;
  uint64_t offset = 0;

  asm ("addi %0, %1, 1" : "=r"(sp_) :"r"(sp_));
  asm ("mul %0, %1, %2" : "=r"(i) : "r"(i), "r"(sizeof(const_pull_[0])));
  asm ("add %0, %1, %2" : "=r"(tmp_0) :"r"(const_pull_), "r"(i));

  asm ("ld  %0, 0(%1)" : "=r"(tmp_0) : "r"(tmp_0));

  asm ("mul %0, %1, %2" : "=r"(offset) : "r"(sp_), "r"(sizeof(stack_[0])));
  asm ("add %0, %1, %2" : "=r"(tmp_1)  : "r"(offset), "r"(stack_));
  asm ("sd  %0, 0(%1)"  :  : "r"(tmp_0), "r"(tmp_1));

#else
  // C-code:
  ++sp_;
  stack_[sp_] = const_pull_[i];
#endif
}

void JavaVM::Execute_iadd() {
#ifdef ASM
    uint64_t a1, a2;
    uint64_t p1, p2;
    asm ("add %0, %1, %2" :"=r"(p2) :"r"(sp_ * 8), "r"((uint64_t)stack_));
    asm ("lw  %0, 0(%1)" :"=r"(a2) :"r"(p2));
    asm ("addi %0, %1, -1" :"=r"(sp_) :"r"(sp_));
    asm ("add %0, %1, %2" :"=r"(p1) :"r"(sp_ * 8), "r"((uint64_t)stack_));
    asm ("lw  %0, 0(%1)" :"=r"(a1) :"r"(p1));
    asm ("addw %0, %1, %2" :"=r"(a1) :"r"(a1), "r"(a2));
    asm ("sw %0, 0(%1)" :  :"r"(a1), "r"(p1));
#else
    --sp_;
    stack_[sp_] = (stack_[sp_] + stack_[sp_ + 1]) % UINT32_MAX;
#endif

}

uint64_t JavaVM::GetSP() {
    return sp_;
}

void JavaVM::MoveSP(int i) {
    if (i >= 0) {
        sp_ += static_cast<uint64_t>(i);
    }
    else {
        i = -i;
        assert(sp_ > static_cast<uint64_t>(i));
        sp_ -= static_cast<uint64_t>(i);
    }
}

int JavaVM::FillConstPull(uint8_t num_pull, uint64_t value) {
    if (num_pull >= CONST_PULL_SIZE)
        return -1;
    else {
        const_pull_[num_pull] = value;
    }
}

uint64_t JavaVM::RetStackVal(uint64_t pos) {
    assert(pos <= sp_);
    return stack_[pos];
}

void JavaVM::SetStackVal(uint64_t pos, uint64_t val) {
    stack_[pos] = val;
}
