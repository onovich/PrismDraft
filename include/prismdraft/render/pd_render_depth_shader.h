#ifndef PRISMDRAFT_RENDER_PD_RENDER_DEPTH_SHADER_H
#define PRISMDRAFT_RENDER_PD_RENDER_DEPTH_SHADER_H

typedef struct PdRenderDepthShaderConfig {
    const char* vertex_shader_path;
    const char* fragment_shader_path;
} PdRenderDepthShaderConfig;

PdRenderDepthShaderConfig pd_render_depth_shader_config_default(void);

#endif
