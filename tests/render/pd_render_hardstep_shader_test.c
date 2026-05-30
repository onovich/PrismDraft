#include "prismdraft/render/pd_render_hardstep_shader.h"

#include <assert.h>
#include <string.h>

int main(void)
{
    PdRenderHardstepShaderConfig config = pd_render_hardstep_shader_config_default();

    assert(strcmp(config.vertex_shader_path, "shaders/pd_render_hardstep_shader.vert") == 0);
    assert(strcmp(config.fragment_shader_path, "shaders/pd_render_hardstep_shader.frag") == 0);
    assert(config.light_direction.x == -0.45f);
    assert(config.light_direction.y == -0.75f);
    assert(config.light_direction.z == -0.5f);
    assert(config.dark_intensity == 0.35f);

    return 0;
}
