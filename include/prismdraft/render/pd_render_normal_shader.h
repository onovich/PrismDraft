#ifndef PRISMDRAFT_RENDER_PD_RENDER_NORMAL_SHADER_H
#define PRISMDRAFT_RENDER_PD_RENDER_NORMAL_SHADER_H

typedef struct PdRenderNormalShaderConfig {
    const char* vertex_shader_path;
    const char* fragment_shader_path;
} PdRenderNormalShaderConfig;

PdRenderNormalShaderConfig pd_render_normal_shader_config_default(void);

#endif
