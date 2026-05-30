#include "prismdraft/render/pd_render_target_config.h"

#include "prismdraft/render/pd_render_visual_config.h"

PdRenderTargetConfig pd_render_target_config_make(int width, int height)
{
    PdRenderVisualConfig visual_config = pd_render_visual_config_default();
    PdRenderTargetConfig config;

    config.width = width;
    config.height = height;
    config.color_format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    config.normal_format = PIXELFORMAT_UNCOMPRESSED_R32G32B32A32;
    config.depth_bits = PD_RENDER_TARGET_CONFIG_DEFAULT_DEPTH_BITS;
    config.edge_depth_threshold = visual_config.edge_depth_threshold;
    config.edge_normal_threshold = visual_config.edge_normal_threshold;

    return config;
}

int pd_render_target_config_is_valid(PdRenderTargetConfig config)
{
    if (config.width <= 0 || config.height <= 0) {
        return 0;
    }

    if (config.color_format <= 0 || config.normal_format <= 0) {
        return 0;
    }

    if (config.depth_bits <= 0) {
        return 0;
    }

    if (config.edge_depth_threshold <= 0.0f || config.edge_normal_threshold <= 0.0f) {
        return 0;
    }

    return 1;
}
