#include "prismdraft/core/pd_core_face_inset_controller.h"

#include "prismdraft/core/pd_core_mesh_storage_controller.h"
#include "prismdraft/core/pd_core_mesh_validator.h"

static PdCoreResult pd_core_face_inset_controller_local_collect_face(
    const PdCoreMeshEntity* mesh_entity,
    uint32_t face_index,
    uint32_t half_edges[PD_CORE_FACE_INSET_CONTROLLER_MAX_FACE_EDGES],
    uint32_t vertices[PD_CORE_FACE_INSET_CONTROLLER_MAX_FACE_EDGES],
    uint32_t* edge_count)
{
    uint32_t start_half_edge_index;
    uint32_t current_half_edge_index;
    uint32_t count = 0u;

    if (mesh_entity == 0 || half_edges == 0 || vertices == 0 || edge_count == 0 || face_index >= mesh_entity->face_count) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    start_half_edge_index = mesh_entity->faces[face_index].half_edge;
    current_half_edge_index = start_half_edge_index;

    do {
        if (count >= PD_CORE_FACE_INSET_CONTROLLER_MAX_FACE_EDGES) {
            return PD_CORE_RESULT_ERROR_TOPOLOGY_INVALID;
        }

        half_edges[count] = current_half_edge_index;
        vertices[count] = mesh_entity->half_edges[current_half_edge_index].vertex;
        count++;
        current_half_edge_index = mesh_entity->half_edges[current_half_edge_index].next_half_edge;
    } while (current_half_edge_index != start_half_edge_index);

    if (count < 3u) {
        return PD_CORE_RESULT_ERROR_TOPOLOGY_INVALID;
    }

    *edge_count = count;
    return PD_CORE_RESULT_OK;
}

static void pd_core_face_inset_controller_local_compute_center(
    const PdCoreMeshEntity* mesh_entity,
    const uint32_t vertices[PD_CORE_FACE_INSET_CONTROLLER_MAX_FACE_EDGES],
    uint32_t edge_count,
    float center[3])
{
    uint32_t edge_index;

    center[0] = 0.0f;
    center[1] = 0.0f;
    center[2] = 0.0f;

    for (edge_index = 0u; edge_index < edge_count; edge_index++) {
        const PdCoreVertexEntity* vertex = &mesh_entity->vertices[vertices[edge_index]];
        center[0] += vertex->position[0];
        center[1] += vertex->position[1];
        center[2] += vertex->position[2];
    }

    center[0] /= (float)edge_count;
    center[1] /= (float)edge_count;
    center[2] /= (float)edge_count;
}

static void pd_core_face_inset_controller_local_write_inner_vertex(
    PdCoreMeshEntity* mesh_entity,
    uint32_t source_vertex_index,
    uint32_t inner_vertex_index,
    uint32_t inner_half_edge_index,
    const float center[3],
    const PdCoreFaceEntity* source_face,
    float inset_ratio)
{
    const PdCoreVertexEntity* source_vertex = &mesh_entity->vertices[source_vertex_index];
    PdCoreVertexEntity* inner_vertex = &mesh_entity->vertices[inner_vertex_index];

    inner_vertex->position[0] =
        source_vertex->position[0] + ((center[0] - source_vertex->position[0]) * inset_ratio);
    inner_vertex->position[1] =
        source_vertex->position[1] + ((center[1] - source_vertex->position[1]) * inset_ratio);
    inner_vertex->position[2] =
        source_vertex->position[2] + ((center[2] - source_vertex->position[2]) * inset_ratio);

    inner_vertex->normal[0] = source_face->face_normal[0];
    inner_vertex->normal[1] = source_face->face_normal[1];
    inner_vertex->normal[2] = source_face->face_normal[2];
    inner_vertex->color[0] = source_face->base_color[0];
    inner_vertex->color[1] = source_face->base_color[1];
    inner_vertex->color[2] = source_face->base_color[2];
    inner_vertex->color[3] = source_face->base_color[3];
    inner_vertex->half_edge = inner_half_edge_index;
}

static void pd_core_face_inset_controller_local_write_ring_face(
    PdCoreMeshEntity* mesh_entity,
    uint32_t ring_face_index,
    uint32_t ring_half_edge_index,
    const PdCoreFaceEntity* source_face)
{
    PdCoreFaceEntity* ring_face = &mesh_entity->faces[ring_face_index];

    ring_face->half_edge = ring_half_edge_index;
    ring_face->face_normal[0] = source_face->face_normal[0];
    ring_face->face_normal[1] = source_face->face_normal[1];
    ring_face->face_normal[2] = source_face->face_normal[2];
    ring_face->base_color[0] = source_face->base_color[0];
    ring_face->base_color[1] = source_face->base_color[1];
    ring_face->base_color[2] = source_face->base_color[2];
    ring_face->base_color[3] = source_face->base_color[3];
}

PdCoreResult pd_core_face_inset_controller_apply(
    PdCoreMeshEntity* mesh_entity,
    uint32_t face_index,
    float inset_ratio)
{
    uint32_t face_half_edges[PD_CORE_FACE_INSET_CONTROLLER_MAX_FACE_EDGES];
    uint32_t face_vertices[PD_CORE_FACE_INSET_CONTROLLER_MAX_FACE_EDGES];
    uint32_t edge_count = 0u;
    uint32_t old_vertex_count;
    uint32_t old_half_edge_count;
    uint32_t old_face_count;
    uint32_t inner_half_edge_start;
    uint32_t ring_half_edge_start;
    uint32_t ring_face_start;
    uint32_t edge_index;
    float center[3];
    PdCoreFaceEntity source_face;
    PdCoreResult result;

    /*
     * Topology proof for face inset:
     * Input invariants: mesh_entity validates, face_index is an n-gon with 3..64 closed half-edges, and inset_ratio is
     * within (0, 1). Allocated additions: n inner vertices, n inner-face half-edges, 3n ring half-edges, and n ring faces.
     * next_half_edge rewiring: original face becomes the inner face using inner half-edges; each original boundary
     * half-edge is moved into a generated quad ring face and followed by side, inner-opposite, and side-back half-edges.
     * pair_half_edge rewiring: original boundary pairs are preserved; each inner half-edge pairs with its opposite
     * ring half-edge; neighboring ring side half-edges pair with each other. Output invariants: all faces remain closed,
     * pair symmetry is restored, counts grow by n/4n/n, and pd_core_mesh_validator_check must return OK.
     */

    if (mesh_entity == 0 || face_index == PD_CORE_MESH_ENTITY_INVALID_INDEX || inset_ratio <= 0.0f ||
        inset_ratio >= 1.0f) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    result = pd_core_mesh_validator_check(mesh_entity);
    if (result != PD_CORE_RESULT_OK) {
        return result;
    }

    result = pd_core_face_inset_controller_local_collect_face(
        mesh_entity,
        face_index,
        face_half_edges,
        face_vertices,
        &edge_count);
    if (result != PD_CORE_RESULT_OK) {
        return result;
    }

    old_vertex_count = mesh_entity->vertex_count;
    old_half_edge_count = mesh_entity->half_edge_count;
    old_face_count = mesh_entity->face_count;

    result = pd_core_mesh_storage_controller_reserve_vertices(mesh_entity, old_vertex_count + edge_count);
    if (result != PD_CORE_RESULT_OK) {
        return result;
    }

    result = pd_core_mesh_storage_controller_reserve_half_edges(mesh_entity, old_half_edge_count + (edge_count * 4u));
    if (result != PD_CORE_RESULT_OK) {
        return result;
    }

    result = pd_core_mesh_storage_controller_reserve_faces(mesh_entity, old_face_count + edge_count);
    if (result != PD_CORE_RESULT_OK) {
        return result;
    }

    source_face = mesh_entity->faces[face_index];
    pd_core_face_inset_controller_local_compute_center(mesh_entity, face_vertices, edge_count, center);

    inner_half_edge_start = old_half_edge_count;
    ring_half_edge_start = inner_half_edge_start + edge_count;
    ring_face_start = old_face_count;

    for (edge_index = 0u; edge_index < edge_count; edge_index++) {
        uint32_t inner_vertex_index = old_vertex_count + edge_index;
        uint32_t inner_half_edge_index = inner_half_edge_start + edge_index;
        pd_core_face_inset_controller_local_write_inner_vertex(
            mesh_entity,
            face_vertices[edge_index],
            inner_vertex_index,
            inner_half_edge_index,
            center,
            &source_face,
            inset_ratio);
    }

    for (edge_index = 0u; edge_index < edge_count; edge_index++) {
        uint32_t next_edge_index = (edge_index + 1u) % edge_count;
        uint32_t previous_edge_index = (edge_index + edge_count - 1u) % edge_count;
        uint32_t original_half_edge_index = face_half_edges[edge_index];
        uint32_t inner_half_edge_index = inner_half_edge_start + edge_index;
        uint32_t inner_next_half_edge_index = inner_half_edge_start + next_edge_index;
        uint32_t side_next_half_edge_index = ring_half_edge_start + (edge_index * 3u);
        uint32_t inner_opposite_half_edge_index = side_next_half_edge_index + 1u;
        uint32_t side_back_half_edge_index = side_next_half_edge_index + 2u;
        uint32_t previous_side_next_half_edge_index = ring_half_edge_start + (previous_edge_index * 3u);
        uint32_t next_side_back_half_edge_index = ring_half_edge_start + (next_edge_index * 3u) + 2u;
        uint32_t inner_vertex_index = old_vertex_count + edge_index;
        uint32_t next_inner_vertex_index = old_vertex_count + next_edge_index;
        uint32_t ring_face_index = ring_face_start + edge_index;

        mesh_entity->half_edges[inner_half_edge_index].next_half_edge = inner_next_half_edge_index;
        mesh_entity->half_edges[inner_half_edge_index].pair_half_edge = inner_opposite_half_edge_index;
        mesh_entity->half_edges[inner_half_edge_index].vertex = inner_vertex_index;
        mesh_entity->half_edges[inner_half_edge_index].face = face_index;

        mesh_entity->half_edges[original_half_edge_index].next_half_edge = side_next_half_edge_index;
        mesh_entity->half_edges[original_half_edge_index].face = ring_face_index;

        mesh_entity->half_edges[side_next_half_edge_index].next_half_edge = inner_opposite_half_edge_index;
        mesh_entity->half_edges[side_next_half_edge_index].pair_half_edge = next_side_back_half_edge_index;
        mesh_entity->half_edges[side_next_half_edge_index].vertex = face_vertices[next_edge_index];
        mesh_entity->half_edges[side_next_half_edge_index].face = ring_face_index;

        mesh_entity->half_edges[inner_opposite_half_edge_index].next_half_edge = side_back_half_edge_index;
        mesh_entity->half_edges[inner_opposite_half_edge_index].pair_half_edge = inner_half_edge_index;
        mesh_entity->half_edges[inner_opposite_half_edge_index].vertex = next_inner_vertex_index;
        mesh_entity->half_edges[inner_opposite_half_edge_index].face = ring_face_index;

        mesh_entity->half_edges[side_back_half_edge_index].next_half_edge = original_half_edge_index;
        mesh_entity->half_edges[side_back_half_edge_index].pair_half_edge = previous_side_next_half_edge_index;
        mesh_entity->half_edges[side_back_half_edge_index].vertex = inner_vertex_index;
        mesh_entity->half_edges[side_back_half_edge_index].face = ring_face_index;

        pd_core_face_inset_controller_local_write_ring_face(
            mesh_entity,
            ring_face_index,
            original_half_edge_index,
            &source_face);
    }

    mesh_entity->faces[face_index] = source_face;
    mesh_entity->faces[face_index].half_edge = inner_half_edge_start;
    mesh_entity->vertex_count = old_vertex_count + edge_count;
    mesh_entity->half_edge_count = old_half_edge_count + (edge_count * 4u);
    mesh_entity->face_count = old_face_count + edge_count;

    return pd_core_mesh_validator_check(mesh_entity);
}
