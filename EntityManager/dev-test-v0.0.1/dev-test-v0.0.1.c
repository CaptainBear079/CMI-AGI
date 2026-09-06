#include "dev-test-v0.0.1.h"

void* dev_test_v1(void* arg) {
	EM* em = (EM*)arg;
	int* emEntityIndex = (int*)arg + sizeof(EM);
	int* ret_code = malloc(sizeof(int));
	while(em->events[em->eventIndex].event != EM_EVENT_SHUTDOWN) {
		em->entities[*emEntityIndex].channel;
	}
	*ret_code = 0;
	return ret_code; // Shutdown via exit code signal
}