#include "EntityManager.h"

void* EntityThread(void* arg) {
	EM* em = (EM*)arg;
	unsigned int* id = (unsigned int*)(arg + sizeof(EM));
	int* retCode = malloc(sizeof(int));
	if(em->entities[*id]->type == EM_ENTITY__TYPE_CPLUGIN) {
		// Load C Plugin AI program
		void* handle = dlopen(em->entities[*id]->path, RTLD_NOW);
		if(!handle) {
			fprintf(stderr, "[Entity Thread] Error while loading C Plugin:\n-> Load error: %s\n", dlerror());
			*retCode = 1;
			return retCode;
		}

		PluginMain _CPlugin = (PluginMain)dlsym(handle, "CPlugin_main");
		if(!_CPlugin) {
			fprintf(stderr, "[Entity Thread] Error while loading C Plugin:\n-> Symbol error: %s\n", dlerror());
			dlclose(handle);
			*retCode = 1;
			return retCode;
		}

		// Set up Plugin Interface
		PluginInterface PI;
		PI.entity = (void*)em->entities[*id];

		// Run AI
		*retCode = _CPlugin(&PI);

		// Tick end
		dlclose(handle);
		return retCode;
	}
	else if(em->entities[*id]->type == EM_ENTITY__TYPE_ENTITY_ENGINE) {
		// Load EE (Entity Engine) script
		// INDEV
	}

	// Unexpected error
	*retCode = -1;
	return retCode;
}

void EM__init(EM* handler, bool printExitCodes) {
	handler->tick = 0;
	handler->tickYear = 0;
	handler->entities = NULL;
	handler->entityCount = 0;
	handler->entityRetCodes = malloc(handler->entityCount * sizeof(int*));
	handler->printExitCodes = printExitCodes;
	return;
}

int EM__createEntity(EM* handler, unsigned int id, char* name, unsigned int type, char* path) {
	if(handler->simActive) {
		printf("[Entity Manager] Simulation is currently running.\n");
		return 1;
	}
	else {
		printf("[Entity Manager] Creating entity...");
		handler->entities[id] = malloc(sizeof(EM_Entity));
		handler->entities[id]->id = id;
		handler->entities[id]->name = name;
		handler->entities[id]->type = type;
		handler->entities[id]->path = path;
		handler->entityCount++;
		printf(" Success!\n");
	}
	return 0;
}

void EM__startSimulation(EM* handler) {
	handler->simActive = true;
	for(int i = 0; i < handler->entityCount; i++) {
		printf("[Entity Manager] Creating Entity...");
		fflush(stdout);
		handler->entities[i]->thread = malloc(sizeof(pthread_t));
		void* arg = malloc(sizeof(EM) + sizeof(int));
		memcpy(arg, handler, sizeof(EM));
		memcpy(arg + sizeof(EM), &(handler->entities[i]->id), sizeof(int));
		pthread_create(handler->entities[i]->thread, NULL, (void*)EntityThread, arg);
		printf(" Done!\n");
	}
}

void EM__quitSimulation(EM* handler) {
	for(int i = 0; i < handler->entityCount; i++) {
		pthread_join(*(handler->entities[i]->thread), (void**)&(handler->entityRetCodes[i]));
	}
	handler->simActive = false;
	if(handler->printExitCodes) {
		for(int i = 0; i < handler->entityCount; i++) {
			printf("[Entity:%d]: Exit code: %d\n", i, *(handler->entityRetCodes[i]));
		}
	}
}
