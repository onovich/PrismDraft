#ifndef PRISMDRAFT_CORE_PD_CORE_MESH_STORAGE_CONTROLLER_H
#define PRISMDRAFT_CORE_PD_CORE_MESH_STORAGE_CONTROLLER_H

#include <stdint.h>

#include "prismdraft/core/pd_core_mesh_entity.h"
#include "prismdraft/core/pd_core_mesh_storage_config.h"
#include "prismdraft/core/pd_core_result_entity.h"

PdCoreResult pd_core_mesh_storage_controller_init(
    PdCoreMeshEntity* mesh_entity,
    PdCoreMeshStorageConfig config);

void pd_core_mesh_storage_controller_free(PdCoreMeshEntity* mesh_entity);

PdCoreResult pd_core_mesh_storage_controller_reserve_vertices(
    PdCoreMeshEntity* mesh_entity,
    uint32_t vertex_capacity);

PdCoreResult pd_core_mesh_storage_controller_reserve_half_edges(
    PdCoreMeshEntity* mesh_entity,
    uint32_t half_edge_capacity);

PdCoreResult pd_core_mesh_storage_controller_reserve_faces(
    PdCoreMeshEntity* mesh_entity,
    uint32_t face_capacity);

#endif
