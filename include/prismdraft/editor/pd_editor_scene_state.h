#ifndef PRISMDRAFT_EDITOR_PD_EDITOR_SCENE_STATE_H
#define PRISMDRAFT_EDITOR_PD_EDITOR_SCENE_STATE_H

#include "prismdraft/core/pd_core_mesh_entity.h"
#include "prismdraft/core/pd_core_result_entity.h"
#include "prismdraft/editor/pd_editor_transform_state.h"

#include <stdint.h>

#define PD_EDITOR_SCENE_STATE_MAX_OBJECTS 8u

typedef struct PdEditorSceneObjectEntity {
    uint32_t object_id;
    PdCoreMeshEntity mesh;
    PdEditorTransformState transform_state;
    int is_allocated;
} PdEditorSceneObjectEntity;

typedef struct PdEditorSceneState {
    PdEditorSceneObjectEntity objects[PD_EDITOR_SCENE_STATE_MAX_OBJECTS];
    uint32_t object_count;
    uint32_t active_object_index;
    uint32_t next_object_id;
} PdEditorSceneState;

PdCoreResult pd_editor_scene_state_init(PdEditorSceneState* scene_state);

void pd_editor_scene_state_free(PdEditorSceneState* scene_state);

PdCoreResult pd_editor_scene_state_create_cube(PdEditorSceneState* scene_state, uint32_t* object_index);

PdCoreResult pd_editor_scene_state_set_active(PdEditorSceneState* scene_state, uint32_t object_index);

PdEditorSceneObjectEntity* pd_editor_scene_state_get_active(PdEditorSceneState* scene_state);

const PdEditorSceneObjectEntity* pd_editor_scene_state_get_active_const(const PdEditorSceneState* scene_state);

#endif
