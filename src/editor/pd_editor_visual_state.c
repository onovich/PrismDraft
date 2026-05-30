#include "prismdraft/editor/pd_editor_visual_state.h"

PdCoreResult pd_editor_visual_state_init(PdEditorVisualState* visual_state)
{
    if (visual_state == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    visual_state->face_color[0] = 219u;
    visual_state->face_color[1] = 64u;
    visual_state->face_color[2] = 68u;
    visual_state->face_color[3] = 255u;

    visual_state->background_color[0] = 255u;
    visual_state->background_color[1] = 114u;
    visual_state->background_color[2] = 90u;
    visual_state->background_color[3] = 255u;

    visual_state->shadow_color[0] = 92u;
    visual_state->shadow_color[1] = 58u;
    visual_state->shadow_color[2] = 74u;
    visual_state->shadow_color[3] = 112u;

    visual_state->light_direction[0] = -0.45f;
    visual_state->light_direction[1] = -0.75f;
    visual_state->light_direction[2] = -0.5f;
    visual_state->dark_intensity = 0.35f;
    visual_state->edge_depth_threshold = 0.0125f;
    visual_state->edge_normal_threshold = 0.25f;
    visual_state->shadow_plane_y = -1.02f;
    visual_state->shadow_offset_x = 0.72f;
    visual_state->shadow_offset_z = 0.78f;
    visual_state->shadow_half_width = 1.55f;
    visual_state->shadow_half_depth = 1.18f;
    visual_state->shadow_skew_x = 0.38f;

    return PD_CORE_RESULT_OK;
}
