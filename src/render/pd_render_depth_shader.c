#include "prismdraft/render/pd_render_depth_shader.h"

PdRenderDepthShaderConfig pd_render_depth_shader_config_default(void)
{
    PdRenderDepthShaderConfig config;

    config.vertex_shader_path = "shaders/pd_render_depth_shader.vert";
    config.fragment_shader_path = "shaders/pd_render_depth_shader.frag";

    return config;
}
