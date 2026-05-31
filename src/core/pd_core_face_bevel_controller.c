#include "prismdraft/core/pd_core_face_bevel_controller.h"

#include "prismdraft/core/pd_core_face_inset_controller.h"
#include "prismdraft/core/pd_core_mesh_validator.h"

#include <math.h>

static PdCoreResult pd_core_face_bevel_controller_local_collect_face(
    const PdCoreMeshEntity* mesh_entity,
    uint32_t face_index,
    uint32_t vertices[PD_CORE_FACE_INSET_CONTROLLER_MAX_FACE_EDGES],
    uint32_t* edge_count)
{
    uint32_t start_half_edge_index;
    uint32_t current_half_edge_index;
    uint32_t count = 0u;

    if (mesh_entity == 0 || vertices == 0 || edge_count == 0 || face_index >= mesh_entity->face_count) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    start_half_edge_index = mesh_entity->faces[face_index].half_edge;
    current_half_edge_index = start_half_edge_index;

    do {
        if (count >= PD_CORE_FACE_INSET_CONTROLLER_MAX_FACE_EDGES) {
            return PD_CORE_RESULT_ERROR_TOPOLOGY_INVALID;
        }

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

static void pd_core_face_bevel_controller_local_normalize(float value[3])
{
    float length = sqrtf((value[0] * value[0]) + (value[1] * value[1]) + (value[2] * value[2]));

    if (length <= 0.000001f) {
        return;
    }

    value[0] /= length;
    value[1] /= length;
    value[2] /= length;
}

static void pd_core_face_bevel_controller_local_compute_center(
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

static void pd_core_face_bevel_controller_local_move_inner_face(
    PdCoreMeshEntity* mesh_entity,
    uint32_t face_index,
    const PdCoreFaceEntity* source_face,
    float distance)
{
    uint32_t start_half_edge_index = mesh_entity->faces[face_index].half_edge;
    uint32_t current_half_edge_index = start_half_edge_index;

    do {
        PdCoreVertexEntity* vertex = &mesh_entity->vertices[mesh_entity->half_edges[current_half_edge_index].vertex];
        vertex->position[0] -= source_face->face_normal[0] * distance;
        vertex->position[1] -= source_face->face_normal[1] * distance;
        vertex->position[2] -= source_face->face_normal[2] * distance;
        current_half_edge_index = mesh_entity->half_edges[current_half_edge_index].next_half_edge;
    } while (current_half_edge_index != start_half_edge_index);
}

static void pd_core_face_bevel_controller_local_write_ring_normal(
    PdCoreMeshEntity* mesh_entity,
    uint32_t ring_face_index,
    const uint32_t source_vertices[PD_CORE_FACE_INSET_CONTROLLER_MAX_FACE_EDGES],
    uint32_t edge_index,
    uint32_t edge_count,
    const float center[3],
    const PdCoreFaceEntity* source_face)
{
    uint32_t next_edge_index = (edge_index + 1u) % edge_count;
    const PdCoreVertexEntity* start_vertex = &mesh_entity->vertices[source_vertices[edge_index]];
    const PdCoreVertexEntity* next_vertex = &mesh_entity->vertices[source_vertices[next_edge_index]];
    float outward[3];
    float bevel_normal[3];

    outward[0] = ((start_vertex->position[0] + next_vertex->position[0]) * 0.5f) - center[0];
    outward[1] = ((start_vertex->position[1] + next_vertex->position[1]) * 0.5f) - center[1];
    outward[2] = ((start_vertex->position[2] + next_vertex->position[2]) * 0.5f) - center[2];
    pd_core_face_bevel_controller_local_normalize(outward);

    bevel_normal[0] = source_face->face_normal[0] + outward[0];
    bevel_normal[1] = source_face->face_normal[1] + outward[1];
    bevel_normal[2] = source_face->face_normal[2] + outward[2];
    pd_core_face_bevel_controller_local_normalize(bevel_normal);

    mesh_entity->faces[ring_face_index].face_normal[0] = bevel_normal[0];
    mesh_entity->faces[ring_face_index].face_normal[1] = bevel_normal[1];
    mesh_entity->faces[ring_face_index].face_normal[2] = bevel_normal[2];
}

PdCoreResult pd_core_face_bevel_controller_apply(
    PdCoreMeshEntity* mesh_entity,
    uint32_t face_index,
    float inset_ratio,
    float distance)
{
    uint32_t source_vertices[PD_CORE_FACE_INSET_CONTROLLER_MAX_FACE_EDGES];
    uint32_t edge_count = 0u;
    uint32_t ring_face_start;
    uint32_t edge_index;
    float center[3];
    PdCoreFaceEntity source_face;
    PdCoreResult result;

    /*
     * Topology proof for face bevel:
     * Input invariants: mesh_entity validates, face_index is a closed face accepted by the inset controller,
     * inset_ratio is within (0, 1), and distance is positive. Allocated additions: the inset step creates an inner face
     * and a ring of boundary quads. Geometry plan: the selected inner face is moved inward along the original face
     * normal, and the generated ring faces receive diagonal normals derived from face normal plus edge-outward
     * direction. Output invariants: bevel feedback is an inward chamfer around the original face, not an outward
     * extrusion, ring faces stay closed, and pd_core_mesh_validator_check must return OK.
     */

    if (mesh_entity == 0 || face_index == PD_CORE_MESH_ENTITY_INVALID_INDEX || inset_ratio <= 0.0f ||
        inset_ratio >= 1.0f || distance <= 0.0f) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    result = pd_core_mesh_validator_check(mesh_entity);
    if (result != PD_CORE_RESULT_OK) {
        return result;
    }

    result = pd_core_face_bevel_controller_local_collect_face(mesh_entity, face_index, source_vertices, &edge_count);
    if (result != PD_CORE_RESULT_OK) {
        return result;
    }

    source_face = mesh_entity->faces[face_index];
    ring_face_start = mesh_entity->face_count;
    pd_core_face_bevel_controller_local_compute_center(mesh_entity, source_vertices, edge_count, center);

    result = pd_core_face_inset_controller_apply(mesh_entity, face_index, inset_ratio);
    if (result != PD_CORE_RESULT_OK) {
        return result;
    }

    pd_core_face_bevel_controller_local_move_inner_face(mesh_entity, face_index, &source_face, distance);
    for (edge_index = 0u; edge_index < edge_count; edge_index++) {
        pd_core_face_bevel_controller_local_write_ring_normal(
            mesh_entity,
            ring_face_start + edge_index,
            source_vertices,
            edge_index,
            edge_count,
            center,
            &source_face);
    }

    return pd_core_mesh_validator_check(mesh_entity);
}
