#ifndef PRISMDRAFT_APP_PD_APP_CONTEXT_ENTITY_H
#define PRISMDRAFT_APP_PD_APP_CONTEXT_ENTITY_H

#include "prismdraft/core/pd_core_mesh_entity.h"
#include "prismdraft/editor/pd_editor_selection_state.h"
#include "prismdraft/editor/pd_editor_transform_state.h"

typedef struct PdAppContextEntity {
    PdCoreMeshEntity active_mesh;
    PdEditorSelectionState selection_state;
    PdEditorTransformState transform_state;
    int is_running;
} PdAppContextEntity;

#endif
