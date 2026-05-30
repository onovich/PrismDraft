#include "prismdraft/render/pd_render_visual_config.h"

#include <assert.h>

int main(void)
{
    PdRenderVisualConfig config = pd_render_visual_config_default();

    assert(config.background_color.r == 255u);
    assert(config.background_color.g == 114u);
    assert(config.background_color.b == 90u);
    assert(config.background_color.a == 255u);
    assert(config.light_direction.x == -0.45f);
    assert(config.light_direction.y == -0.75f);
    assert(config.light_direction.z == -0.5f);
    assert(config.dark_intensity == 0.35f);
    assert(config.edge_depth_threshold > 0.0f);
    assert(config.edge_normal_threshold > 0.0f);

    return 0;
}
