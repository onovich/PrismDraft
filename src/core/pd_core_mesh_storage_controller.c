#include "prismdraft/core/pd_core_mesh_storage_controller.h"

#include <stdint.h>
#include <stdlib.h>

static PdCoreResult pd_core_mesh_storage_controller_local_resize(
    void** storage,
    uint32_t* current_capacity,
    uint32_t requested_capacity,
    size_t element_size)
{
    void* resized_storage;
    size_t requested_size;

    if (storage == NULL || current_capacity == NULL || element_size == 0u) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (requested_capacity <= *current_capacity) {
        return PD_CORE_RESULT_OK;
    }

    if ((size_t)requested_capacity > (SIZE_MAX / element_size)) {
        return PD_CORE_RESULT_ERROR_OUT_OF_MEMORY;
    }

    requested_size = (size_t)requested_capacity * element_size;
    resized_storage = realloc(*storage, requested_size);
    if (resized_storage == NULL) {
        return PD_CORE_RESULT_ERROR_OUT_OF_MEMORY;
    }

    *storage = resized_storage;
    *current_capacity = requested_capacity;
    return PD_CORE_RESULT_OK;
}

PdCoreResult pd_core_mesh_storage_controller_init(
    PdCoreMeshEntity* mesh_entity,
    PdCoreMeshStorageConfig config)
{
    PdCoreResult result;

    if (mesh_entity == NULL) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    *mesh_entity = (PdCoreMeshEntity){ 0 };

    result = pd_core_mesh_storage_controller_reserve_vertices(mesh_entity, config.vertex_capacity);
    if (result != PD_CORE_RESULT_OK) {
        pd_core_mesh_storage_controller_free(mesh_entity);
        return result;
    }

    result = pd_core_mesh_storage_controller_reserve_half_edges(mesh_entity, config.half_edge_capacity);
    if (result != PD_CORE_RESULT_OK) {
        pd_core_mesh_storage_controller_free(mesh_entity);
        return result;
    }

    result = pd_core_mesh_storage_controller_reserve_faces(mesh_entity, config.face_capacity);
    if (result != PD_CORE_RESULT_OK) {
        pd_core_mesh_storage_controller_free(mesh_entity);
        return result;
    }

    return PD_CORE_RESULT_OK;
}

void pd_core_mesh_storage_controller_free(PdCoreMeshEntity* mesh_entity)
{
    if (mesh_entity == NULL) {
        return;
    }

    free(mesh_entity->vertices);
    free(mesh_entity->half_edges);
    free(mesh_entity->faces);

    *mesh_entity = (PdCoreMeshEntity){ 0 };
}

PdCoreResult pd_core_mesh_storage_controller_reserve_vertices(
    PdCoreMeshEntity* mesh_entity,
    uint32_t vertex_capacity)
{
    if (mesh_entity == NULL) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    return pd_core_mesh_storage_controller_local_resize(
        (void**)&mesh_entity->vertices,
        &mesh_entity->vertex_capacity,
        vertex_capacity,
        sizeof(PdCoreVertexEntity));
}

PdCoreResult pd_core_mesh_storage_controller_reserve_half_edges(
    PdCoreMeshEntity* mesh_entity,
    uint32_t half_edge_capacity)
{
    if (mesh_entity == NULL) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    return pd_core_mesh_storage_controller_local_resize(
        (void**)&mesh_entity->half_edges,
        &mesh_entity->half_edge_capacity,
        half_edge_capacity,
        sizeof(PdCoreHalfEdgeEntity));
}

PdCoreResult pd_core_mesh_storage_controller_reserve_faces(
    PdCoreMeshEntity* mesh_entity,
    uint32_t face_capacity)
{
    if (mesh_entity == NULL) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    return pd_core_mesh_storage_controller_local_resize(
        (void**)&mesh_entity->faces,
        &mesh_entity->face_capacity,
        face_capacity,
        sizeof(PdCoreFaceEntity));
}
