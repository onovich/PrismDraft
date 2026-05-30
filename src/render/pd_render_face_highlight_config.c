#include "prismdraft/render/pd_render_face_highlight_config.h"

PdRenderFaceHighlightConfig pd_render_face_highlight_config_default(void)
{
    PdRenderFaceHighlightConfig config;

    config.color[0] = 95u;
    config.color[1] = 185u;
    config.color[2] = 255u;
    config.color[3] = 176u;
    config.surface_offset = 0.0125f;

    return config;
}

int pd_render_face_highlight_config_is_valid(PdRenderFaceHighlightConfig config)
{
    if (config.color[3] == 0u) {
        return 0;
    }

    if (config.surface_offset < 0.0f) {
        return 0;
    }

    return 1;
}
