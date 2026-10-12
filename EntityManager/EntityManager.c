#include "EntityManager.h"

void* EntityThread(void* arg) {
	EM* em = (EM*)arg;
	unsigned int* id = (unsigned int*)(arg + sizeof(EM));
	if(em->entities[*id]->type == EM_ENTITY__TYPE_CPLUGIN) {
		// Load C Plugin AI program
		void* handle = dlopen(em->entities[*id]->path, RTLD_NOW);
		if(!handle) {
			fprintf(stderr, "[Entity Thread] Error while loading C Plugin:\n-> Load error: %s\n", dlerror());
			*em->entities[*id]->retCode = 1;
			return em->entities[*id]->retCode;
		}

		PluginMain _CPlugin = (PluginMain)dlsym(handle, "CPlugin_main");
		if(!_CPlugin) {
			fprintf(stderr, "[Entity Thread] Error while loading C Plugin:\n-> Symbol error: %s\n", dlerror());
			dlclose(handle);
			*em->entities[*id]->retCode = 1;
			return em->entities[*id]->retCode;
		}

		// Set up Plugin Interface
		PluginInterface PI;
		PI.entity = (void*)em->entities[*id];

		// Run AI
		*em->entities[*id]->retCode = _CPlugin(&PI);

		// Tick end
		dlclose(handle);
		return em->entities[*id]->retCode;
	}
	else if(em->entities[*id]->type == EM_ENTITY__TYPE_ENTITY_ENGINE) {
		// Load EE (Entity Engine) script
		// INDEV
	}

	// Unexpected error
	*em->entities[*id]->retCode = -1;
	return em->entities[*id]->retCode;
}

void EM__init(EM* handler, bool printExitCodes) {
	handler->tick = 0;
	handler->tickYear = 0;
	handler->entities = NULL;
	handler->entityCount = 0;
	handler->printExitCodes = printExitCodes;
	handler->simActive = false;
	return;
}

int EM__createEntity(
	EM* handler, unsigned int id,
	char* name, unsigned int type, char* path,
	unsigned long long dataSectionSize, unsigned long long inputSectionSize, unsigned long long outputSectionSize
) {
	if(handler->simActive) {
		printf("[Entity Manager] Simulation is currently running.\n");
		return 1;
	}
	else {
		printf("[Entity Manager] Creating entity...");

		// Allocate/Reallocate entity list
		fflush(stdout);
		handler->entityCount++;
		if(handler->entities == NULL) {
			handler->entities = malloc(handler->entityCount * sizeof(EM_Entity*));
		}
		else {
			handler->entities = realloc(handler->entities, handler->entityCount * sizeof(EM_Entity*));
		}

		// Allocate entity
		handler->entities[id] = malloc(sizeof(EM_Entity));

		// Set id
		handler->entities[id]->id = id;

		// Copy name (if not NULL)
		if(name != NULL) {
			handler->entities[id]->name = malloc((strlen(name) + 1) * sizeof(char));
			strcpy(handler->entities[id]->name, name);
		}

		// Set type
		handler->entities[id]->type = type;

		// Copy path (return if NULL)
		if(path == NULL) {
			fprintf(stderr, "ERROR!\n[Entity Manager] ERROR: path is NULL.\n");
			return 1;
		}
		handler->entities[id]->path = malloc((strlen(path) + 1) * sizeof(char));
		strcpy(handler->entities[id]->path, path);

		// Allocate data memory
		handler->entities[id]->data = malloc(dataSectionSize);

		// Allocate input memory
		handler->entities[id]->input = malloc(inputSectionSize);

		// Allocate output memory
		handler->entities[id]->output = malloc(outputSectionSize);

		printf(" Success!\n");
	}
	return 0;
}

void EM__startSimulation(EM* handler) {
	handler->simActive = true;
	for(int i = 0; i < handler->entityCount; i++) {
		printf("\n[Entity Manager] Starting Entity...");
		fflush(stdout);
		void* arg = malloc(sizeof(EM) + sizeof(int));
		memcpy(arg, handler, sizeof(EM));
		memcpy(arg + sizeof(EM), &(handler->entities[i]->id), sizeof(int));
		pthread_create(&(handler->entities[i]->thread), NULL, (void*)EntityThread, arg);
		printf(" Done!\n");
	}
}

void EM__quitSimulation(EM* handler) {
	for(int i = 0; i < handler->entityCount; i++) {
		handler->entities[i]->retCode = malloc(sizeof(int));
		pthread_join(handler->entities[i]->thread, (void**)&(handler->entities[i]->retCode));
	}
	handler->simActive = false;
	if(handler->printExitCodes) {
		for(int i = 0; i < handler->entityCount; i++) {
			printf("[Entity:%d]: Exit code: %d\n", i, *(handler->entities[i]->retCode));
			free(handler->entities[i]->retCode);
		}
	}
	else {
		for(int i = 0; i < handler->entityCount; i++) {
			free(handler->entities[i]->retCode);
		}
	}
}
