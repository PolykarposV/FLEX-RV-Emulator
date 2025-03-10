#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <cfu.h>
#include <inttypes.h>
#include <perf.h>
#include <console.h>
#include "./flo_input.h"  // Include the file containing the test_data array
#include "./flo_layer1.h"  // This directly includes the .c file with definitions
#include "./flo_layer2.h"  // This directly includes the .c file with definitions
#include <stdint.h>


#define MulAdd(a,b)    opcode_R(0x30, 0x0, 0x1, a, b)
#define Mul4Add(a,b)     opcode_R(0x30, 0x1, 0x1, a, b)
#define QuantResc(a,b)     opcode_R(0x30, 0x2, 0x1, a, b)
#define ReluQuantResc(a,b)     opcode_R(0x30, 0x3, 0x1, a, b)
#define ShowCycles(a,b)     opcode_R(0x30, 0x4, 0x1, a, b)
#define ShowAllInst(a,b)     opcode_R(0x30, 0x5, 0x1, a, b)
#define ShowCpuInst(a,b)     opcode_R(0x30, 0x6, 0x1, a, b)
#define ShowRWInst(a,b)     opcode_R(0x30, 0x7, 0x1, a, b)

#define test_size 4500

#ifdef OPT_LINK_CODE_IN_SRAM
void donut(void) __attribute__((section(".ramtext")));
#else
void donut(void);
#endif

#define test_size 4500

void print_binary(const char *label, int32_t value) {
    printf("%s: ", label);
    for (int bit = 31; bit >= 0; bit--) {
        printf("%d", (value >> bit) & 1);
    }
    printf("\n");
}

int argmax(int *values, int len) {
    int max_index = 0;
    for (int i = 1; i < len; i++) {
        if (values[i] > values[max_index]) {
            max_index = i;
        }
    }

    return max_index;
}

// Forward pass through the network
void forward(int *input, int *output, int w1[][45], int w2[3][20], int num, int b1[20], int b2[3]) {

    printf("---------- Sample num: %d -----------\n", num);

    uint32_t a = 0;
    uint32_t b = 0;

    // Layer 1 - Dense
    int dense_output[20];

    for (int i = 0; i < 20; i++) {  // Iterate over 4 neurons in the dense layer
        dense_output[i] = b1[i];
    	for (int j = 0; j < 44; j=j+2) {
	    a = 0;
            b = 0;

            // Construct operand 'a'
            a |= ((w1[i][j+1] & 0xFF) << 8); // Set bits 8-15 input2, in this case for weight1[j+1]
            a |= ((w1[i][j] & 0xFF) << 16);    // Set bits 16-23 input1, in this case for weight1[j]

            // Construct operand 'b'
            b |= ((input[j+1] & 0xF) << 8);  // Set bits 8-11 weight2, in this case for input[j+1]
            b |= ((input[j] & 0xF) << 12);     // Set bits 15-12 weight1, in this case for input[j]

            dense_output[i] += MulAdd(a,b);
    	}
	dense_output[i] += input[44] * w1[i][44]; //Last iteration goes here
        dense_output[i] = (dense_output[i] > 0)? dense_output[i] : 0;
     }

    // Layer 2 - Dense (output size: 5)
    int logits[3];      // Logits before softmax

    for (int i = 0; i < 3; i++) {  // Iterate over the 5 output neurons
        logits[i] = b2[i];
	for (int j = 0; j < 20; j++) {
                logits[i] += dense_output[j] * w2[i][j];
        }
	logits[i] = (logits[i] > 0)? logits[i] : 0;
    }

    // Determine the predicted class using argmax
    *output = argmax(logits, 3);
}


void donut(void) {
    // Evaluate the test data
    int correct = 0;
    int start_cycles = ShowCycles(0,0);
    int start_inst = ShowAllInst(0,0);
    int start_cpuinst = ShowCpuInst(0,0);
    int start_rwinst = ShowRWInst(0,0);

    for (int i = 0; i < 23; i++) {
        int output;
        forward(flo_input[i], &output, flo_layer1, flo_layer2, i+1, flo_biases1, flo_biases2);
        // Check if the prediction is correct
        if (flo_input[i][45] == output) {
            correct++;
        }
    }

    int end_cycles = ShowCycles(0,0);
    int end_inst = ShowAllInst(0,0);
    int end_cpuinst = ShowCpuInst(0,0);
    int end_rwinst = ShowRWInst(0,0);

    int cycles = end_cycles - start_cycles;
    int instructions = end_inst - start_inst;
    int cpu_instructions = end_cpuinst - start_cpuinst;
    int cfu_instructions = instructions - cpu_instructions;
    int rw_instructions = end_rwinst - start_rwinst;

    printf("Cycles passed %d \n", cycles);
    printf("Instruction executed %d \n", instructions);
    printf("CPU instructions %d \n", cpu_instructions);
    printf("CFU instructions %d \n", cfu_instructions);
    printf("RW instructions %d \n", rw_instructions);
    printf("Correct Predictions %d out of 4500 samples: \n", correct);
    //printf("Evaluation ended: \n");
}
