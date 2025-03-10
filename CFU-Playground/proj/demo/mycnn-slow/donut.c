#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <cfu.h>
#include <inttypes.h>
#include <perf.h>
#include <console.h>
#include "./test_data.h"  // Include the file containing the test_data array
#include "./layer1_data.h"  // This directly includes the .c file with definitions
#include "./layer2_data.h"  // This directly includes the .c file with definitions
#include "./layer3_data.h"  // This directly includes the .c file with definitions
#include <stdint.h>


#define MulAdd(a,b)    opcode_R(0x33, 0x0, 0x1, a, b)
#define Mul4Add(a,b)     opcode_R(0x33, 0x1, 0x1, a, b)
#define QuantResc(a,b)     opcode_R(0x33, 0x2, 0x1, a, b)
#define ReluQuantResc(a,b)     opcode_R(0x33, 0x3, 0x1, a, b)
#define ShowCycles(a,b)     opcode_R(0x33, 0x4, 0x1, a, b)
#define ShowAllInst(a,b)     opcode_R(0x33, 0x5, 0x1, a, b)
#define ShowCpuInst(a,b)     opcode_R(0x33, 0x6, 0x1, a, b)
#define ShowRWInst(a,b)     opcode_R(0x33, 0x7, 0x1, a, b)

#define test_size 4500

#ifdef OPT_LINK_CODE_IN_SRAM
void donut(void) __attribute__((section(".ramtext")));
#else
void donut(void);
#endif

#define test_size 4500

int quantize_4bit(int value) {
    // Scaling the value by the constant scale factor
    int scaled_value = value / 32;

    // Quantizing to the range [-8, 7] by clipping the value
    if (scaled_value < -8) {
        scaled_value = -8;
    } else if (scaled_value > 7) {
        scaled_value = 7;
    }

    return scaled_value;
}

// Activation functions
int relu(int x) {
    return x > 0 ? x : 0;
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

void print_binary(const char *label, int32_t value) {
    printf("%s: ", label);
    for (int bit = 31; bit >= 0; bit--) {
        printf("%d", (value >> bit) & 1);
    }
    printf("\n");
}

// Forward pass through the network
void forward(int *input, int *output, int w1[][2], int w2[2224][4], int w3[4][5], int num, int b1[16], int b2[4], int b3[5]) {

   // printf("---------- Sample num: %d -----------\n", num);

    //Layer 1
    int conv_output[139][16];

        for (int i = 0; i < 139; i++) {
        for (int j = 0; j < 16; j++) {
            int input1 = input[i];
            int input2 = input[i + 1];
            int weight1 = w1[j/2][j%2];
            int weight2 = w1[j/2+8][j%2];

            conv_output[i][j] = relu(input1 * weight1 + input2 * weight2 + b1[j]);
        }
    }

    for (int i = 0; i < 139; i++) {
        for (int j = 0; j < 16; j++) {
            conv_output[i][j] = quantize_4bit(conv_output[i][j]);
        }
    }

    // Layer 2 - Dense (from conv_output to dense_output)

    int dense_output[4];

    for (int i = 0; i < 4; i++) {  // Iterate over 4 neurons in the dense layer
        for (int t = 0; t < 139; t++) {  // Iterate over timesteps in conv_output
            for (int f = 0; f < 16; f++) {  // Iterate over filters in conv_output
                dense_output[i] += conv_output[t][f] * w2[t * 16 + f][i];
            }
        }
        dense_output[i] = relu(dense_output[i] + b2[i]);
    }

    for (int i = 0; i < 4; i++) {
        dense_output[i] = quantize_4bit(dense_output[i]);
    }

    // Layer 3 - Dense (output size: 5)

    int logits[5];      // Logits before softmax

    for (int i = 0; i < 5; i++) {  // Iterate over the 5 output neurons
        for (int j = 0; j < 4; j++) {  // Iterate over the 4 neurons in the dense layer
            logits[i] += dense_output[j] * w3[j][i];
        }
        logits[i] += b3[i];
    }

    for (int i = 0; i < 5; i++) {
        logits[i] = quantize_4bit(logits[i]);
    }

    // Determine the predicted class using argmax
    *output = argmax(logits, 5);
}


void donut(void) {
    // Evaluate the test data
    int correct = 0;
    int start_cycles = ShowCycles(0,0);
    int start_inst = ShowAllInst(0,0);
    int start_cpuinst = ShowCpuInst(0,0);
    int start_rwinst = ShowRWInst(0,0);

    for (int i = 0; i < 1; i++) {
        int output;
        forward(test_data[i], &output, layer1_weights, layer2_weights, layer3_weights, i+1, layer1_biases, layer2_biases, layer3_biases);

        // Check if the prediction is correct
        if (test_data[i][140] == output + 1) {
            correct++;
        }
    }

    int end_inst = ShowAllInst(0,0);
    int end_cpuinst = ShowCpuInst(0,0);
    int end_rwinst = ShowRWInst(0,0);
    int end_cycles = ShowCycles(0,0);

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
