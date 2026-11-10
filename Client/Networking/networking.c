#include "networking.h"

// Mark a ID in the client object list as used
unsigned long long CMI_Client__setUniqueID(unsigned long long id) {}
unsigned long long CMI_Client__setUniqueID(Object* objects) {
    unsigned long long id = 0;
    while(objects[id].type != 0) {
        id++;
    }
    return id;
}
