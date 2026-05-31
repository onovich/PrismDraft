#include "prismdraft/editor/pd_editor_visual_state.h"

#include <math.h>
#include <stddef.h>

static const uint8_t PD_EDITOR_VISUAL_STATE_FACE_PALETTE[][4] = {
    { 219u, 64u, 68u, 255u },
    { 238u, 194u, 85u, 255u },
    { 80u, 151u, 211u, 255u },
    { 134u, 209u, 153u, 255u },
    { 228u, 228u, 240u, 255u }
};

static const uint8_t PD_EDITOR_VISUAL_STATE_BACKGROUND_PALETTE[][4] = {
    { 255u, 114u, 90u, 255u },
    { 236u, 203u, 158u, 255u },
    { 122u, 172u, 199u, 255u },
    { 72u, 78u, 102u, 255u }
};

static int pd_editor_visual_state_local_palette_count(size_t byte_count)
{
    return (int)(byte_count / (sizeof(uint8_t) * 4u));
}

static float pd_editor_visual_state_local_clamp(float value, float min_value, float max_value)
{
    if (value < min_value) {
        return min_value;
    }

    if (value > max_value) {
        return max_value;
    }

    return value;
}

static int pd_editor_visual_state_local_wrap_palette_index(int current_index, int direction, int count)
{
    int next_index;

    if (count <= 0) {
        return 0;
    }

    next_index = current_index + (direction >= 0 ? 1 : -1);
    if (next_index < 0) {
        return count - 1;
    }

    if (next_index >= count) {
        return 0;
    }

    return next_index;
}

static void pd_editor_visual_state_local_copy_color(uint8_t destination[4], const uint8_t source[4])
{
    destination[0] = source[0];
    destination[1] = source[1];
    destination[2] = source[2];
    destination[3] = source[3];
}

PdCoreResult pd_editor_visual_state_init(PdEditorVisualState* visual_state)
{
    if (visual_state == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    visual_state->face_palette_index = 0;
    visual_state->background_palette_index = 0;
    pd_editor_visual_state_local_copy_color(
        visual_state->face_color,
        PD_EDITOR_VISUAL_STATE_FACE_PALETTE[visual_state->face_palette_index]);
    pd_editor_visual_state_local_copy_color(
        visual_state->background_color,
        PD_EDITOR_VISUAL_STATE_BACKGROUND_PALETTE[visual_state->background_palette_index]);

    visual_state->shadow_color[0] = 92u;
    visual_state->shadow_color[1] = 58u;
    visual_state->shadow_color[2] = 74u;
    visual_state->shadow_color[3] = 112u;
    visual_state->ground_color[0] = 240u;
    visual_state->ground_color[1] = 218u;
    visual_state->ground_color[2] = 178u;
    visual_state->ground_color[3] = 104u;

    visual_state->light_direction[0] = -0.45f;
    visual_state->light_direction[1] = -0.75f;
    visual_state->light_direction[2] = -0.5f;
    visual_state->dark_intensity = 0.35f;
    visual_state->edge_sample_radius = 1.0f;
    visual_state->edge_depth_threshold = 0.0125f;
    visual_state->edge_normal_threshold = 0.25f;
    visual_state->ground_y = -1.02f;
    visual_state->ground_size = 16.0f;
    visual_state->ground_grid_step = 0.5f;
    visual_state->shadow_plane_y = visual_state->ground_y;
    visual_state->shadow_offset_x = 0.0f;
    visual_state->shadow_offset_z = 0.0f;
    visual_state->shadow_half_width = 1.55f;
    visual_state->shadow_half_depth = 1.18f;
    visual_state->shadow_skew_x = 0.38f;
    visual_state->is_ground_visible = 1;

    return PD_CORE_RESULT_OK;
}

PdCoreResult pd_editor_visual_state_cycle_face_color(PdEditorVisualState* visual_state, int direction)
{
    int palette_count;

    if (visual_state == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    palette_count = pd_editor_visual_state_local_palette_count(sizeof(PD_EDITOR_VISUAL_STATE_FACE_PALETTE));
    visual_state->face_palette_index =
        pd_editor_visual_state_local_wrap_palette_index(visual_state->face_palette_index, direction, palette_count);
    pd_editor_visual_state_local_copy_color(
        visual_state->face_color,
        PD_EDITOR_VISUAL_STATE_FACE_PALETTE[visual_state->face_palette_index]);
    return PD_CORE_RESULT_OK;
}

PdCoreResult pd_editor_visual_state_cycle_background_color(PdEditorVisualState* visual_state, int direction)
{
    int palette_count;

    if (visual_state == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    palette_count = pd_editor_visual_state_local_palette_count(sizeof(PD_EDITOR_VISUAL_STATE_BACKGROUND_PALETTE));
    visual_state->background_palette_index = pd_editor_visual_state_local_wrap_palette_index(
        visual_state->background_palette_index,
        direction,
        palette_count);
    pd_editor_visual_state_local_copy_color(
        visual_state->background_color,
        PD_EDITOR_VISUAL_STATE_BACKGROUND_PALETTE[visual_state->background_palette_index]);
    return PD_CORE_RESULT_OK;
}

PdCoreResult pd_editor_visual_state_adjust_edge_depth_threshold(PdEditorVisualState* visual_state, float delta)
{
    if (visual_state == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    visual_state->edge_depth_threshold =
        pd_editor_visual_state_local_clamp(visual_state->edge_depth_threshold + delta, 0.001f, 0.08f);
    return PD_CORE_RESULT_OK;
}

PdCoreResult pd_editor_visual_state_adjust_edge_normal_threshold(PdEditorVisualState* visual_state, float delta)
{
    if (visual_state == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    visual_state->edge_normal_threshold =
        pd_editor_visual_state_local_clamp(visual_state->edge_normal_threshold + delta, 0.02f, 1.0f);
    return PD_CORE_RESULT_OK;
}

PdCoreResult pd_editor_visual_state_adjust_dark_intensity(PdEditorVisualState* visual_state, float delta)
{
    if (visual_state == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    visual_state->dark_intensity =
        pd_editor_visual_state_local_clamp(visual_state->dark_intensity + delta, 0.05f, 0.9f);
    return PD_CORE_RESULT_OK;
}

PdCoreResult pd_editor_visual_state_adjust_edge_sample_radius(PdEditorVisualState* visual_state, float delta)
{
    if (visual_state == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    visual_state->edge_sample_radius =
        pd_editor_visual_state_local_clamp(visual_state->edge_sample_radius + delta, 0.5f, 3.0f);
    return PD_CORE_RESULT_OK;
}

PdCoreResult pd_editor_visual_state_adjust_shadow_strength(PdEditorVisualState* visual_state, int delta_alpha)
{
    int next_alpha;

    if (visual_state == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    next_alpha = (int)visual_state->shadow_color[3] + delta_alpha;
    if (next_alpha < 0) {
        next_alpha = 0;
    }

    if (next_alpha > 255) {
        next_alpha = 255;
    }

    visual_state->shadow_color[3] = (uint8_t)next_alpha;
    return PD_CORE_RESULT_OK;
}

PdCoreResult pd_editor_visual_state_adjust_shadow_offset(
    PdEditorVisualState* visual_state,
    float delta_x,
    float delta_z)
{
    if (visual_state == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    visual_state->shadow_offset_x =
        pd_editor_visual_state_local_clamp(visual_state->shadow_offset_x + delta_x, -3.0f, 3.0f);
    visual_state->shadow_offset_z =
        pd_editor_visual_state_local_clamp(visual_state->shadow_offset_z + delta_z, -3.0f, 3.0f);
    return PD_CORE_RESULT_OK;
}

PdCoreResult pd_editor_visual_state_adjust_light_direction(
    PdEditorVisualState* visual_state,
    float delta_x,
    float delta_y,
    float delta_z)
{
    float next_x;
    float next_y;
    float next_z;
    float length;

    if (visual_state == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    next_x = visual_state->light_direction[0] + delta_x;
    next_y = visual_state->light_direction[1] + delta_y;
    next_z = visual_state->light_direction[2] + delta_z;
    length = sqrtf((next_x * next_x) + (next_y * next_y) + (next_z * next_z));
    if (length <= 0.000001f) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    visual_state->light_direction[0] = next_x / length;
    visual_state->light_direction[1] = next_y / length;
    visual_state->light_direction[2] = next_z / length;
    return PD_CORE_RESULT_OK;
}
