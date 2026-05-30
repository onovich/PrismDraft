#include "prismdraft/render/pd_render_face_highlight_buffer.h"

#include "prismdraft/core/pd_core_mesh_validator.h"

#include <stdint.h>
#include <stdlib.h>

static PdCoreResult pd_render_face_highlight_buffer_local_count_face_edges(
    const PdCoreMeshEntity* mesh_entity,
    uint32_t face_index,
    uint32_t* edge_count)
{
    uint32_t start_half_edge_index;
    uint32_t current_half_edge_index;
    uint32_t count = 0u;

    if (mesh_entity == 0 || edge_count == 0 || face_index >= mesh_entity->face_count) {
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

static PdCoreResult pd_render_face_highlight_buffer_local_reserve(
    PdRenderMeshBuffer* render_mesh_buffer,
    uint32_t vertex_capacity)
{
    PdRenderMeshBufferVertex* resized_vertices;
    size_t requested_size;

    if (render_mesh_buffer == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (vertex_capacity <= render_mesh_buffer->vertex_capacity) {
        return PD_CORE_RESULT_OK;
    }

    if ((size_t)vertex_capacity > (SIZE_MAX / sizeof(PdRenderMeshBufferVertex))) {
        return PD_CORE_RESULT_ERROR_OUT_OF_MEMORY;
    }

    requested_size = (size_t)vertex_capacity * sizeof(PdRenderMeshBufferVertex);
    resized_vertices = (PdRenderMeshBufferVertex*)realloc(render_mesh_buffer->vertices, requested_size);
    if (resized_vertices == 0) {
        return PD_CORE_RESULT_ERROR_OUT_OF_MEMORY;
    }

    render_mesh_buffer->vertices = resized_vertices;
    render_mesh_buffer->vertex_capacity = vertex_capacity;
    return PD_CORE_RESULT_OK;
}

static void pd_render_face_highlight_buffer_local_write_vertex(
    PdRenderMeshBufferVertex* render_vertex,
    const PdCoreVertexEntity* source_vertex,
    const PdCoreFaceEntity* source_face,
    PdRenderFaceHighlightConfig config)
{
    render_vertex->position[0] =
        source_vertex->position[0] + (source_face->face_normal[0] * config.surface_offset);
    render_vertex->position[1] =
        source_vertex->position[1] + (source_face->face_normal[1] * config.surface_offset);
    render_vertex->position[2] =
        source_vertex->position[2] + (source_face->face_normal[2] * config.surface_offset);

    render_vertex->normal[0] = source_face->face_normal[0];
    render_vertex->normal[1] = source_face->face_normal[1];
    render_vertex->normal[2] = source_face->face_normal[2];

    render_vertex->color[0] = config.color[0];
    render_vertex->color[1] = config.color[1];
    render_vertex->color[2] = config.color[2];
    render_vertex->color[3] = config.color[3];
}

static void pd_render_face_highlight_buffer_local_write_face(
    PdRenderMeshBuffer* render_mesh_buffer,
    const PdCoreMeshEntity* mesh_entity,
    uint32_t face_index,
    PdRenderFaceHighlightConfig config)
{
    const PdCoreFaceEntity* face = &mesh_entity->faces[face_index];
    uint32_t first_half_edge_index = face->half_edge;
    uint32_t previous_half_edge_index = mesh_entity->half_edges[first_half_edge_index].next_half_edge;
    uint32_t current_half_edge_index = mesh_entity->half_edges[previous_half_edge_index].next_half_edge;
    uint32_t write_index = 0u;

    while (current_half_edge_index != first_half_edge_index) {
        const PdCoreVertexEntity* first_vertex =
            &mesh_entity->vertices[mesh_entity->half_edges[first_half_edge_index].vertex];
        const PdCoreVertexEntity* previous_vertex =
            &mesh_entity->vertices[mesh_entity->half_edges[previous_half_edge_index].vertex];
        const PdCoreVertexEntity* current_vertex =
            &mesh_entity->vertices[mesh_entity->half_edges[current_half_edge_index].vertex];

        pd_render_face_highlight_buffer_local_write_vertex(
            &render_mesh_buffer->vertices[write_index],
            first_vertex,
            face,
            config);
        write_index++;
        pd_render_face_highlight_buffer_local_write_vertex(
            &render_mesh_buffer->vertices[write_index],
            previous_vertex,
            face,
            config);
        write_index++;
        pd_render_face_highlight_buffer_local_write_vertex(
            &render_mesh_buffer->vertices[write_index],
            current_vertex,
            face,
            config);
        write_index++;

        previous_half_edge_index = current_half_edge_index;
        current_half_edge_index = mesh_entity->half_edges[current_half_edge_index].next_half_edge;
    }

    render_mesh_buffer->vertex_count = write_index;
}

PdCoreResult pd_render_face_highlight_buffer_build_for_face(
    PdRenderMeshBuffer* render_mesh_buffer,
    const PdCoreMeshEntity* mesh_entity,
    uint32_t face_index,
    PdRenderFaceHighlightConfig config)
{
    uint32_t face_edge_count = 0u;
    uint32_t required_vertex_count;
    PdCoreResult result;

    if (render_mesh_buffer == 0 || mesh_entity == 0 || !pd_render_face_highlight_config_is_valid(config)) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (face_index >= mesh_entity->face_count) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    result = pd_core_mesh_validator_check(mesh_entity);
    if (result != PD_CORE_RESULT_OK) {
        return result;
    }

    result = pd_render_face_highlight_buffer_local_count_face_edges(mesh_entity, face_index, &face_edge_count);
    if (result != PD_CORE_RESULT_OK) {
        return result;
    }

    if (face_edge_count < 3u || face_edge_count > ((UINT32_MAX / 3u) + 2u)) {
        return PD_CORE_RESULT_ERROR_TOPOLOGY_INVALID;
    }

    required_vertex_count = (face_edge_count - 2u) * 3u;
    result = pd_render_face_highlight_buffer_local_reserve(render_mesh_buffer, required_vertex_count);
    if (result != PD_CORE_RESULT_OK) {
        return result;
    }

    pd_render_face_highlight_buffer_local_write_face(render_mesh_buffer, mesh_entity, face_index, config);
    return PD_CORE_RESULT_OK;
}
