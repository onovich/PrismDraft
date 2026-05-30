#include "prismdraft/core/pd_core_mesh_validator.h"

#include <stdint.h>

static PdCoreResult pd_core_mesh_validator_local_check_storage(const PdCoreMeshEntity* mesh_entity)
{
    if (mesh_entity == NULL) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (mesh_entity->vertex_count > mesh_entity->vertex_capacity ||
        mesh_entity->half_edge_count > mesh_entity->half_edge_capacity ||
        mesh_entity->face_count > mesh_entity->face_capacity) {
        return PD_CORE_RESULT_ERROR_TOPOLOGY_INVALID;
    }

    if ((mesh_entity->vertex_count > 0u && mesh_entity->vertices == 0) ||
        (mesh_entity->half_edge_count > 0u && mesh_entity->half_edges == 0) ||
        (mesh_entity->face_count > 0u && mesh_entity->faces == 0)) {
        return PD_CORE_RESULT_ERROR_TOPOLOGY_INVALID;
    }

    return PD_CORE_RESULT_OK;
}

static PdCoreResult pd_core_mesh_validator_local_check_vertex_indices(const PdCoreMeshEntity* mesh_entity)
{
    uint32_t vertex_index;

    for (vertex_index = 0u; vertex_index < mesh_entity->vertex_count; vertex_index++) {
        uint32_t half_edge_index = mesh_entity->vertices[vertex_index].half_edge;
        if (half_edge_index != PD_CORE_MESH_ENTITY_INVALID_INDEX &&
            half_edge_index >= mesh_entity->half_edge_count) {
            return PD_CORE_RESULT_ERROR_TOPOLOGY_INVALID;
        }
    }

    return PD_CORE_RESULT_OK;
}

static PdCoreResult pd_core_mesh_validator_local_check_half_edge_indices(const PdCoreMeshEntity* mesh_entity)
{
    uint32_t half_edge_index;

    for (half_edge_index = 0u; half_edge_index < mesh_entity->half_edge_count; half_edge_index++) {
        const PdCoreHalfEdgeEntity* half_edge = &mesh_entity->half_edges[half_edge_index];

        if (half_edge->next_half_edge >= mesh_entity->half_edge_count ||
            half_edge->vertex >= mesh_entity->vertex_count ||
            half_edge->face >= mesh_entity->face_count) {
            return PD_CORE_RESULT_ERROR_TOPOLOGY_INVALID;
        }

        if (half_edge->pair_half_edge != PD_CORE_MESH_ENTITY_INVALID_INDEX) {
            const PdCoreHalfEdgeEntity* pair_half_edge;

            if (half_edge->pair_half_edge >= mesh_entity->half_edge_count) {
                return PD_CORE_RESULT_ERROR_TOPOLOGY_INVALID;
            }

            pair_half_edge = &mesh_entity->half_edges[half_edge->pair_half_edge];
            if (pair_half_edge->pair_half_edge != half_edge_index) {
                return PD_CORE_RESULT_ERROR_TOPOLOGY_INVALID;
            }
        }
    }

    return PD_CORE_RESULT_OK;
}

static PdCoreResult pd_core_mesh_validator_local_check_face_loop(
    const PdCoreMeshEntity* mesh_entity,
    uint32_t face_index)
{
    uint32_t start_half_edge_index = mesh_entity->faces[face_index].half_edge;
    uint32_t current_half_edge_index = start_half_edge_index;
    uint32_t step_count = 0u;

    if (start_half_edge_index >= mesh_entity->half_edge_count) {
        return PD_CORE_RESULT_ERROR_TOPOLOGY_INVALID;
    }

    do {
        const PdCoreHalfEdgeEntity* half_edge;

        if (current_half_edge_index >= mesh_entity->half_edge_count) {
            return PD_CORE_RESULT_ERROR_TOPOLOGY_INVALID;
        }

        half_edge = &mesh_entity->half_edges[current_half_edge_index];
        if (half_edge->face != face_index) {
            return PD_CORE_RESULT_ERROR_TOPOLOGY_INVALID;
        }

        current_half_edge_index = half_edge->next_half_edge;
        step_count++;

        if (step_count > mesh_entity->half_edge_count) {
            return PD_CORE_RESULT_ERROR_TOPOLOGY_INVALID;
        }
    } while (current_half_edge_index != start_half_edge_index);

    if (step_count < 3u) {
        return PD_CORE_RESULT_ERROR_TOPOLOGY_INVALID;
    }

    return PD_CORE_RESULT_OK;
}

static PdCoreResult pd_core_mesh_validator_local_check_faces(const PdCoreMeshEntity* mesh_entity)
{
    uint32_t face_index;

    for (face_index = 0u; face_index < mesh_entity->face_count; face_index++) {
        PdCoreResult result = pd_core_mesh_validator_local_check_face_loop(mesh_entity, face_index);
        if (result != PD_CORE_RESULT_OK) {
            return result;
        }
    }

    return PD_CORE_RESULT_OK;
}

PdCoreResult pd_core_mesh_validator_check(const PdCoreMeshEntity* mesh_entity)
{
    PdCoreResult result = pd_core_mesh_validator_local_check_storage(mesh_entity);
    if (result != PD_CORE_RESULT_OK) {
        return result;
    }

    result = pd_core_mesh_validator_local_check_vertex_indices(mesh_entity);
    if (result != PD_CORE_RESULT_OK) {
        return result;
    }

    result = pd_core_mesh_validator_local_check_half_edge_indices(mesh_entity);
    if (result != PD_CORE_RESULT_OK) {
        return result;
    }

    return pd_core_mesh_validator_local_check_faces(mesh_entity);
}
