#include "prismdraft/editor/pd_editor_visual_state.h"

#include <assert.h>

static void pd_editor_visual_state_test_local_initializes_defaults(void)
{
    PdEditorVisualState visual_state;

    assert(pd_editor_visual_state_init(0) == PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);
    assert(pd_editor_visual_state_init(&visual_state) == PD_CORE_RESULT_OK);
    assert(visual_state.face_color[3] == 255u);
    assert(visual_state.background_color[0] == 255u);
    assert(visual_state.background_color[3] == 255u);
    assert(visual_state.shadow_color[3] > 0u);
    assert(visual_state.ground_color[3] > 0u);
    assert(visual_state.light_direction[1] < 0.0f);
    assert(visual_state.dark_intensity > 0.0f);
    assert(visual_state.edge_sample_radius > 0.0f);
    assert(visual_state.edge_depth_threshold > 0.0f);
    assert(visual_state.edge_normal_threshold > 0.0f);
    assert(visual_state.is_ground_visible);
    assert(visual_state.ground_y < 0.0f);
    assert(visual_state.ground_size > 0.0f);
    assert(visual_state.ground_grid_step > 0.0f);
    assert(visual_state.shadow_half_width > 0.0f);
    assert(visual_state.shadow_half_depth > 0.0f);
}

static void pd_editor_visual_state_test_local_updates_editable_values(void)
{
    PdEditorVisualState visual_state;
    uint8_t first_face_red;
    uint8_t first_background_red;
    float first_edge_depth_threshold;
    float first_edge_normal_threshold;
    float first_edge_sample_radius;
    float first_dark_intensity;
    uint8_t first_shadow_alpha;
    float first_shadow_offset_x;
    float first_light_x;

    assert(pd_editor_visual_state_init(&visual_state) == PD_CORE_RESULT_OK);

    first_face_red = visual_state.face_color[0];
    assert(pd_editor_visual_state_cycle_face_color(&visual_state, 1) == PD_CORE_RESULT_OK);
    assert(visual_state.face_color[0] != first_face_red);
    assert(visual_state.face_palette_index == 1);

    first_background_red = visual_state.background_color[0];
    assert(pd_editor_visual_state_cycle_background_color(&visual_state, 1) == PD_CORE_RESULT_OK);
    assert(visual_state.background_color[0] != first_background_red);
    assert(visual_state.background_palette_index == 1);

    first_edge_depth_threshold = visual_state.edge_depth_threshold;
    assert(pd_editor_visual_state_adjust_edge_depth_threshold(&visual_state, 0.01f) == PD_CORE_RESULT_OK);
    assert(visual_state.edge_depth_threshold > first_edge_depth_threshold);

    first_edge_normal_threshold = visual_state.edge_normal_threshold;
    assert(pd_editor_visual_state_adjust_edge_normal_threshold(&visual_state, -0.05f) == PD_CORE_RESULT_OK);
    assert(visual_state.edge_normal_threshold < first_edge_normal_threshold);

    first_edge_sample_radius = visual_state.edge_sample_radius;
    assert(pd_editor_visual_state_adjust_edge_sample_radius(&visual_state, 0.5f) == PD_CORE_RESULT_OK);
    assert(visual_state.edge_sample_radius > first_edge_sample_radius);

    first_dark_intensity = visual_state.dark_intensity;
    assert(pd_editor_visual_state_adjust_dark_intensity(&visual_state, 0.1f) == PD_CORE_RESULT_OK);
    assert(visual_state.dark_intensity > first_dark_intensity);

    first_shadow_alpha = visual_state.shadow_color[3];
    assert(pd_editor_visual_state_adjust_shadow_strength(&visual_state, 16) == PD_CORE_RESULT_OK);
    assert(visual_state.shadow_color[3] > first_shadow_alpha);

    first_shadow_offset_x = visual_state.shadow_offset_x;
    assert(pd_editor_visual_state_adjust_shadow_offset(&visual_state, 0.25f, -0.25f) == PD_CORE_RESULT_OK);
    assert(visual_state.shadow_offset_x > first_shadow_offset_x);

    first_light_x = visual_state.light_direction[0];
    assert(pd_editor_visual_state_adjust_light_direction(&visual_state, 0.25f, 0.0f, 0.0f) == PD_CORE_RESULT_OK);
    assert(visual_state.light_direction[0] > first_light_x);
}

int main(void)
{
    pd_editor_visual_state_test_local_initializes_defaults();
    pd_editor_visual_state_test_local_updates_editable_values();
    return 0;
}
