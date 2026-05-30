#include "prismdraft/render/pd_render_target_controller.h"

#include <assert.h>

int main(void)
{
    PdRenderTargetController target_controller = { 0 };

    assert(!pd_render_target_controller_is_ready(0));
    assert(!pd_render_target_controller_is_ready(&target_controller));

    target_controller.config = pd_render_target_config_make(64, 64);
    target_controller.color_depth_target.id = 1u;
    target_controller.color_depth_target.texture.id = 2u;
    target_controller.color_depth_target.depth.id = 3u;
    target_controller.normal_target.id = 4u;
    target_controller.normal_target.texture.id = 5u;
    target_controller.depth_target.id = 6u;
    target_controller.depth_target.texture.id = 7u;

    assert(pd_render_target_controller_is_ready(&target_controller));

    return 0;
}
