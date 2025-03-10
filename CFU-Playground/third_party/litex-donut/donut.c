#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <perf.h>
#include <math.h>
#include <cfu.h>
#include <perf.h>
#include <console.h>
#include "./test_data.h"  // Include the file containing the test_data array
#include "./layer1_data.h"  // This directly includes the .c file with definitions
#include "./layer2_data.h"  // This directly includes the .c file with definitions
#include "./layer3_data.h"  // This directly includes the .c file with definitions
#include <stdint.h>
#include <time.h>

#define test_size 4500

#ifdef OPT_LINK_CODE_IN_SRAM
void donut(void) __attribute__((section(".ramtext")));
#else
void donut(void);
#endif

int8_t quantize_4bit(int32_t value) {
    // Scaling the value by the constant scale factor
    int32_t scaled_value = value / 32;

    // Quantizing to the range [-8, 7] by clipping the value
    if (scaled_value < -8) {
        scaled_value = -8;
    } else if (scaled_value > 7) {
        scaled_value = 7;
    }

    return (int8_t)scaled_value;
}

// Activation functions
int relu(int x) {
    return x > 0 ? x : 0;
}

int argmax(float *values, int len) {
    int max_index = 0;
    for (int i = 1; i < len; i++) {
        if (values[i] > values[max_index]) {
            max_index = i;
        }
    }
    return max_index;
}

// Forward pass through the network
void forward(int *input, int *output, int w1[][2], int w2[2224][4], int w3[4][5],int num) {

    //printf("-----------Sample %d----------\n", num);

    //Layer 1
    int conv_output[139][16];

    for (int i = 0; i < 139; i++) {
        for (int j = 0; j < 16; j++) {
            int input1 = input[i];
            int input2 = input[i + 1];
            int weight1 = w1[j/2][j%2];
            int weight2 = w1[j/2+8][j%2];

            conv_output[i][j] = relu(input1 * weight1 + input2 * weight2);
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
        dense_output[i] = 0;

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
        logits[i] = 0;
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
   // printf("Evaluation started: \n");
    uint64_t start_cycles = perf_get_mcycle();
    // Evaluate the test data
    int correct = 0;
    for (int i = 0; i < 1; i++) {
        int output;
        forward(test_data[i], &output, layer1_weights, layer1_biases, layer2_weights, layer2_biases, layer3_weights, layer3_biases, i+1);

        // Check if the prediction is correct
        if ((int)test_data[i][140] == output + 1) {
            correct++;
        }
    }
   // printf("Correct Predictions %d out of 4500 samples: \n", correct);
    uint64_t end_cycles = perf_get_mcycle();
    printf("%lu cycles.\n", end_cycles - start_cycles);
   // printf("Evaluation ended: \n");
}
