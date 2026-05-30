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
    assert(visual_state.light_direction[1] < 0.0f);
    assert(visual_state.dark_intensity > 0.0f);
    assert(visual_state.edge_depth_threshold > 0.0f);
    assert(visual_state.edge_normal_threshold > 0.0f);
    assert(visual_state.shadow_half_width > 0.0f);
    assert(visual_state.shadow_half_depth > 0.0f);
}

int main(void)
{
    pd_editor_visual_state_test_local_initializes_defaults();
    return 0;
}
