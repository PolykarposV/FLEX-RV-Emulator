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
void forward(int *input, int *output, int w1[][2], int w2[2224][4], int w3[4][5], int num, int b1[16], int b2[4], int b3[5]) {

    printf("---------- Sample num: %d -----------\n", num);

    //Layer 1
    int conv_output[139][16] = {0};

    // Initialize operands
    uint32_t a = 0;
    uint32_t b = 0;

    for (int i = 0; i < 139; i++) {
        for (int j = 0; j < 16; j++) {

	a = 0;
	b = 0;

            // Construct operand 'a'
	    a |= ((input[i+1] & 0xFF) << 8); // Set bits 8-15 input2
    	    a |= ((input[i] & 0xFF) << 16);    // Set bits 16-23 input1

	    // Construct operand 'b'
	    b |= ((w1[j / 2 + 8][j % 2] & 0xF) << 8);  // Set bits 8-11 weight2
    	    b |= ((w1[j / 2][j % 2] & 0xF) << 12);     // Set bits 15-12 weight1

            conv_output[i][j] = MulAdd(a,b);

        }
    }

    for (int i = 0; i < 139; i++) {
        for (int j = 0; j < 16; j++) {
            conv_output[i][j] = ReluQuantResc(conv_output[i][j],b1[j]);
	}
    }

    // Layer 2 - Dense (from conv_output to dense_output)
    int dense_output[4];

    for (int i = 0; i < 4; i++) {  // Iterate over 4 neurons in the dense layer
        dense_output[i] = 0;
        for (int t = 0; t < 139; t++) {  // Iterate over timesteps in conv_output
            for (int f = 0; f < 16; f += 4) {  // Process 4 filters at once
		a = 0;
    		b = 0;

		// Construct operand 'a'
		a |= ((conv_output[t][f+3] & 0xF)); 	// Set bits 0-3 input4
                a |= ((conv_output[t][f+2] & 0xF) << 4);    // Set bits 4-7 input3
            	a |= ((conv_output[t][f+1] & 0xF) << 8); 	// Set bits 8-15 0000input2
            	a |= ((conv_output[t][f] & 0xF) << 16);    	// Set bits 16-23 0000input1

		// Construct operand 'b'
		b |= ((w2[t * 16 + f + 3][i] & 0xF));      	// Set bits 0-3 to weight4
                b |= ((w2[t * 16 + f + 2][i] & 0xF) << 4);      // Set bits 4-7 weight3
            	b |= ((w2[t * 16 + f + 1][i] & 0xF) << 8);      // Set bits 8-11 weight2
            	b |= ((w2[t * 16 + f][i] & 0xF) << 12);         // Set bits 15-12 weight1


        	dense_output[i] += Mul4Add(a,b);

	     }
	}

        dense_output[i] = ReluQuantResc(dense_output[i],b2[i]);

    }

    // Layer 3 - Dense (output size: 5)
    int logits[5];      // Logits before softmax

    for (int i = 0; i < 5; i++) {  // Iterate over the 5 output neurons
        logits[i] = 0;

	a = 0;
        b = 0;

	// Construct operand 'a'
        a |= ((dense_output[3] & 0xF));             // Set bits 0-3 input4
        a |= ((dense_output[2] & 0xF) << 4);        // Set bits 4-7 input3
        a |= ((dense_output[1] & 0xF) << 8);        // Set bits 8-15 0000input2
        a |= ((dense_output[0] & 0xF) << 16);       // Set bits 16-23 0000input1

	// Construct operand 'b'
        b |= ((w3[3][i] & 0xF));           // Set bits 0-3 to weight4
        b |= ((w3[2][i] & 0xF) << 4);      // Set bits 4-7 weight3
        b |= ((w3[1][i] & 0xF) << 8);      // Set bits 8-11 weight2
        b |= ((w3[0][i] & 0xF) << 12);     // Set bits 15-12 weight1

	logits[i] = Mul4Add(a,b);

        logits[i] = QuantResc(logits[i],b3[i]);
    }

    // Determine the predicted class using argmax
    *output = argmax(logits, 5);
}


void donut(void) {
    // Evaluate the test data
    int correct = 0;
    int start_cycles = ShowCycles(0,0);
    int start_inst = ShowAllInst(0,0);
    int end_cycles = ShowCycles(0,0);
    int start_cpuinst = ShowCpuInst(0,0);
    int start_rwinst = ShowRWInst(0,0);

    for (int i = 0; i < 500; i++) {
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
