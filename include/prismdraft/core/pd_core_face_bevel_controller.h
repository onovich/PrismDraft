#ifndef PRISMDRAFT_CORE_PD_CORE_FACE_BEVEL_CONTROLLER_H
#define PRISMDRAFT_CORE_PD_CORE_FACE_BEVEL_CONTROLLER_H

#include "prismdraft/core/pd_core_mesh_entity.h"
#include "prismdraft/core/pd_core_result_entity.h"

#include <stdint.h>

PdCoreResult pd_core_face_bevel_controller_apply(
    PdCoreMeshEntity* mesh_entity,
    uint32_t face_index,
    float inset_ratio,
    float distance);

#endif
