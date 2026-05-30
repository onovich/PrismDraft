#ifndef PRISMDRAFT_EDITOR_PD_EDITOR_TRANSFORM_STATE_H
#define PRISMDRAFT_EDITOR_PD_EDITOR_TRANSFORM_STATE_H

#include "prismdraft/core/pd_core_result_entity.h"

typedef struct PdEditorTransformState {
    float position[3];
    float rotation_degrees[3];
    float scale[3];
} PdEditorTransformState;

PdCoreResult pd_editor_transform_state_init(PdEditorTransformState* transform_state);

void pd_editor_transform_state_reset(PdEditorTransformState* transform_state);

PdCoreResult pd_editor_transform_state_translate(
    PdEditorTransformState* transform_state,
    float delta_x,
    float delta_y,
    float delta_z);

PdCoreResult pd_editor_transform_state_rotate_degrees(
    PdEditorTransformState* transform_state,
    float delta_x_degrees,
    float delta_y_degrees,
    float delta_z_degrees);

PdCoreResult pd_editor_transform_state_scale_uniform(PdEditorTransformState* transform_state, float scale_delta);

#endif
