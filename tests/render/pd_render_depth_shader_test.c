#include "prismdraft/render/pd_render_depth_shader.h"

#include <assert.h>
#include <string.h>

int main(void)
{
    PdRenderDepthShaderConfig depth_config = pd_render_depth_shader_config_default();

    assert(strcmp(depth_config.vertex_shader_path, "shaders/pd_render_depth_shader.vert") == 0);
    assert(strcmp(depth_config.fragment_shader_path, "shaders/pd_render_depth_shader.frag") == 0);

    return 0;
}
