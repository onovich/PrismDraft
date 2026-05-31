#include "prismdraft/render/pd_render_shadow_config.h"

PdRenderShadowConfig pd_render_shadow_config_default(void)
{
    PdRenderShadowConfig config;

    config.color = (Color){ 92u, 58u, 74u, 112u };
    config.plane_y = -1.02f;
    config.offset_x = 0.0f;
    config.offset_z = 0.0f;
    config.half_width = 1.55f;
    config.half_depth = 1.18f;
    config.skew_x = 0.38f;

    return config;
}

int pd_render_shadow_config_is_valid(PdRenderShadowConfig config)
{
    if (config.color.a == 0u) {
        return 0;
    }

    if (config.half_width <= 0.0f || config.half_depth <= 0.0f) {
        return 0;
    }

    return 1;
}
