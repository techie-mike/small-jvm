#ifndef TEST_H
#define TEST_H

typedef enum {
    ONE_TEST,
    DEFAULT_SEQUENCE,
    RANDOM_SEQUENCE
} ExecType;

void RunTest(ExecType type, int it);

void Test(int it);

void TestIinvokestatic();
void TestIand();
void TestIadd();

#define LOG_ON

#endif
