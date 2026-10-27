#include <dlfcn.h>

typedef struct _PluginInterface_ {
    void* entity;   // The entity object of the Entity Manager
    bool* exitFlag; // Pointer to the exit flag
} PluginInterface;

typedef int (*PluginMain)(PluginInterface* interface);
