#ifndef PRISMDRAFT_CORE_PD_CORE_FACE_LOOP_CUT_CONTROLLER_H
#define PRISMDRAFT_CORE_PD_CORE_FACE_LOOP_CUT_CONTROLLER_H

#include "prismdraft/core/pd_core_mesh_entity.h"
#include "prismdraft/core/pd_core_result_entity.h"

#include <stdint.h>

PdCoreResult pd_core_face_loop_cut_controller_apply_quad(PdCoreMeshEntity* mesh_entity, uint32_t face_index);

#endif
