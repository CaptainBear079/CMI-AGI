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

void EM__init(EM* handler) {
	handler->tick = 0;
	handler->tickYear = 0;
	handler->entities = NULL;
	handler->entityCount = 0;
	return;
}

void EM__createAIThread(EM* handler, unsigned int id, char* name) {
	handler->entities[id] = malloc(sizeof(EM_Entity));
	handler->entities[id]->id = id;
	handler->entities[id]->name = name;
}

void EM__startSimulation(EM* handler) {}
