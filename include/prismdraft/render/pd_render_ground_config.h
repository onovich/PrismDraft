#ifndef PRISMDRAFT_RENDER_PD_RENDER_GROUND_CONFIG_H
#define PRISMDRAFT_RENDER_PD_RENDER_GROUND_CONFIG_H

#include "raylib.h"

typedef struct PdRenderGroundConfig {
    Color color;
    Color grid_color;
    float y;
    float size;
    float grid_step;
    int is_visible;
} PdRenderGroundConfig;

PdRenderGroundConfig pd_render_ground_config_default(void);

int pd_render_ground_config_is_valid(PdRenderGroundConfig config);

#endif
