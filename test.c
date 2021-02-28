#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <assert.h>
#include "vm_interface.h"
#include "opcodes.h"
#include "test.h"

extern uint64_t sp; // We should have access this variable for tests

int const TEST_COUNT = OPCODE_NUM;

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
           
            // Initialize VM
            Init();
            uint64_t prev_sp = GetSP();
            // Fill zero const pull by random value
            uint8_t rand_value = (rand());
            const_pull[0] = rand_value;
#ifdef LOG_ON
            printf("Enter to iconst_0 test\n\r");
            printf("  rand = %d\n\r", rand_value);
            printf("  sp = %ld\n\r", prev_sp);
#endif

            // Execute bc
            Execute(bytecodes);
#ifdef LOG_ON
            printf("Exit from execution iconst_0 test\n\r");
            printf("  stack[sp] = %ld\n\r", stack[GetSP()]);
            printf("  sp = %ld\n\r", GetSP());
#endif
            assert(GetSP() == prev_sp + 1);
            assert(stack[GetSP()] == rand_value);
            // Create bytecode:
            break; 
        }

        case (iand) : {
            uint8_t bytecodes[2] = {iand, return_};
           
            // Initialize VM
            Init();
            // Fill zero const pull by random value
            uint64_t rand_value_1 = (rand());
            uint64_t rand_value_2 = (rand());
            stack[++sp] = rand_value_1;
            stack[++sp] = rand_value_2;
            uint64_t prev_sp = GetSP();

#ifdef LOG_ON
            printf("Enter to iand test\n\r");
            printf("  rand_1 = %ld\n\r", rand_value_1);
            printf("  rand_2 = %ld\n\r", rand_value_2);
            printf("  sp = %ld\n\r", GetSP());
            printf("  Should result = %ld\n\r", rand_value_1 & rand_value_2);

#endif

            // Execute bc
            Execute(bytecodes);
#ifdef LOG_ON
            printf("Exit from execution iand test\n\r");
            printf("  stack[sp] = %ld\n\r", stack[GetSP()]);
            printf("  sp = %ld\n\r", GetSP());
#endif
            assert(GetSP() == prev_sp - 1);
            assert(stack[GetSP()] == (rand_value_1 & rand_value_2));
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




