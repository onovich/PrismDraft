#ifndef PRISMDRAFT_RENDER_PD_RENDER_EDGE_SHADER_H
#define PRISMDRAFT_RENDER_PD_RENDER_EDGE_SHADER_H

typedef struct PdRenderEdgeShaderConfig {
    const char* vertex_shader_path;
    const char* fragment_shader_path;
    float edge_depth_threshold;
    float edge_normal_threshold;
} PdRenderEdgeShaderConfig;

PdRenderEdgeShaderConfig pd_render_edge_shader_config_default(void);

#endif
