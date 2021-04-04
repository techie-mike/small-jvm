#include <cstdio>
#include <cstdlib>
#include <ctime>

#include <cassert>
#include "vm_interface.h"
#include "opcodes.h"
#include "test.h"


int const TEST_COUNT = OPCODE_NUM;

void Test_iconst_i(uint8_t bytecodes[2], uint8_t i);

// Execution type and number of iterations (or test ID)
void RunTest(ExecType type, int it) {
    switch (type) {
        case (ONE_TEST) : {
            Test(it);

            #ifdef LOG_ON
            printf("test %d executed\n\n\r", it);
            #endif
            break;
        }

        case (DEFAULT_SEQUENCE) : {
            for(int j = 0; j < it; ++j) {
                for(int i = 0; i < TEST_COUNT; ++i) {
                    Test(i);
                }
            }
            #ifdef LOG_ON
            printf("defatult sequence - done\n\n\r");
            #endif
            break;
        }

        case (RANDOM_SEQUENCE) : {
            time_t t = time(NULL);
            srand(t);
            for(int i = 0; i < TEST_COUNT * it; ++i) {
                int id = rand() % TEST_COUNT;
                Test(id);
            }
            #ifdef LOG_ON
            printf("randomized testing - done\n\r");
            #endif
            break;
        }
    }
}


void Test(int id) {
    time_t t = time(NULL);
    srand(t);

    switch(id) {
        // ------------------------------------------------
        #define TEST_ICONST_I(num)\
        case(iconst_##num) : {\
            uint8_t bytecodes[2] = {iconst_##num, return_};\
            Test_iconst_i(bytecodes, num ## u);\
            break;\
        }

        TEST_ICONST_I(0)
        TEST_ICONST_I(1)
        TEST_ICONST_I(2)
        TEST_ICONST_I(3)
        TEST_ICONST_I(4)
        TEST_ICONST_I(5)

        #undef TEST_ICONST_I
        // ------------------------------------------------

        case (iand) : {
            TestIand();
            break;
        }

        case(iadd) : {
            TestIadd();
            break; 
        }

        case(invokestatic): {
            TestIinvokestatic();
            break;
        }
        // Test for "bipush", "ireturn" not necessary since
        // this test in "invokestatic"

        default:
            #ifdef LOG_ON
            printf("unsupported opcode %d \n\r", id);
            #endif
            break;
    }
}


void Test_iconst_i(uint8_t bytecodes[2], uint8_t i) {
    // Initialize VM
    JavaVM vm;

    uint64_t prev_sp = vm.GetSP();
    // Fill zero const pull by random value
    uint8_t rand_value = rand();
    vm.FillConstPull(i, rand_value);

    #ifdef LOG_ON
    printf("Enter to iconst_%u test\n\r", i);
    printf("  rand = %d\n\r", rand_value);
    printf("  sp = %ld\n\r", prev_sp);
    #endif

    // Execute bc
    vm.Execute(bytecodes);

    #ifdef LOG_ON
    printf("Exit from execution iconst_%u test\n\r", i);
    printf("  stack[sp] = %ld\n\r", vm.RetStackVal(vm.GetSP()));
    printf("  sp = %ld\n\r", vm.GetSP());
    #endif

    assert(vm.GetSP() == prev_sp + 1);
    assert(vm.RetStackVal(vm.GetSP()) == rand_value);
}


void TestIinvokestatic() {
    uint8_t bytecodes[4] = {invokestatic, 0, 0, return_};
    uint8_t val = rand();

    #ifdef LOG_ON
    printf("Enter to invokestatic test\n\r");
    printf("  Should result = %d\n\r", val);
    #endif

    JavaVM vm;
    vm.method_info_[0].code_info_.SetValues(50, val, 3);
    vm.FillConstPull (0, 
        reinterpret_cast<uint64_t> (&vm.method_info_[0]));

    uint8_t code[3] = {bipush, val, ireturn};

    vm.method_info_[0].code_info_.UploadCode(3, code);

    vm.Execute(bytecodes);
    #ifdef LOG_ON
    printf("  stack[sp] = %ld\n\r", vm.RetStackVal(vm.GetSP()));
    #endif
    assert(val == vm.RetStackVal(vm.GetSP()));
}

void TestIand() {
    uint8_t bytecodes[2] = {iand, return_};

    // Initialize VM
    JavaVM vm;

    // Fill zero const pull by random value
    uint64_t rand_value_1 = (rand());
    uint64_t rand_value_2 = (rand());

    vm.MoveSP(1);
    vm.SetStackVal(vm.GetSP(), rand_value_1);
    vm.MoveSP(1);
    vm.SetStackVal(vm.GetSP(), rand_value_2);

    uint64_t prev_sp = vm.GetSP();

    #ifdef LOG_ON
    printf("Enter to iand test\n\r");
    printf("  rand_1 = %ld\n\r", rand_value_1);
    printf("  rand_2 = %ld\n\r", rand_value_2);
    printf("  sp = %ld\n\r", vm.GetSP());
    printf("  Should result = %ld\n\r", rand_value_1 & rand_value_2);
    #endif

    // Execute bc
    vm.Execute(bytecodes);

    #ifdef LOG_ON
    printf("Exit from execution iand test\n\r");
    printf("  stack[sp] = %ld\n\r", vm.RetStackVal(vm.GetSP()));
    printf("  sp = %ld\n\r", vm.GetSP());
    #endif
    assert(vm.GetSP() == prev_sp - 1);
    fprintf (stderr, "stack[sp] = %ld  ", vm.RetStackVal(vm.GetSP()));
    assert(vm.RetStackVal(vm.GetSP()) == (rand_value_1 & rand_value_2));
}

void TestIadd() {
uint8_t bytecodes[2] = {iadd, return_};
    // Initialize VM
    JavaVM vm;

    uint32_t rv1, rv2;
    rv1 = rand() + rand();
    rv2 = rand() + rand();

    vm.MoveSP(1);
    vm.SetStackVal(vm.GetSP(), rv1);
    vm.MoveSP(1);
    vm.SetStackVal(vm.GetSP(), rv2);

    uint64_t prev_sp = vm.GetSP();

    #ifdef LOG_ON
    printf("Enter to iadd test\r\n");
    printf("  rv1 = %u,  rv2 = %u\r\n", rv1, rv2);
    printf("  sp = %lu\r\n", vm.GetSP());

    printf("  Should result = %u\n\r", rv1 + rv2);
    #endif

    // Execute bc
    vm.Execute(bytecodes);

    #ifdef LOG_ON
    printf("Exit from execution iadd test\r\n");
    printf("  stack[sp] = %lu\r\n", vm.RetStackVal(vm.GetSP()));
    printf("  sp = %lu\r\n", vm.GetSP());
    #endif
    assert(vm.GetSP() == prev_sp - 1);
    assert(vm.RetStackVal(vm.GetSP()) == rv1 + rv2);
}
