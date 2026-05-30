#ifndef PRISMDRAFT_RENDER_PD_RENDER_VISUAL_CONFIG_H
#define PRISMDRAFT_RENDER_PD_RENDER_VISUAL_CONFIG_H

#include "raylib.h"

typedef struct PdRenderVisualConfig {
    Color background_color;
    Vector3 light_direction;
    float dark_intensity;
    float edge_sample_radius;
    float edge_depth_threshold;
    float edge_normal_threshold;
} PdRenderVisualConfig;

PdRenderVisualConfig pd_render_visual_config_default(void);

#endif
