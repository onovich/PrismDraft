#include "prismdraft/editor/pd_editor_transform_state.h"

static void pd_editor_transform_state_local_set_identity(PdEditorTransformState* transform_state)
{
    transform_state->position[0] = 0.0f;
    transform_state->position[1] = 0.0f;
    transform_state->position[2] = 0.0f;
    transform_state->rotation_degrees[0] = 0.0f;
    transform_state->rotation_degrees[1] = 0.0f;
    transform_state->rotation_degrees[2] = 0.0f;
    transform_state->scale[0] = 1.0f;
    transform_state->scale[1] = 1.0f;
    transform_state->scale[2] = 1.0f;
}

PdCoreResult pd_editor_transform_state_init(PdEditorTransformState* transform_state)
{
    if (transform_state == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    pd_editor_transform_state_local_set_identity(transform_state);
    return PD_CORE_RESULT_OK;
}

void pd_editor_transform_state_reset(PdEditorTransformState* transform_state)
{
    if (transform_state == 0) {
        return;
    }

    pd_editor_transform_state_local_set_identity(transform_state);
}

PdCoreResult pd_editor_transform_state_translate(
    PdEditorTransformState* transform_state,
    float delta_x,
    float delta_y,
    float delta_z)
{
    if (transform_state == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    transform_state->position[0] += delta_x;
    transform_state->position[1] += delta_y;
    transform_state->position[2] += delta_z;
    return PD_CORE_RESULT_OK;
}

PdCoreResult pd_editor_transform_state_rotate_degrees(
    PdEditorTransformState* transform_state,
    float delta_x_degrees,
    float delta_y_degrees,
    float delta_z_degrees)
{
    if (transform_state == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    transform_state->rotation_degrees[0] += delta_x_degrees;
    transform_state->rotation_degrees[1] += delta_y_degrees;
    transform_state->rotation_degrees[2] += delta_z_degrees;
    return PD_CORE_RESULT_OK;
}

PdCoreResult pd_editor_transform_state_scale_uniform(PdEditorTransformState* transform_state, float scale_delta)
{
    float next_scale;

    if (transform_state == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    next_scale = transform_state->scale[0] + scale_delta;
    if (next_scale <= 0.05f) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    transform_state->scale[0] = next_scale;
    transform_state->scale[1] = next_scale;
    transform_state->scale[2] = next_scale;
    return PD_CORE_RESULT_OK;
}
