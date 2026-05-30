#ifndef PRISMDRAFT_EDITOR_PD_EDITOR_MODELING_SERVICE_H
#define PRISMDRAFT_EDITOR_PD_EDITOR_MODELING_SERVICE_H

#include "prismdraft/core/pd_core_mesh_entity.h"
#include "prismdraft/core/pd_core_result_entity.h"
#include "prismdraft/editor/pd_editor_tool_state.h"

#include <stdint.h>

typedef struct PdEditorModelingServiceConfig {
    float inset_ratio;
    float extrude_distance;
    float bevel_inset_ratio;
    float bevel_distance;
} PdEditorModelingServiceConfig;

PdEditorModelingServiceConfig pd_editor_modeling_service_config_default(void);

PdCoreResult pd_editor_modeling_service_apply(
    PdCoreMeshEntity* mesh_entity,
    uint32_t face_index,
    PdEditorToolKind tool_kind,
    PdEditorModelingServiceConfig config);

#endif
