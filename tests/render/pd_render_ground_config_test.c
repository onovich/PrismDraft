#include "prismdraft/render/pd_render_ground_config.h"

#include <assert.h>

int main(void)
{
    PdRenderGroundConfig config = pd_render_ground_config_default();

    assert(pd_render_ground_config_is_valid(config));
    assert(config.is_visible);
    assert(config.color.a > 0u);
    assert(config.y < 0.0f);
    assert(config.size > 0.0f);
    assert(config.grid_step > 0.0f);

    config.size = 0.0f;
    assert(!pd_render_ground_config_is_valid(config));

    config = pd_render_ground_config_default();
    config.grid_step = 0.0f;
    assert(!pd_render_ground_config_is_valid(config));

    return 0;
}
