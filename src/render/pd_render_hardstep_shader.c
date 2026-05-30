#include "prismdraft/render/pd_render_hardstep_shader.h"

PdRenderHardstepShaderConfig pd_render_hardstep_shader_config_default(void)
{
    PdRenderHardstepShaderConfig config;

    config.vertex_shader_path = "shaders/pd_render_hardstep_shader.vert";
    config.fragment_shader_path = "shaders/pd_render_hardstep_shader.frag";
    config.light_direction = (Vector3){ -0.45f, -0.75f, -0.5f };
    config.dark_intensity = 0.35f;

    return config;
}
