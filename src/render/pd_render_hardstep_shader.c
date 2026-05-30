#include "prismdraft/render/pd_render_hardstep_shader.h"

#include "prismdraft/render/pd_render_visual_config.h"

PdRenderHardstepShaderConfig pd_render_hardstep_shader_config_default(void)
{
    PdRenderHardstepShaderConfig config;
    PdRenderVisualConfig visual_config = pd_render_visual_config_default();

    config.vertex_shader_path = "shaders/pd_render_hardstep_shader.vert";
    config.fragment_shader_path = "shaders/pd_render_hardstep_shader.frag";
    config.light_direction = visual_config.light_direction;
    config.dark_intensity = visual_config.dark_intensity;

    return config;
}
