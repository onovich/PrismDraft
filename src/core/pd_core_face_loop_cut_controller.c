#include "prismdraft/core/pd_core_face_loop_cut_controller.h"

#include "prismdraft/core/pd_core_mesh_storage_controller.h"
#include "prismdraft/core/pd_core_mesh_validator.h"

static PdCoreResult pd_core_face_loop_cut_controller_local_collect_quad(
    const PdCoreMeshEntity* mesh_entity,
    uint32_t face_index,
    uint32_t half_edges[4])
{
    uint32_t start_half_edge_index;
    uint32_t current_half_edge_index;
    uint32_t edge_count = 0u;

    if (mesh_entity == 0 || half_edges == 0 || face_index >= mesh_entity->face_count) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    start_half_edge_index = mesh_entity->faces[face_index].half_edge;
    current_half_edge_index = start_half_edge_index;

    do {
        if (edge_count >= 4u) {
            return PD_CORE_RESULT_ERROR_TOPOLOGY_INVALID;
        }

        half_edges[edge_count] = current_half_edge_index;
        edge_count++;
        current_half_edge_index = mesh_entity->half_edges[current_half_edge_index].next_half_edge;
    } while (current_half_edge_index != start_half_edge_index);

    if (edge_count != 4u) {
        return PD_CORE_RESULT_ERROR_TOPOLOGY_INVALID;
    }

    return PD_CORE_RESULT_OK;
}

static void pd_core_face_loop_cut_controller_local_write_midpoint_vertex(
    PdCoreMeshEntity* mesh_entity,
    uint32_t vertex_index,
    uint32_t start_vertex_index,
    uint32_t end_vertex_index,
    uint32_t half_edge_index,
    const PdCoreFaceEntity* source_face)
{
    PdCoreVertexEntity* vertex = &mesh_entity->vertices[vertex_index];
    const PdCoreVertexEntity* start_vertex = &mesh_entity->vertices[start_vertex_index];
    const PdCoreVertexEntity* end_vertex = &mesh_entity->vertices[end_vertex_index];

    vertex->position[0] = (start_vertex->position[0] + end_vertex->position[0]) * 0.5f;
    vertex->position[1] = (start_vertex->position[1] + end_vertex->position[1]) * 0.5f;
    vertex->position[2] = (start_vertex->position[2] + end_vertex->position[2]) * 0.5f;
    vertex->normal[0] = source_face->face_normal[0];
    vertex->normal[1] = source_face->face_normal[1];
    vertex->normal[2] = source_face->face_normal[2];
    vertex->color[0] = source_face->base_color[0];
    vertex->color[1] = source_face->base_color[1];
    vertex->color[2] = source_face->base_color[2];
    vertex->color[3] = source_face->base_color[3];
    vertex->half_edge = half_edge_index;
}

PdCoreResult pd_core_face_loop_cut_controller_apply_quad(PdCoreMeshEntity* mesh_entity, uint32_t face_index)
{
    uint32_t half_edges[4];
    uint32_t old_vertex_count;
    uint32_t old_half_edge_count;
    uint32_t old_face_count;
    uint32_t first_midpoint_vertex_index;
    uint32_t second_midpoint_vertex_index;
    uint32_t first_remainder_half_edge_index;
    uint32_t second_remainder_half_edge_index;
    uint32_t cut_forward_half_edge_index;
    uint32_t cut_back_half_edge_index;
    uint32_t cut_face_index;
    PdCoreFaceEntity source_face;
    PdCoreResult result;

    /*
     * Topology proof for constrained quad loop cut:
     * Input invariants: mesh_entity validates and face_index is a closed quad. Allocated additions: two midpoint
     * vertices on opposite boundary edges, two boundary remainder half-edges, two internal cut half-edges, and one
     * additional face. next_half_edge rewiring plan: the selected quad becomes two closed quads sharing the inserted
     * internal cut. pair_half_edge rewiring plan: the two internal cut half-edges pair with each other; existing
     * boundary pairs remain attached to their original half-edges. Output invariants: both resulting faces remain
     * quads, the visible feedback is a midline cut rather than a diagonal triangulation, and validation must return OK.
     */

    if (mesh_entity == 0 || face_index == PD_CORE_MESH_ENTITY_INVALID_INDEX) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    result = pd_core_mesh_validator_check(mesh_entity);
    if (result != PD_CORE_RESULT_OK) {
        return result;
    }

    result = pd_core_face_loop_cut_controller_local_collect_quad(mesh_entity, face_index, half_edges);
    if (result != PD_CORE_RESULT_OK) {
        return result;
    }

    old_vertex_count = mesh_entity->vertex_count;
    old_half_edge_count = mesh_entity->half_edge_count;
    old_face_count = mesh_entity->face_count;

    result = pd_core_mesh_storage_controller_reserve_vertices(mesh_entity, old_vertex_count + 2u);
    if (result != PD_CORE_RESULT_OK) {
        return result;
    }

    result = pd_core_mesh_storage_controller_reserve_half_edges(mesh_entity, old_half_edge_count + 4u);
    if (result != PD_CORE_RESULT_OK) {
        return result;
    }

    result = pd_core_mesh_storage_controller_reserve_faces(mesh_entity, old_face_count + 1u);
    if (result != PD_CORE_RESULT_OK) {
        return result;
    }

    source_face = mesh_entity->faces[face_index];
    first_midpoint_vertex_index = old_vertex_count;
    second_midpoint_vertex_index = old_vertex_count + 1u;
    first_remainder_half_edge_index = old_half_edge_count;
    second_remainder_half_edge_index = old_half_edge_count + 1u;
    cut_forward_half_edge_index = old_half_edge_count + 2u;
    cut_back_half_edge_index = old_half_edge_count + 3u;
    cut_face_index = old_face_count;

    pd_core_face_loop_cut_controller_local_write_midpoint_vertex(
        mesh_entity,
        first_midpoint_vertex_index,
        mesh_entity->half_edges[half_edges[1]].vertex,
        mesh_entity->half_edges[half_edges[2]].vertex,
        first_remainder_half_edge_index,
        &source_face);
    pd_core_face_loop_cut_controller_local_write_midpoint_vertex(
        mesh_entity,
        second_midpoint_vertex_index,
        mesh_entity->half_edges[half_edges[3]].vertex,
        mesh_entity->half_edges[half_edges[0]].vertex,
        second_remainder_half_edge_index,
        &source_face);

    mesh_entity->half_edges[first_remainder_half_edge_index].next_half_edge = half_edges[2];
    mesh_entity->half_edges[first_remainder_half_edge_index].pair_half_edge = PD_CORE_MESH_ENTITY_INVALID_INDEX;
    mesh_entity->half_edges[first_remainder_half_edge_index].vertex = first_midpoint_vertex_index;
    mesh_entity->half_edges[first_remainder_half_edge_index].face = cut_face_index;

    mesh_entity->half_edges[second_remainder_half_edge_index].next_half_edge = half_edges[0];
    mesh_entity->half_edges[second_remainder_half_edge_index].pair_half_edge = PD_CORE_MESH_ENTITY_INVALID_INDEX;
    mesh_entity->half_edges[second_remainder_half_edge_index].vertex = second_midpoint_vertex_index;
    mesh_entity->half_edges[second_remainder_half_edge_index].face = face_index;

    mesh_entity->half_edges[cut_forward_half_edge_index].next_half_edge = second_remainder_half_edge_index;
    mesh_entity->half_edges[cut_forward_half_edge_index].pair_half_edge = cut_back_half_edge_index;
    mesh_entity->half_edges[cut_forward_half_edge_index].vertex = first_midpoint_vertex_index;
    mesh_entity->half_edges[cut_forward_half_edge_index].face = face_index;

    mesh_entity->half_edges[cut_back_half_edge_index].next_half_edge = first_remainder_half_edge_index;
    mesh_entity->half_edges[cut_back_half_edge_index].pair_half_edge = cut_forward_half_edge_index;
    mesh_entity->half_edges[cut_back_half_edge_index].vertex = second_midpoint_vertex_index;
    mesh_entity->half_edges[cut_back_half_edge_index].face = cut_face_index;

    mesh_entity->half_edges[half_edges[0]].next_half_edge = half_edges[1];
    mesh_entity->half_edges[half_edges[0]].face = face_index;
    mesh_entity->half_edges[half_edges[1]].next_half_edge = cut_forward_half_edge_index;
    mesh_entity->half_edges[half_edges[1]].face = face_index;

    mesh_entity->half_edges[half_edges[2]].next_half_edge = half_edges[3];
    mesh_entity->half_edges[half_edges[2]].face = cut_face_index;
    mesh_entity->half_edges[half_edges[3]].next_half_edge = cut_back_half_edge_index;
    mesh_entity->half_edges[half_edges[3]].face = cut_face_index;

    mesh_entity->faces[face_index] = source_face;
    mesh_entity->faces[face_index].half_edge = half_edges[0];
    mesh_entity->faces[cut_face_index] = source_face;
    mesh_entity->faces[cut_face_index].half_edge = first_remainder_half_edge_index;
    mesh_entity->vertex_count = old_vertex_count + 2u;
    mesh_entity->half_edge_count = old_half_edge_count + 4u;
    mesh_entity->face_count = old_face_count + 1u;

    return pd_core_mesh_validator_check(mesh_entity);
}
