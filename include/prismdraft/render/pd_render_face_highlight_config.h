#ifndef PRISMDRAFT_RENDER_PD_RENDER_FACE_HIGHLIGHT_CONFIG_H
#define PRISMDRAFT_RENDER_PD_RENDER_FACE_HIGHLIGHT_CONFIG_H

#include <stdint.h>

typedef struct PdRenderFaceHighlightConfig {
    uint8_t color[4];
    float surface_offset;
} PdRenderFaceHighlightConfig;

PdRenderFaceHighlightConfig pd_render_face_highlight_config_default(void);

int pd_render_face_highlight_config_is_valid(PdRenderFaceHighlightConfig config);

#endif
