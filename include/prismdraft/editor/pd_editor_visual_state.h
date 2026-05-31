#ifndef PRISMDRAFT_EDITOR_PD_EDITOR_VISUAL_STATE_H
#define PRISMDRAFT_EDITOR_PD_EDITOR_VISUAL_STATE_H

#include "prismdraft/core/pd_core_result_entity.h"

#include <stdint.h>

typedef struct PdEditorVisualState {
    uint8_t face_color[4];
    uint8_t background_color[4];
    uint8_t shadow_color[4];
    uint8_t ground_color[4];
    float light_direction[3];
    float dark_intensity;
    float edge_sample_radius;
    float edge_depth_threshold;
    float edge_normal_threshold;
    float ground_y;
    float ground_size;
    float ground_grid_step;
    float shadow_plane_y;
    float shadow_offset_x;
    float shadow_offset_z;
    float shadow_half_width;
    float shadow_half_depth;
    float shadow_skew_x;
    int face_palette_index;
    int background_palette_index;
    int is_ground_visible;
} PdEditorVisualState;

PdCoreResult pd_editor_visual_state_init(PdEditorVisualState* visual_state);

PdCoreResult pd_editor_visual_state_cycle_face_color(PdEditorVisualState* visual_state, int direction);

PdCoreResult pd_editor_visual_state_cycle_background_color(PdEditorVisualState* visual_state, int direction);

PdCoreResult pd_editor_visual_state_adjust_edge_depth_threshold(PdEditorVisualState* visual_state, float delta);

PdCoreResult pd_editor_visual_state_adjust_edge_normal_threshold(PdEditorVisualState* visual_state, float delta);

PdCoreResult pd_editor_visual_state_adjust_dark_intensity(PdEditorVisualState* visual_state, float delta);

PdCoreResult pd_editor_visual_state_adjust_edge_sample_radius(PdEditorVisualState* visual_state, float delta);

PdCoreResult pd_editor_visual_state_adjust_shadow_strength(PdEditorVisualState* visual_state, int delta_alpha);

PdCoreResult pd_editor_visual_state_adjust_shadow_offset(
    PdEditorVisualState* visual_state,
    float delta_x,
    float delta_z);

PdCoreResult pd_editor_visual_state_adjust_light_direction(
    PdEditorVisualState* visual_state,
    float delta_x,
    float delta_y,
    float delta_z);

#endif
