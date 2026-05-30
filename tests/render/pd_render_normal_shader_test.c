#include "prismdraft/render/pd_render_normal_shader.h"

#include <assert.h>
#include <string.h>

int main(void)
{
    PdRenderNormalShaderConfig normal_config = pd_render_normal_shader_config_default();

    assert(strcmp(normal_config.vertex_shader_path, "shaders/pd_render_normal_shader.vert") == 0);
    assert(strcmp(normal_config.fragment_shader_path, "shaders/pd_render_normal_shader.frag") == 0);

    return 0;
}
