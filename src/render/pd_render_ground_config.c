#include "prismdraft/render/pd_render_ground_config.h"

PdRenderGroundConfig pd_render_ground_config_default(void)
{
    PdRenderGroundConfig config;

    config.color = (Color){ 240u, 218u, 178u, 104u };
    config.grid_color = (Color){ 88u, 96u, 108u, 96u };
    config.y = -1.02f;
    config.size = 16.0f;
    config.grid_step = 0.5f;
    config.is_visible = 1;
    return config;
}

int pd_render_ground_config_is_valid(PdRenderGroundConfig config)
{
    if (config.size <= 0.0f) {
        return 0;
    }

    if (config.grid_step <= 0.0f) {
        return 0;
    }

    return 1;
}
