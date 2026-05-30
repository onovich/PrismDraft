#ifndef PRISMDRAFT_RENDER_PD_RENDER_SHADOW_CONFIG_H
#define PRISMDRAFT_RENDER_PD_RENDER_SHADOW_CONFIG_H

#include "raylib.h"

typedef struct PdRenderShadowConfig {
    Color color;
    float plane_y;
    float offset_x;
    float offset_z;
    float half_width;
    float half_depth;
    float skew_x;
} PdRenderShadowConfig;

PdRenderShadowConfig pd_render_shadow_config_default(void);

int pd_render_shadow_config_is_valid(PdRenderShadowConfig config);

#endif
