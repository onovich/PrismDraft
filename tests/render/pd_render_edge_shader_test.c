#include "prismdraft/render/pd_render_edge_shader.h"

#include "prismdraft/render/pd_render_visual_config.h"

#include <assert.h>
#include <string.h>

int main(void)
{
    PdRenderVisualConfig visual_config = pd_render_visual_config_default();
    PdRenderEdgeShaderConfig edge_config = pd_render_edge_shader_config_default();

    assert(strcmp(edge_config.vertex_shader_path, "shaders/pd_render_edge_shader.vert") == 0);
    assert(strcmp(edge_config.fragment_shader_path, "shaders/pd_render_edge_shader.frag") == 0);
    assert(edge_config.edge_depth_threshold == visual_config.edge_depth_threshold);
    assert(edge_config.edge_normal_threshold == visual_config.edge_normal_threshold);

    return 0;
}
