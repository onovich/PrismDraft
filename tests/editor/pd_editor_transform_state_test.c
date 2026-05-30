#include "prismdraft/editor/pd_editor_transform_state.h"

#include <assert.h>

int main(void)
{
    PdEditorTransformState transform_state;

    assert(pd_editor_transform_state_init(0) == PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);
    assert(pd_editor_transform_state_init(&transform_state) == PD_CORE_RESULT_OK);
    assert(transform_state.position[0] == 0.0f);
    assert(transform_state.position[1] == 0.0f);
    assert(transform_state.position[2] == 0.0f);
    assert(transform_state.rotation_degrees[0] == 0.0f);
    assert(transform_state.rotation_degrees[1] == 0.0f);
    assert(transform_state.rotation_degrees[2] == 0.0f);
    assert(transform_state.scale[0] == 1.0f);
    assert(transform_state.scale[1] == 1.0f);
    assert(transform_state.scale[2] == 1.0f);

    assert(pd_editor_transform_state_translate(&transform_state, 1.0f, 2.0f, 3.0f) == PD_CORE_RESULT_OK);
    assert(transform_state.position[0] == 1.0f);
    assert(transform_state.position[1] == 2.0f);
    assert(transform_state.position[2] == 3.0f);

    assert(pd_editor_transform_state_rotate_degrees(&transform_state, 10.0f, 20.0f, 30.0f) == PD_CORE_RESULT_OK);
    assert(transform_state.rotation_degrees[0] == 10.0f);
    assert(transform_state.rotation_degrees[1] == 20.0f);
    assert(transform_state.rotation_degrees[2] == 30.0f);

    assert(pd_editor_transform_state_scale_uniform(&transform_state, 0.5f) == PD_CORE_RESULT_OK);
    assert(transform_state.scale[0] == 1.5f);
    assert(transform_state.scale[1] == 1.5f);
    assert(transform_state.scale[2] == 1.5f);
    assert(pd_editor_transform_state_scale_uniform(&transform_state, -2.0f) == PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);

    pd_editor_transform_state_reset(&transform_state);
    assert(transform_state.position[0] == 0.0f);
    assert(transform_state.rotation_degrees[1] == 0.0f);
    assert(transform_state.scale[2] == 1.0f);

    return 0;
}
