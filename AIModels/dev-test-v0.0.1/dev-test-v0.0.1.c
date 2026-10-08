#include "dev-test-v0.0.1.h"

int CPlugin_main(PluginInterface* interface) {
	// Process neuron
	// - Fetch input from the Entity
	// - Update neurons
	// Variables
	clock_t now;
	clock_t then;
	if(NEURAL_NETWORK->neurons == NULL) {
		NEURAL_NETWORK->neurons = malloc(
			NEURAL_NETWORK->neuronCount * sizeof(Neuron));
	}
	then = clock();
	while(true) {
		for(int i = 0; i < INPUT_COUNT; i++) {
			((Neuron*)INPUT[i].target)->input += INPUT[i].input;
		}
		now = clock();
		for(int n = 0; n < NEURAL_NETWORK->neuronCount; n++) {
			double deltaTime = (double)(now - then) / (double)CLOCKS_PER_SEC;
			NEURAL_NETWORK->neurons[n].V += (deltaTime / NEURAL_NETWORK->neurons[n].tau) * (-(NEURAL_NETWORK->neurons[n].V - NEURAL_NETWORK->neurons[n].V_rest) + NEURAL_NETWORK->neurons[n].input);
			if(NEURAL_NETWORK->neurons[n].V >= NEURAL_NETWORK->neurons[n].V_threshold) {
				NEURAL_NETWORK->neurons[n].V = NEURAL_NETWORK->neurons[n].V_reset;
				((Neuron*)NEURAL_NETWORK->neurons[n].output)->input += 1.0;
			}
		}
		then = now;
		for(int o = 0; o < OUTPUT_COUNT; o++) {
			OUTPUT[o].output = ((Neuron*)OUTPUT[o].source)->V;
		}
	}
	return 0;
}
