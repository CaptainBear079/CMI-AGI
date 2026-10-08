#include "./../../EntityManager/EntityManager.h"
#include "./../../plugin.h"
#include <math.h>

#define NEURAL_NETWORK ((NeuralNetwork*)((EM_Entity*)interface->entity)->data)
#define INPUT ((Input*)((EM_Entity*)interface->entity)->input)
#define INPUT_COUNT ((int)((EM_Entity*)interface->entity)->inputCount)
#define OUTPUT ((Output*)((EM_Entity*)interface->entity)->output)
#define OUTPUT_COUNT ((int)((EM_Entity*)interface->entity)->outputCount)

typedef struct _Input_ {
    double input; // Input current
    void* target; // Target neuron for the input
} Input;

typedef struct _Output_ {
    void* source;  // Source neuron
    double output; // Output current
} Output;

typedef struct _Neuron_ {
    double input;       // Input current
    void* output;       // Neuron for output
    double tau;         // Leak intensity
    double V;           // Membrane potential
    double V_threshold; // Membrane threshold
    double V_rest;      // Membrane resting potential
    double V_reset;     // Membrane reset potential
} Neuron;

typedef struct _NeuralNetwork_ {
    Neuron* neurons; // Array of neurons
    int neuronCount; // Number of neurons
} NeuralNetwork;
