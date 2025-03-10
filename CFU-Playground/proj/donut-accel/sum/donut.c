#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <cfu.h>
#include <inttypes.h>
#include <perf.h>
#include <console.h>
#include <stdint.h>
#include <limits.h>  // For INT_MAX and INT_MIN



#define MulAdd(a,b)    opcode_R(0x30, 0x0, 0x1, a, b)
#define Mul4Add(a,b)     opcode_R(0x30, 0x1, 0x1, a, b)
#define QuantResc(a,b)     opcode_R(0x30, 0x2, 0x1, a, b)
#define ReluQuantResc(a,b)     opcode_R(0x30, 0x3, 0x1, a, b)
#define ShowCycles(a,b)     opcode_R(0x30, 0x4, 0x1, a, b)
#define ShowAllInst(a,b)     opcode_R(0x30, 0x5, 0x1, a, b)
#define ShowCpuInst(a,b)     opcode_R(0x30, 0x6, 0x1, a, b)
#define ShowRWInst(a,b)     opcode_R(0x30, 0x7, 0x1, a, b)


#ifdef OPT_LINK_CODE_IN_SRAM
void donut(void) __attribute__((section(".ramtext")));
#else
void donut(void);
#endif

#define array_size 128

void donut() { //32 bits

    int start_cycles,end_cycles;
    int sum;


    // 32-bits

    int arr_32bit[128] = {
    3, -22, 5, -100, 1, 66, -33, 77, -55, 44, 0, -11, 22, -66, -4, -127,
    54, -89, 17, 23, -44, 9, -15, 88, -100, 67, 56, 34, -12, 25, -42, 13,
    -99, 78, 65, 28, 89, -31, -20, -55, 60, 101, 41, 92, -6, -36, 44, 14,
    -10, 29, -2, -77, -20, 62, -50, 80, 4, 32, -23, 75, 36, 0, -67, 19,
    -12, 7, 39, -1, 11, 58, 27, 73, -15, 50, 90, -68, -8, 45, 82, 19,
    67, -60, -39, 81, 96, -88, 49, -16, 71, 37, -75, 15, -80, -58, 41, 63,
    -46, 55, -101, 38, 0, 69, -34, -26, 18, 83, -62, 46, -13, -74, 22, 81,
    -100, -99, -8, 64, 24, 65, -10, -51, 30, 74, -17, 10, 84, 56, -30, 6
    };

    start_cycles = ShowCycles(0,0);

    // Find sum value
    sum = 0;

    // Calculate the sum with overflow protection
    for (int i = 0; i < 128; i++) {
        if (sum > 0 && INT_MAX - sum < arr_32bit[i]) {
            sum = INT_MAX;  // Clamp to INT_MAX if overflow would occur
        } else if (sum < 0 && INT_MIN - sum > arr_32bit[i]) {
            sum = INT_MIN;  // Clamp to INT_MIN if underflow would occur
        } else
            sum += arr_32bit[i];
    }

    end_cycles = ShowCycles(0,0);
    printf("SUM IS %d\n", sum);
    printf("Cycles passed %d\n", end_cycles - start_cycles);
}


