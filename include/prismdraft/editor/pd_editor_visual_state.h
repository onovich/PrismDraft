#ifndef PRISMDRAFT_EDITOR_PD_EDITOR_VISUAL_STATE_H
#define PRISMDRAFT_EDITOR_PD_EDITOR_VISUAL_STATE_H

#include "prismdraft/core/pd_core_result_entity.h"

#include <stdint.h>

typedef struct PdEditorVisualState {
    uint8_t face_color[4];
    uint8_t background_color[4];
    uint8_t shadow_color[4];
    float light_direction[3];
    float dark_intensity;
    float edge_depth_threshold;
    float edge_normal_threshold;
    float shadow_plane_y;
    float shadow_offset_x;
    float shadow_offset_z;
    float shadow_half_width;
    float shadow_half_depth;
    float shadow_skew_x;
} PdEditorVisualState;

PdCoreResult pd_editor_visual_state_init(PdEditorVisualState* visual_state);

#endif
