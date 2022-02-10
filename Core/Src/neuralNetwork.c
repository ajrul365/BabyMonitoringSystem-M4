/*
 * neuralNetwork.cpp
 *
 *  Created on: 9 Jan 2022
 *      Author: mzuber
 */

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "neuralNetwork.h"
#define NUM_INPUT 2
#define NUM_HIDDEN_NODE 2
#define NUM_OUTPUT 1
#define LEARNING_RATE 0.1f // learning rate
#define NUM_TRAIN_SET 4
double hiddenLayer[NUM_HIDDEN_NODE];
double outputLayer[NUM_OUTPUT];
double hiddenLayerBias[NUM_HIDDEN_NODE];
double outputLayerBias[NUM_OUTPUT];
double hiddenWeights[NUM_INPUT][NUM_HIDDEN_NODE];
double outputWeights[NUM_HIDDEN_NODE][NUM_OUTPUT];
// create training dictionary of OR Gate with 2 inputs
double training_inputs[NUM_TRAIN_SET][NUM_INPUT] = { {0.0f,0.0f},{0.0f,1.0f},{1.0f,0.0f},{1.0f,1.0f} };
double training_outputs[NUM_TRAIN_SET][NUM_OUTPUT] = { {0.0f},{1.0f},{1.0f},{1.0f} };

// Simple network that can learn OR
// Feartures : sigmoid activation function, stochastic gradient descent, and mean square error fuction

double sigmoid(double x) { return 1 / (1 + exp(-x)); }
double dSigmoid(double x) { return x * (1 - x); }
double init_weight() { return ((double)rand())/((double)RAND_MAX); }
void shuffle(int *array, size_t n)
{
    if (n > 1)
    {
        size_t i;
        for (i = 0; i < n - 1; i++)
        {
            size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
            int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}

int initNeuralNetwork(){
    // -------------------- START: Weight and Bias Init ---------------------------//
    // Init the hidden weight with rand value with 2 inputs
    for (int i=0; i<NUM_INPUT; i++) {
        for (int j=0; j<NUM_HIDDEN_NODE; j++) {
            hiddenWeights[i][j] = init_weight();
        }
    }

    // Init the hidden bias and output weight for each nodes with rand value
    for (int i=0; i<NUM_HIDDEN_NODE; i++) {
        hiddenLayerBias[i] = init_weight();
        for (int j=0; j<NUM_OUTPUT; j++) {
            outputWeights[i][j] = init_weight();
        }
    }

    // Init the output layer bias with rand value (only 1 output is expected)
    for (int i=0; i<NUM_OUTPUT; i++) {
        outputLayerBias[i] = init_weight();
    }
    // -------------------- END: Weight and Bias Init ---------------------------//
    // -------------------- START: Train the model -----------------------------//

    // {0,1,2,3} => {[0,0],[0,1],[1,0],[1,1]}
    int trainingSetOrder[] = {0,1,2,3};

    for (int n=0; n < 10000; n++) {
        // shuffle the training sets order
        shuffle(trainingSetOrder,NUM_TRAIN_SET);
        for (int x=0; x<NUM_TRAIN_SET; x++) {

            int i = trainingSetOrder[x];

            // Feed Forward Algorithm (from input layer to output layer)
            // The bias neuron of current layer takes input from bias neuron of previous only.

            // HIDDEN LAYERS = 2
            for (int j=0; j<NUM_HIDDEN_NODE; j++) {
                // set activation with rand value of hidden layer bias defined in init phase
                double activation=hiddenLayerBias[j];
                 for (int k=0; k<NUM_INPUT; k++) {
                    // update activation value
                    // ex: [i][k]==[1],[0] ==> i=[0,1] => k=[0]
                    activation+=training_inputs[i][k]*hiddenWeights[k][j];
                }
                // update the hidden layer with new activation value
                hiddenLayer[j] = sigmoid(activation);
            }

            // OUTPUT LAYER = 1
            for (int j=0; j<NUM_OUTPUT; j++) {
                // set activation with rand value of output layer bias defined in init phase
                double activation=outputLayerBias[j];
                for (int k=0; k<NUM_HIDDEN_NODE; k++) {
                    activation+=hiddenLayer[k]*outputWeights[k][j];
                }
                outputLayer[j] = sigmoid(activation);
            }

            // Back Propagation Algorithm (from output layer to input layer)

            double deltaOutput[NUM_OUTPUT];
            for (int j=0; j<NUM_OUTPUT; j++) {
                double errorOutput = (training_outputs[i][j]-outputLayer[j]);
                deltaOutput[j] = errorOutput*dSigmoid(outputLayer[j]);
            }

            double deltaHidden[NUM_HIDDEN_NODE];
            for (int j=0; j<NUM_HIDDEN_NODE; j++) {
                double errorHidden = 0.0f;
                for(int k=0; k<NUM_OUTPUT; k++) {
                    errorHidden+=deltaOutput[k]*outputWeights[j][k];
                }
                deltaHidden[j] = errorHidden*dSigmoid(hiddenLayer[j]);
            }

            for (int j=0; j<NUM_OUTPUT; j++) {
                outputLayerBias[j] += deltaOutput[j]*LEARNING_RATE;
                for (int k=0; k<NUM_HIDDEN_NODE; k++) {
                    outputWeights[k][j]+=hiddenLayer[k]*deltaOutput[j]*LEARNING_RATE;
                }
            }

            for (int j=0; j<NUM_HIDDEN_NODE; j++) {
                hiddenLayerBias[j] += deltaHidden[j]*LEARNING_RATE;
                for(int k=0; k<NUM_INPUT; k++) {
                    hiddenWeights[k][j]+=training_inputs[i][k]*deltaHidden[j]*LEARNING_RATE;
                }
            }

        }
    }
    // -------------------- END: Train the model -----------------------------//

    return 0;
}

// Expecting Input: 0 = Good env, 1 = bad env
int runNeuralNetwork(int inputTemp, int inputNoise){
    // Run a test
    double test_inputs[NUM_TRAIN_SET][NUM_INPUT] = { {0.0f,0.0f},{0.0f,1.0f},{1.0f,0.0f},{1.0f,1.0f} };
    
    // Mapping inputTemp/inputNoise to test set
    int m;
    if (inputTemp == 0 && inputNoise == 0){ m = 0; }
    else if (inputTemp == 0 && inputNoise == 1){ m = 1; }
    else if (inputTemp == 1 && inputNoise == 0){ m = 2; }
    else if (inputTemp == 1 && inputNoise == 1){ m = 3; }
    
    // HIDDEN LAYERS = 2
    for (int j=0; j<NUM_HIDDEN_NODE; j++) {
       // set activation with modeled hiddenLayerBias
       double activation=hiddenLayerBias[j];
        for (int k=0; k<NUM_INPUT; k++) {
            //update activation value
            activation+=test_inputs[m][k]*hiddenWeights[k][j];
       }
        //update the hidden layer with new activation value
        hiddenLayer[j] = sigmoid(activation);
    }

    // OUTPUT LAYER = 1
    for (int j=0; j<NUM_OUTPUT; j++) {
        // set activation with modeled outputLayerBias
        double activation=outputLayerBias[j];
        for (int k=0; k<NUM_HIDDEN_NODE; k++) {
            activation+=hiddenLayer[k]*outputWeights[k][j];
            }
        outputLayer[j] = sigmoid(activation);
    }

    return round(outputLayer[0]); //will return either 0 (turn OFF LED) or 1 (turn ON LED)
}







