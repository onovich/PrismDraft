#include "prismdraft/render/pd_render_edge_shader.h"

#include "prismdraft/render/pd_render_visual_config.h"

PdRenderEdgeShaderConfig pd_render_edge_shader_config_default(void)
{
    PdRenderVisualConfig visual_config = pd_render_visual_config_default();
    PdRenderEdgeShaderConfig config;

    config.vertex_shader_path = "shaders/pd_render_edge_shader.vert";
    config.fragment_shader_path = "shaders/pd_render_edge_shader.frag";
    config.edge_sample_radius = visual_config.edge_sample_radius;
    config.edge_depth_threshold = visual_config.edge_depth_threshold;
    config.edge_normal_threshold = visual_config.edge_normal_threshold;

    return config;
}
