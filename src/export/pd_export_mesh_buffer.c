#include "prismdraft/export/pd_export_mesh_buffer.h"

#include "prismdraft/core/pd_core_mesh_validator.h"

#include <stdint.h>
#include <stdlib.h>

static PdCoreResult pd_export_mesh_buffer_local_count_face_edges(
    const PdCoreMeshEntity* mesh_entity,
    uint32_t face_index,
    uint32_t* edge_count)
{
    uint32_t start_half_edge_index;
    uint32_t current_half_edge_index;
    uint32_t count = 0u;

    if (mesh_entity == 0 || edge_count == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    start_half_edge_index = mesh_entity->faces[face_index].half_edge;
    current_half_edge_index = start_half_edge_index;

    do {
        current_half_edge_index = mesh_entity->half_edges[current_half_edge_index].next_half_edge;
        count++;
    } while (current_half_edge_index != start_half_edge_index);

    *edge_count = count;
    return PD_CORE_RESULT_OK;
}

static PdCoreResult pd_export_mesh_buffer_local_count_vertices(
    const PdCoreMeshEntity* mesh_entity,
    uint32_t* vertex_count)
{
    uint32_t face_index;
    uint32_t total_vertex_count = 0u;

    if (mesh_entity == 0 || vertex_count == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    for (face_index = 0u; face_index < mesh_entity->face_count; face_index++) {
        uint32_t face_edge_count = 0u;
        PdCoreResult result = pd_export_mesh_buffer_local_count_face_edges(mesh_entity, face_index, &face_edge_count);

        if (result != PD_CORE_RESULT_OK) {
            return result;
        }

        if (face_edge_count < 3u || face_edge_count > ((UINT32_MAX / 3u) + 2u)) {
            return PD_CORE_RESULT_ERROR_TOPOLOGY_INVALID;
        }

        if (total_vertex_count > UINT32_MAX - ((face_edge_count - 2u) * 3u)) {
            return PD_CORE_RESULT_ERROR_OUT_OF_MEMORY;
        }

        total_vertex_count += (face_edge_count - 2u) * 3u;
    }

    *vertex_count = total_vertex_count;
    return PD_CORE_RESULT_OK;
}

static PdCoreResult pd_export_mesh_buffer_local_reserve(
    PdExportMeshBuffer* export_mesh_buffer,
    uint32_t vertex_capacity)
{
    PdExportMeshBufferVertex* resized_vertices;
    size_t requested_size;

    if (export_mesh_buffer == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (vertex_capacity <= export_mesh_buffer->vertex_capacity) {
        return PD_CORE_RESULT_OK;
    }

    if ((size_t)vertex_capacity > (SIZE_MAX / sizeof(PdExportMeshBufferVertex))) {
        return PD_CORE_RESULT_ERROR_OUT_OF_MEMORY;
    }

    requested_size = (size_t)vertex_capacity * sizeof(PdExportMeshBufferVertex);
    resized_vertices = (PdExportMeshBufferVertex*)realloc(export_mesh_buffer->vertices, requested_size);
    if (resized_vertices == 0) {
        return PD_CORE_RESULT_ERROR_OUT_OF_MEMORY;
    }

    export_mesh_buffer->vertices = resized_vertices;
    export_mesh_buffer->vertex_capacity = vertex_capacity;
    return PD_CORE_RESULT_OK;
}

static void pd_export_mesh_buffer_local_write_vertex(
    PdExportMeshBufferVertex* export_vertex,
    const PdCoreVertexEntity* source_vertex,
    const PdCoreFaceEntity* source_face)
{
    export_vertex->position[0] = source_vertex->position[0];
    export_vertex->position[1] = source_vertex->position[1];
    export_vertex->position[2] = source_vertex->position[2];

    export_vertex->normal[0] = source_face->face_normal[0];
    export_vertex->normal[1] = source_face->face_normal[1];
    export_vertex->normal[2] = source_face->face_normal[2];

    export_vertex->color[0] = (float)source_face->base_color[0] / 255.0f;
    export_vertex->color[1] = (float)source_face->base_color[1] / 255.0f;
    export_vertex->color[2] = (float)source_face->base_color[2] / 255.0f;
    export_vertex->color[3] = (float)source_face->base_color[3] / 255.0f;
}

static void pd_export_mesh_buffer_local_write_face(
    PdExportMeshBuffer* export_mesh_buffer,
    const PdCoreMeshEntity* mesh_entity,
    uint32_t face_index,
    uint32_t* write_index)
{
    const PdCoreFaceEntity* face = &mesh_entity->faces[face_index];
    uint32_t first_half_edge_index = face->half_edge;
    uint32_t previous_half_edge_index = mesh_entity->half_edges[first_half_edge_index].next_half_edge;
    uint32_t current_half_edge_index = mesh_entity->half_edges[previous_half_edge_index].next_half_edge;

    while (current_half_edge_index != first_half_edge_index) {
        const PdCoreVertexEntity* first_vertex =
            &mesh_entity->vertices[mesh_entity->half_edges[first_half_edge_index].vertex];
        const PdCoreVertexEntity* previous_vertex =
            &mesh_entity->vertices[mesh_entity->half_edges[previous_half_edge_index].vertex];
        const PdCoreVertexEntity* current_vertex =
            &mesh_entity->vertices[mesh_entity->half_edges[current_half_edge_index].vertex];

        pd_export_mesh_buffer_local_write_vertex(&export_mesh_buffer->vertices[*write_index], first_vertex, face);
        (*write_index)++;
        pd_export_mesh_buffer_local_write_vertex(&export_mesh_buffer->vertices[*write_index], previous_vertex, face);
        (*write_index)++;
        pd_export_mesh_buffer_local_write_vertex(&export_mesh_buffer->vertices[*write_index], current_vertex, face);
        (*write_index)++;

        previous_half_edge_index = current_half_edge_index;
        current_half_edge_index = mesh_entity->half_edges[current_half_edge_index].next_half_edge;
    }
}

void pd_export_mesh_buffer_free(PdExportMeshBuffer* export_mesh_buffer)
{
    if (export_mesh_buffer == 0) {
        return;
    }

    free(export_mesh_buffer->vertices);
    *export_mesh_buffer = (PdExportMeshBuffer){ 0 };
}

PdCoreResult pd_export_mesh_buffer_build_from_mesh(
    PdExportMeshBuffer* export_mesh_buffer,
    const PdCoreMeshEntity* mesh_entity)
{
    uint32_t face_index;
    uint32_t required_vertex_count = 0u;
    uint32_t write_index = 0u;
    PdCoreResult result;

    if (export_mesh_buffer == 0 || mesh_entity == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    result = pd_core_mesh_validator_check(mesh_entity);
    if (result != PD_CORE_RESULT_OK) {
        return result;
    }

    result = pd_export_mesh_buffer_local_count_vertices(mesh_entity, &required_vertex_count);
    if (result != PD_CORE_RESULT_OK) {
        return result;
    }

    result = pd_export_mesh_buffer_local_reserve(export_mesh_buffer, required_vertex_count);
    if (result != PD_CORE_RESULT_OK) {
        return result;
    }

    for (face_index = 0u; face_index < mesh_entity->face_count; face_index++) {
        pd_export_mesh_buffer_local_write_face(export_mesh_buffer, mesh_entity, face_index, &write_index);
    }

    export_mesh_buffer->vertex_count = write_index;
    return PD_CORE_RESULT_OK;
}
