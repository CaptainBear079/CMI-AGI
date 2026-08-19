// Includes
#include "Fast3D.h"

// Render function
int render(uint32_t* framebuffer, int fb_width, int fb_height) {
    Vertex v1, v2, v3;
    Triangle t1 = {&v1, &v2, &v3};

    framebuffer[(t1.v1->y * fb_width) + t1.v1->x] = SOMETHING;

    return 0;
}
