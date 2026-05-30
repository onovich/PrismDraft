#include "prismdraft/render/pd_render_normal_shader.h"

PdRenderNormalShaderConfig pd_render_normal_shader_config_default(void)
{
    PdRenderNormalShaderConfig config;

    config.vertex_shader_path = "shaders/pd_render_normal_shader.vert";
    config.fragment_shader_path = "shaders/pd_render_normal_shader.frag";

    return config;
}
