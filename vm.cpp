#include <cstdio>
#include <cstring>
#include "vm_interface.h"

#include "opcodes.h"


JavaVM::JavaVM() {
    // 1. TODO - fill const pull
    memset(const_pull_, 0, CONST_PULL_SIZE * sizeof(const_pull_[0]));
    // TODO - support dynamic frames 

    // 2. Initialize pc
    pc_ = 0;
    CreateFirstFrame();
}

JavaVM::~JavaVM() {
    DeleteFirstFrame();
}

void JavaVM::DeleteFirstFrame() {
    delete frame_[fp_];
    frame_[fp_] = nullptr;
}

void JavaVM::Execute(uint8_t* bc) {
    // TODO Stack owerflow
    // TODO Garbage collection
    while (1) {
        uint8_t opcode = bc[pc_];

        switch (opcode) {
            //------------------------------
            #define ICONST_I(num)\
            case(iconst_##num) : {\
                Execute_iconst_i(num ## u);\
                break;\
            }

            ICONST_I(0)
            ICONST_I(1)
            ICONST_I(2)
            ICONST_I(3)
            ICONST_I(4)
            ICONST_I(5)

            #undef ICONST_I
            //------------------------------

            case(iand) : {
                Execute_iand();
	            break;
            }

            case(iadd) : {
                Execute_iadd();
                break;
            }

            case(bipush) : {
                Execute_bipush(bc);
                break;
            }

            case(return_) : {
                // TODO support frame removing and return from methods
                if (fp_ == 0) {
                    return;
                }
                break;
            }

            case(invokestatic) : {
                Execute_invokestatic(bc);
                break;
            }

            case(ireturn) : {
                uint64_t ret = curr_frame_->operand_stack_[curr_frame_->sp_];
                curr_frame_->sp_--;
                frame_[fp_ - 1]->sp_++;
                frame_[fp_ - 1]->operand_stack_[frame_[fp_ - 1]->sp_] = ret;
                return;
                break;
            }

            default: {
                #ifdef LOG_ON
                printf("Unsupported instruction with bc = %d at pc = %d \n\r", opcode, pc_);
                #endif
                break;
            }
        }
        ++pc_;
    }
}

void JavaVM::Execute_iand() {
    // Operand Stack:
    // ..., value_1, value_2, ->
    // ..., result

#ifdef ASM
    uint64_t value_1 = 0, value_2 = 0, result = 0;
    uint64_t offset = 0;

    // Load value_1
    asm ("mul %0, %1, %2" : "=r"(offset) : "r"(curr_frame_->sp_), "r"(sizeof(curr_frame_->operand_stack_[0])));
    asm ("add %0, %1, %2" : "=r"(value_1)  : "r"(offset), "r"(curr_frame_->operand_stack_));

    asm ("ld %0, 0(%1)" : "=r"(value_1): "r"(value_1));

    asm ("li %0, 1" : "=r"(offset));
    asm ("sub %0, %1, %2" :"=r"(curr_frame_->sp_) :"r"(curr_frame_->sp_), "r"(offset));

    // Load value_2
    asm ("mul %0, %1, %2" : "=r"(offset) : "r"(curr_frame_->sp_), "r"(sizeof(curr_frame_->operand_stack_[0])));
    asm ("add %0, %1, %2" : "=r"(value_2)  : "r"(offset), "r"(curr_frame_->operand_stack_));

    asm ("ld %0, 0(%1)" : "=r"(value_2): "r"(value_2));

    asm ("and %0, %1, %2" : "=r"(result) : "r"(value_1), "r"(value_2));

    // Write result
    asm ("mul %0, %1, %2" : "=r"(offset) : "r"(curr_frame_->sp_), "r"(sizeof(curr_frame_->operand_stack_[0])));
    asm ("add %0, %1, %2" : "=r"(offset)  : "r"(offset), "r"(curr_frame_->operand_stack_));

    asm ("sd  %0, 0(%1)"  :  : "r"(result), "r"(offset));

#else
    // C-code:
    uint64_t value_1 = 0, value_2 = 0, result = 0;

    // "stack" type is "unit64_t"
    value_1 = (uint64_t) curr_frame_->operand_stack_[curr_frame_->sp_ - 1];
    value_2 = (uint64_t) curr_frame_->operand_stack_[curr_frame_->sp_];
    result = value_1 & value_2;

    curr_frame_->sp_ -= 1;
    curr_frame_->operand_stack_[curr_frame_->sp_] = result;

#endif
}

void JavaVM::Execute_iconst_i(uint8_t i) {
#ifdef ASM
  // ASM CODE DIDN'T TEST, AFTER SWITCH ON "uint64_t"
  uint64_t tmp_0 = 0;
  uint64_t tmp_1 = 0;
  uint64_t offset = 0;

  asm ("addi %0, %1, 1" : "=r"(curr_frame_->sp_) :"r"(curr_frame_->sp_));
  asm ("mul %0, %1, %2" : "=r"(i) : "r"(i), "r"(sizeof(const_pull_[0])));
  asm ("add %0, %1, %2" : "=r"(tmp_0) :"r"(const_pull_), "r"(i));

  asm ("ld  %0, 0(%1)" : "=r"(tmp_0) : "r"(tmp_0));

  asm ("mul %0, %1, %2" : "=r"(offset) : "r"(curr_frame_->sp_), "r"(sizeof(curr_frame_->operand_stack_[0])));
  asm ("add %0, %1, %2" : "=r"(tmp_1)  : "r"(offset), "r"(curr_frame_->operand_stack_));
  asm ("sd  %0, 0(%1)"  :  : "r"(tmp_0), "r"(tmp_1));

#else
  // C-code:
  curr_frame_->sp_++;
  curr_frame_->operand_stack_[curr_frame_->sp_] = reinterpret_cast<uint64_t> (const_pull_[i]);
#endif
}

void JavaVM::Execute_iadd() {
#ifdef ASM
    uint64_t a1, a2;
    uint64_t p1, p2;
    asm ("add %0, %1, %2" :"=r"(p2) :"r"(curr_frame_->sp_ * 8), "r"((uint64_t)curr_frame_->operand_stack_));
    asm ("lw  %0, 0(%1)" :"=r"(a2) :"r"(p2));
    asm ("addi %0, %1, -1" :"=r"(curr_frame_->sp_) :"r"(curr_frame_->sp_));
    asm ("add %0, %1, %2" :"=r"(p1) :"r"(curr_frame_->sp_ * 8), "r"((uint64_t)curr_frame_->operand_stack_));
    asm ("lw  %0, 0(%1)" :"=r"(a1) :"r"(p1));
    asm ("addw %0, %1, %2" :"=r"(a1) :"r"(a1), "r"(a2));
    asm ("sw %0, 0(%1)" :  :"r"(a1), "r"(p1));
#else
    curr_frame_->sp_--;
    curr_frame_->operand_stack_[curr_frame_->sp_] = 
        (curr_frame_->operand_stack_[curr_frame_->sp_] 
        + curr_frame_->operand_stack_[curr_frame_->sp_ + 1]) % UINT32_MAX;
#endif

}

void JavaVM::Execute_invokestatic(uint8_t* bc) {
    uint32_t index_function = (bc[pc_ + 1] << 8) | bc[pc_ + 2];
    bc += 2;

    MethodInfo* info = reinterpret_cast<MethodInfo*>(const_pull_[index_function]);    
    if (info->code_info_.code_ == nullptr) {
        printf ("Info don't have code\n\t");
        throw;
    }

    CreateFrame(info);
    Execute(info->code_info_.code_);
    DeleteFrame();
}

void JavaVM::Execute_bipush(uint8_t* bc) {
    uint8_t byte = bc[++pc_];
    curr_frame_->sp_++;
    curr_frame_->operand_stack_[curr_frame_->sp_] = byte;
}

void JavaVM::CreateFrame(MethodInfo* info) {
    fp_++;
    Frame* new_frame = new Frame;
    frame_[fp_] = new_frame;
    new_frame->CreateStackAndLocalVars(info->code_info_.max_stack_,
                                       info->code_info_.max_locals_);
    curr_frame_ = new_frame;
}

void JavaVM::DeleteFrame() {
    delete curr_frame_;
    fp_--;
    curr_frame_ = frame_[fp_];
}

void JavaVM::CreateFirstFrame() {
    curr_frame_ = new Frame;
    frame_[fp_] = curr_frame_; 
    curr_frame_->CreateStackAndLocalVars(STACK_SIZE, LOCAL_SIZE);
}


uint64_t JavaVM::GetSP() {
    return curr_frame_->sp_;
}

void JavaVM::MoveSP(int i) {
    if (i >= 0) {
        curr_frame_->sp_ += static_cast<uint64_t>(i);
    }
    else {
        i = -i;
        assert(curr_frame_->sp_ > static_cast<uint64_t>(i));
        curr_frame_->sp_ -= static_cast<uint64_t>(i);
    }
}

int JavaVM::FillConstPull(uint8_t num_pull, uint64_t value) {
    if (num_pull >= CONST_PULL_SIZE)
        return -1;
    else {
        const_pull_[num_pull] = reinterpret_cast<uint64_t*> (value);
        return 0;
    }
}

uint64_t JavaVM::RetStackVal(uint64_t pos) {
    assert(pos <= curr_frame_->sp_);
    return curr_frame_->operand_stack_[pos];
}

void JavaVM::SetStackVal(uint64_t pos, uint64_t val) {
    curr_frame_->operand_stack_[pos] = val;
}

JavaVM::MethodInfo::CodeAttribute::CodeAttribute() :
    max_stack_(0),
    max_locals_(0),
    code_length_(0),
    code_(nullptr) {}; 

JavaVM::MethodInfo::CodeAttribute::~CodeAttribute() {
    delete [] code_;
};

void JavaVM::MethodInfo::CodeAttribute::UploadCode(
        uint32_t size, uint8_t* code) {
    if (size > code_length_) {
        printf ("Error, size of code more than code_length!\n");
        throw;
    }
    code_ = new uint8_t [size];
    memcpy (code_, code, size);
}

JavaVM::Frame::Frame() :
    size_operand_stack_ (0),
    operand_stack_ (nullptr),
    return_value_ (0),
    size_local_variable_ (0),
    local_variable_ (nullptr),
    sp_ (0) {};

JavaVM::Frame::Frame(uint16_t size_stack,
                     uint16_t size_locals) :
        size_operand_stack_(size_stack),
        size_local_variable_(size_locals)
{
    operand_stack_  = new uint64_t [size_operand_stack_];
    local_variable_ = new uint8_t [size_local_variable_];
};

JavaVM::Frame::~Frame() {
    delete [] operand_stack_;
    delete [] local_variable_;
}

void JavaVM::Frame::CreateStackAndLocalVars(uint16_t size_stack,
                                            uint16_t size_locals) {
    if (operand_stack_ || local_variable_)
        throw;

    size_operand_stack_ = size_stack;
    size_local_variable_= size_locals;

    operand_stack_  = new uint64_t [size_operand_stack_];
    local_variable_ = new uint8_t [size_local_variable_];
}

void JavaVM::MethodInfo::CodeAttribute::SetValues(uint16_t max_stack,
                                                  uint16_t max_locals,
                                                  uint32_t code_length) {
    max_stack_   = max_stack;
    max_locals_  = max_locals;
    code_length_ = code_length;
}
