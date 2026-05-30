#ifndef PRISMDRAFT_RENDER_PD_RENDER_TARGET_CONFIG_H
#define PRISMDRAFT_RENDER_PD_RENDER_TARGET_CONFIG_H

#include "raylib.h"

#define PD_RENDER_TARGET_CONFIG_DEFAULT_DEPTH_BITS 24

typedef struct PdRenderTargetConfig {
    int width;
    int height;
    int color_format;
    int normal_format;
    int depth_bits;
    float edge_depth_threshold;
    float edge_normal_threshold;
} PdRenderTargetConfig;

PdRenderTargetConfig pd_render_target_config_make(int width, int height);

int pd_render_target_config_is_valid(PdRenderTargetConfig config);

#endif
