#include "./../../EntityManager/EntityManager.h"
#include "./../../plugin.h"
#include <math.h>

typedef struct _Neuron_ {
    double tau;         // Leak intensity
    double input;       // Input current
    double V;           // Membrane potential
    double V_threshold; // Membrane threshold
    double V_rest;      // Membrane resting potential
    double V_reset;     // Membrane reset potential
} Neuron;
