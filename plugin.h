#include <dlfcn.h>
#include "EntityManager/EntityManager.h"

typedef struct _PluginInterface_ {
    EM_Entity* entity; // The entity object of the Entity Manager
} PluginInterface;

typedef int (*PluginMain)(PluginInterface* interface);
