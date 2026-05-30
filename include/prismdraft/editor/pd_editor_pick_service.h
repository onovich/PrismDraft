#ifndef PRISMDRAFT_EDITOR_PD_EDITOR_PICK_SERVICE_H
#define PRISMDRAFT_EDITOR_PD_EDITOR_PICK_SERVICE_H

#include "prismdraft/core/pd_core_mesh_entity.h"
#include "prismdraft/core/pd_core_result_entity.h"

#include <stdint.h>

typedef struct PdEditorPickServiceHit {
    int has_hit;
    uint32_t face_index;
    float distance;
    float position[3];
} PdEditorPickServiceHit;

PdCoreResult pd_editor_pick_service_pick_face(
    const PdCoreMeshEntity* mesh_entity,
    const float ray_origin[3],
    const float ray_direction[3],
    PdEditorPickServiceHit* hit);

#endif
