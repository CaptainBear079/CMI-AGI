#include <dlfcn.h>

typedef struct _PluginInterface_ {
    void* entity; // The entity object of the Entity Manager
} PluginInterface;

typedef int (*PluginMain)(PluginInterface* interface);
