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
        case(iconst_0) : {
            uint8_t bytecodes[2] = {iconst_0, return_};
            Test_iconst_i(bytecodes, 0u);
            break;
        }

        case(iconst_1) : {
            uint8_t bytecodes[2] = {iconst_1, return_};
            Test_iconst_i(bytecodes, 1u);
            break;
        }

        case(iconst_2) : {
            uint8_t bytecodes[2] = {iconst_2, return_};
            Test_iconst_i(bytecodes, 2u);
            break;
        }

        case(iconst_3) : {
            uint8_t bytecodes[2] = {iconst_3, return_};
            Test_iconst_i(bytecodes, 3u);
            break;
        }

        case(iconst_4) : {
            uint8_t bytecodes[2] = {iconst_4, return_};
            Test_iconst_i(bytecodes, 4u);
            break;
        }

        case(iconst_5) : {
            uint8_t bytecodes[2] = {iconst_5, return_};
            Test_iconst_i(bytecodes, 5u);
            break;
        }

        case (iand) : {
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
            assert(vm.RetStackVal(vm.GetSP()) == (rand_value_1 & rand_value_2));
            // Create bytecode:
            break;
        }

        case(iadd) : {
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

            printf("  Should result = %ld\n\r", 
                    static_cast<uint64_t>(rv1 + rv2));
#endif

            // Execute bc
            vm.Execute(bytecodes);
#ifdef LOG_ON
            printf("Exit from execution iadd test\r\n");
            printf("  stack[sp] = %lu\r\n", vm.RetStackVal(vm.GetSP()));
            printf("  sp = %lu\r\n", vm.GetSP());
#endif
            assert(vm.GetSP() == prev_sp - 1);
            assert((uint32_t) vm.RetStackVal(vm.GetSP()) == rv1 + rv2);
            // Create bytecode:
            break; 
        }

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
    uint8_t rand_value = (rand());
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
