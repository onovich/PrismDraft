#include "prismdraft/render/pd_render_visual_config.h"

PdRenderVisualConfig pd_render_visual_config_default(void)
{
    PdRenderVisualConfig config;

    config.background_color = (Color){ 255u, 114u, 90u, 255u };
    config.light_direction = (Vector3){ -0.45f, -0.75f, -0.5f };
    config.dark_intensity = 0.35f;
    config.edge_depth_threshold = 0.0125f;
    config.edge_normal_threshold = 0.25f;

    return config;
}
