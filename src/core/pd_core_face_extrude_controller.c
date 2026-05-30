#include "prismdraft/core/pd_core_face_extrude_controller.h"

#include "prismdraft/core/pd_core_mesh_storage_controller.h"
#include "prismdraft/core/pd_core_mesh_validator.h"

#include <math.h>

static PdCoreResult pd_core_face_extrude_controller_local_collect_face(
    const PdCoreMeshEntity* mesh_entity,
    uint32_t face_index,
    uint32_t half_edges[PD_CORE_FACE_EXTRUDE_CONTROLLER_MAX_FACE_EDGES],
    uint32_t vertices[PD_CORE_FACE_EXTRUDE_CONTROLLER_MAX_FACE_EDGES],
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
        if (count >= PD_CORE_FACE_EXTRUDE_CONTROLLER_MAX_FACE_EDGES) {
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

static void pd_core_face_extrude_controller_local_subtract(const float left[3], const float right[3], float result[3])
{
    result[0] = left[0] - right[0];
    result[1] = left[1] - right[1];
    result[2] = left[2] - right[2];
}

static void pd_core_face_extrude_controller_local_cross(const float left[3], const float right[3], float result[3])
{
    result[0] = (left[1] * right[2]) - (left[2] * right[1]);
    result[1] = (left[2] * right[0]) - (left[0] * right[2]);
    result[2] = (left[0] * right[1]) - (left[1] * right[0]);
}

static void pd_core_face_extrude_controller_local_normalize(float value[3])
{
    float length = sqrtf((value[0] * value[0]) + (value[1] * value[1]) + (value[2] * value[2]));

    if (length <= 0.000001f) {
        return;
    }

    value[0] /= length;
    value[1] /= length;
    value[2] /= length;
}

static void pd_core_face_extrude_controller_local_compute_side_normal(
    const PdCoreMeshEntity* mesh_entity,
    uint32_t source_vertex_index,
    uint32_t next_source_vertex_index,
    uint32_t next_extruded_vertex_index,
    float normal[3])
{
    float edge_a[3];
    float edge_b[3];
    const float* source_position = mesh_entity->vertices[source_vertex_index].position;
    const float* next_source_position = mesh_entity->vertices[next_source_vertex_index].position;
    const float* next_extruded_position = mesh_entity->vertices[next_extruded_vertex_index].position;

    pd_core_face_extrude_controller_local_subtract(next_source_position, source_position, edge_a);
    pd_core_face_extrude_controller_local_subtract(next_extruded_position, source_position, edge_b);
    pd_core_face_extrude_controller_local_cross(edge_a, edge_b, normal);
    pd_core_face_extrude_controller_local_normalize(normal);
}

static void pd_core_face_extrude_controller_local_write_extruded_vertex(
    PdCoreMeshEntity* mesh_entity,
    uint32_t source_vertex_index,
    uint32_t extruded_vertex_index,
    uint32_t extruded_half_edge_index,
    const PdCoreFaceEntity* source_face,
    float distance)
{
    const PdCoreVertexEntity* source_vertex = &mesh_entity->vertices[source_vertex_index];
    PdCoreVertexEntity* extruded_vertex = &mesh_entity->vertices[extruded_vertex_index];

    extruded_vertex->position[0] = source_vertex->position[0] + (source_face->face_normal[0] * distance);
    extruded_vertex->position[1] = source_vertex->position[1] + (source_face->face_normal[1] * distance);
    extruded_vertex->position[2] = source_vertex->position[2] + (source_face->face_normal[2] * distance);
    extruded_vertex->normal[0] = source_face->face_normal[0];
    extruded_vertex->normal[1] = source_face->face_normal[1];
    extruded_vertex->normal[2] = source_face->face_normal[2];
    extruded_vertex->color[0] = source_face->base_color[0];
    extruded_vertex->color[1] = source_face->base_color[1];
    extruded_vertex->color[2] = source_face->base_color[2];
    extruded_vertex->color[3] = source_face->base_color[3];
    extruded_vertex->half_edge = extruded_half_edge_index;
}

static void pd_core_face_extrude_controller_local_write_ring_face(
    PdCoreMeshEntity* mesh_entity,
    uint32_t ring_face_index,
    uint32_t ring_half_edge_index,
    const PdCoreFaceEntity* source_face,
    const float side_normal[3])
{
    PdCoreFaceEntity* ring_face = &mesh_entity->faces[ring_face_index];

    ring_face->half_edge = ring_half_edge_index;
    ring_face->face_normal[0] = side_normal[0];
    ring_face->face_normal[1] = side_normal[1];
    ring_face->face_normal[2] = side_normal[2];
    ring_face->base_color[0] = source_face->base_color[0];
    ring_face->base_color[1] = source_face->base_color[1];
    ring_face->base_color[2] = source_face->base_color[2];
    ring_face->base_color[3] = source_face->base_color[3];
}

PdCoreResult pd_core_face_extrude_controller_apply(
    PdCoreMeshEntity* mesh_entity,
    uint32_t face_index,
    float distance)
{
    uint32_t face_half_edges[PD_CORE_FACE_EXTRUDE_CONTROLLER_MAX_FACE_EDGES];
    uint32_t face_vertices[PD_CORE_FACE_EXTRUDE_CONTROLLER_MAX_FACE_EDGES];
    uint32_t edge_count = 0u;
    uint32_t old_vertex_count;
    uint32_t old_half_edge_count;
    uint32_t old_face_count;
    uint32_t cap_half_edge_start;
    uint32_t side_half_edge_start;
    uint32_t side_face_start;
    uint32_t edge_index;
    PdCoreFaceEntity source_face;
    PdCoreResult result;

    /*
     * Topology proof for face extrude:
     * Input invariants: mesh_entity validates, face_index is an n-gon with 3..64 closed half-edges, and distance is
     * positive. Allocated additions: n offset vertices, n cap half-edges, 3n side half-edges, and n side faces.
     * next_half_edge rewiring: original face becomes the offset cap using cap half-edges; each original boundary
     * half-edge is moved into a generated quad side face followed by side-up, cap-opposite, and side-down half-edges.
     * pair_half_edge rewiring: original boundary pairs are preserved; each cap half-edge pairs with its opposite side
     * half-edge; neighboring vertical side half-edges pair with each other. Output invariants: all loops are closed,
     * pair symmetry is restored, counts grow by n/4n/n, and pd_core_mesh_validator_check must return OK.
     */

    if (mesh_entity == 0 || face_index == PD_CORE_MESH_ENTITY_INVALID_INDEX || distance <= 0.0f) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    result = pd_core_mesh_validator_check(mesh_entity);
    if (result != PD_CORE_RESULT_OK) {
        return result;
    }

    result = pd_core_face_extrude_controller_local_collect_face(
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
    cap_half_edge_start = old_half_edge_count;
    side_half_edge_start = cap_half_edge_start + edge_count;
    side_face_start = old_face_count;

    for (edge_index = 0u; edge_index < edge_count; edge_index++) {
        uint32_t extruded_vertex_index = old_vertex_count + edge_index;
        uint32_t cap_half_edge_index = cap_half_edge_start + edge_index;
        pd_core_face_extrude_controller_local_write_extruded_vertex(
            mesh_entity,
            face_vertices[edge_index],
            extruded_vertex_index,
            cap_half_edge_index,
            &source_face,
            distance);
    }

    for (edge_index = 0u; edge_index < edge_count; edge_index++) {
        uint32_t next_edge_index = (edge_index + 1u) % edge_count;
        uint32_t previous_edge_index = (edge_index + edge_count - 1u) % edge_count;
        uint32_t original_half_edge_index = face_half_edges[edge_index];
        uint32_t cap_half_edge_index = cap_half_edge_start + edge_index;
        uint32_t cap_next_half_edge_index = cap_half_edge_start + next_edge_index;
        uint32_t side_up_half_edge_index = side_half_edge_start + (edge_index * 3u);
        uint32_t cap_opposite_half_edge_index = side_up_half_edge_index + 1u;
        uint32_t side_down_half_edge_index = side_up_half_edge_index + 2u;
        uint32_t previous_side_up_half_edge_index = side_half_edge_start + (previous_edge_index * 3u);
        uint32_t next_side_down_half_edge_index = side_half_edge_start + (next_edge_index * 3u) + 2u;
        uint32_t extruded_vertex_index = old_vertex_count + edge_index;
        uint32_t next_extruded_vertex_index = old_vertex_count + next_edge_index;
        uint32_t side_face_index = side_face_start + edge_index;
        float side_normal[3];

        mesh_entity->half_edges[cap_half_edge_index].next_half_edge = cap_next_half_edge_index;
        mesh_entity->half_edges[cap_half_edge_index].pair_half_edge = cap_opposite_half_edge_index;
        mesh_entity->half_edges[cap_half_edge_index].vertex = extruded_vertex_index;
        mesh_entity->half_edges[cap_half_edge_index].face = face_index;

        mesh_entity->half_edges[original_half_edge_index].next_half_edge = side_up_half_edge_index;
        mesh_entity->half_edges[original_half_edge_index].face = side_face_index;

        mesh_entity->half_edges[side_up_half_edge_index].next_half_edge = cap_opposite_half_edge_index;
        mesh_entity->half_edges[side_up_half_edge_index].pair_half_edge = next_side_down_half_edge_index;
        mesh_entity->half_edges[side_up_half_edge_index].vertex = face_vertices[next_edge_index];
        mesh_entity->half_edges[side_up_half_edge_index].face = side_face_index;

        mesh_entity->half_edges[cap_opposite_half_edge_index].next_half_edge = side_down_half_edge_index;
        mesh_entity->half_edges[cap_opposite_half_edge_index].pair_half_edge = cap_half_edge_index;
        mesh_entity->half_edges[cap_opposite_half_edge_index].vertex = next_extruded_vertex_index;
        mesh_entity->half_edges[cap_opposite_half_edge_index].face = side_face_index;

        mesh_entity->half_edges[side_down_half_edge_index].next_half_edge = original_half_edge_index;
        mesh_entity->half_edges[side_down_half_edge_index].pair_half_edge = previous_side_up_half_edge_index;
        mesh_entity->half_edges[side_down_half_edge_index].vertex = extruded_vertex_index;
        mesh_entity->half_edges[side_down_half_edge_index].face = side_face_index;

        pd_core_face_extrude_controller_local_compute_side_normal(
            mesh_entity,
            face_vertices[edge_index],
            face_vertices[next_edge_index],
            next_extruded_vertex_index,
            side_normal);
        pd_core_face_extrude_controller_local_write_ring_face(
            mesh_entity,
            side_face_index,
            original_half_edge_index,
            &source_face,
            side_normal);
    }

    mesh_entity->faces[face_index] = source_face;
    mesh_entity->faces[face_index].half_edge = cap_half_edge_start;
    mesh_entity->vertex_count = old_vertex_count + edge_count;
    mesh_entity->half_edge_count = old_half_edge_count + (edge_count * 4u);
    mesh_entity->face_count = old_face_count + edge_count;

    return pd_core_mesh_validator_check(mesh_entity);
}
