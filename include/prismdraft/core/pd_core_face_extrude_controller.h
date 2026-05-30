#ifndef PRISMDRAFT_CORE_PD_CORE_FACE_EXTRUDE_CONTROLLER_H
#define PRISMDRAFT_CORE_PD_CORE_FACE_EXTRUDE_CONTROLLER_H

#include "prismdraft/core/pd_core_mesh_entity.h"
#include "prismdraft/core/pd_core_result_entity.h"

#include <stdint.h>

#define PD_CORE_FACE_EXTRUDE_CONTROLLER_MAX_FACE_EDGES 64u

PdCoreResult pd_core_face_extrude_controller_apply(
    PdCoreMeshEntity* mesh_entity,
    uint32_t face_index,
    float distance);

#endif
