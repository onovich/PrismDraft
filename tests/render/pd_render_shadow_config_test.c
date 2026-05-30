#include "prismdraft/render/pd_render_shadow_config.h"

#include <assert.h>

int main(void)
{
    PdRenderShadowConfig config = pd_render_shadow_config_default();

    assert(pd_render_shadow_config_is_valid(config));
    assert(config.color.a > 0u);
    assert(config.plane_y < 0.0f);
    assert(config.half_width > 0.0f);
    assert(config.half_depth > 0.0f);

    config.half_width = 0.0f;
    assert(!pd_render_shadow_config_is_valid(config));

    return 0;
}
