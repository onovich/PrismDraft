#include "prismdraft/render/pd_render_face_highlight_config.h"

#include <assert.h>

int main(void)
{
    PdRenderFaceHighlightConfig config = pd_render_face_highlight_config_default();

    assert(pd_render_face_highlight_config_is_valid(config));
    assert(config.color[3] > 0u);
    assert(config.surface_offset > 0.0f);

    config.color[3] = 0u;
    assert(!pd_render_face_highlight_config_is_valid(config));

    config = pd_render_face_highlight_config_default();
    config.surface_offset = -1.0f;
    assert(!pd_render_face_highlight_config_is_valid(config));

    return 0;
}
