#ifndef PRISMDRAFT_RENDER_PD_RENDER_HARDSTEP_SHADER_H
#define PRISMDRAFT_RENDER_PD_RENDER_HARDSTEP_SHADER_H

#include "raylib.h"

typedef struct PdRenderHardstepShaderConfig {
    const char* vertex_shader_path;
    const char* fragment_shader_path;
    Vector3 light_direction;
    float dark_intensity;
} PdRenderHardstepShaderConfig;

PdRenderHardstepShaderConfig pd_render_hardstep_shader_config_default(void);

#endif
