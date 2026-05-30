#include "prismdraft/render/pd_render_target_config.h"

#include "prismdraft/render/pd_render_visual_config.h"

#include <assert.h>

static void pd_render_target_config_test_local_make_valid_target_plan(void)
{
    PdRenderTargetConfig config = pd_render_target_config_make(1280, 720);

    assert(config.width == 1280);
    assert(config.height == 720);
    assert(config.color_format == PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    assert(config.normal_format == PIXELFORMAT_UNCOMPRESSED_R32G32B32A32);
    assert(config.depth_bits == PD_RENDER_TARGET_CONFIG_DEFAULT_DEPTH_BITS);
    assert(pd_render_target_config_is_valid(config) == 1);
}

static void pd_render_target_config_test_local_uses_visual_edge_thresholds(void)
{
    PdRenderVisualConfig visual_config = pd_render_visual_config_default();
    PdRenderTargetConfig target_config = pd_render_target_config_make(640, 360);

    assert(target_config.edge_depth_threshold == visual_config.edge_depth_threshold);
    assert(target_config.edge_normal_threshold == visual_config.edge_normal_threshold);
}

static void pd_render_target_config_test_local_rejects_invalid_sizes(void)
{
    assert(pd_render_target_config_is_valid(pd_render_target_config_make(0, 720)) == 0);
    assert(pd_render_target_config_is_valid(pd_render_target_config_make(1280, 0)) == 0);
    assert(pd_render_target_config_is_valid(pd_render_target_config_make(-1, 720)) == 0);
}

int main(void)
{
    pd_render_target_config_test_local_make_valid_target_plan();
    pd_render_target_config_test_local_uses_visual_edge_thresholds();
    pd_render_target_config_test_local_rejects_invalid_sizes();
    return 0;
}
