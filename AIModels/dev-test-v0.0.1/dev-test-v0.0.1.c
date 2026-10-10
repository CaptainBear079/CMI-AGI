#include "dev-test-v0.0.1.h"

int CPlugin_main(PluginInterface* interface) {
	// Process neuron
	// - Fetch input from the Entity
	// - Update neurons
	// Variables
	double deltaTime;
	if(NEURAL_NETWORK->neurons == NULL) {
		NEURAL_NETWORK->neurons = malloc(
			NEURAL_NETWORK->neuronCount * sizeof(Neuron));
	}
	for(int n = 0; n < NEURAL_NETWORK->neuronCount; n++) {
		NEURAL_NETWORK->neurons[n].then = clock();
	}
	while(true) {
		for(int i = 0; i < INPUT_COUNT; i++) {
			((Neuron*)INPUT[i].target)->input += INPUT[i].input;
		}

		for(int n = 0; n < NEURAL_NETWORK->neuronCount; n++) {
			NEURAL_NETWORK->neurons[n].now = clock();
			deltaTime = (double)(NEURAL_NETWORK->neurons[n].now - NEURAL_NETWORK->neurons[n].then) / (double)CLOCKS_PER_SEC;
			UpdateNeuron:
			if(!(NEURAL_NETWORK->neurons[n].resetFlag)) {
				NEURAL_NETWORK->neurons[n].V += (deltaTime / NEURAL_NETWORK->neurons[n].tau) * (-(NEURAL_NETWORK->neurons[n].V - NEURAL_NETWORK->neurons[n].V_rest) + NEURAL_NETWORK->neurons[n].input);
				if(NEURAL_NETWORK->neurons[n].V >= NEURAL_NETWORK->neurons[n].V_threshold) {
					NEURAL_NETWORK->neurons[n].V = NEURAL_NETWORK->neurons[n].V_reset;
					((Neuron*)NEURAL_NETWORK->neurons[n].output)->input += 1.0;
				}
				NEURAL_NETWORK->neurons[n].then = NEURAL_NETWORK->neurons[n].now;
			}
			else {
				if(deltaTime >= NEURAL_NETWORK->neurons[n].restTime) {
					NEURAL_NETWORK->neurons[n].resetFlag = false;
					goto UpdateNeuron;
				}
			}
		}
		for(int o = 0; o < OUTPUT_COUNT; o++) {
			OUTPUT[o].output = ((Neuron*)OUTPUT[o].source)->V;
		}
	}
	return 0;
}
