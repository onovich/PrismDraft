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

PdCoreResult pd_core_face_loop_cut_controller_apply_quad(PdCoreMeshEntity* mesh_entity, uint32_t face_index)
{
    uint32_t half_edges[4];
    uint32_t old_half_edge_count;
    uint32_t old_face_count;
    uint32_t cut_forward_half_edge_index;
    uint32_t cut_back_half_edge_index;
    uint32_t cut_face_index;
    PdCoreFaceEntity source_face;
    PdCoreResult result;

    /*
     * Topology proof for constrained quad loop cut:
     * Input invariants: mesh_entity validates and face_index is a closed quad. Allocated additions: two internal
     * half-edges and one additional face. next_half_edge rewiring plan: the first triangle reuses boundary half-edges
     * 0 and 1 plus the backward cut; the second triangle reuses boundary half-edges 2 and 3 plus the forward cut.
     * pair_half_edge rewiring plan: existing boundary pairs are preserved; the two cut half-edges pair with each other.
     * Output invariants: both resulting faces are closed, the inserted cut has symmetric pairs, and
     * pd_core_mesh_validator_check must return OK.
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

    old_half_edge_count = mesh_entity->half_edge_count;
    old_face_count = mesh_entity->face_count;

    result = pd_core_mesh_storage_controller_reserve_half_edges(mesh_entity, old_half_edge_count + 2u);
    if (result != PD_CORE_RESULT_OK) {
        return result;
    }

    result = pd_core_mesh_storage_controller_reserve_faces(mesh_entity, old_face_count + 1u);
    if (result != PD_CORE_RESULT_OK) {
        return result;
    }

    source_face = mesh_entity->faces[face_index];
    cut_forward_half_edge_index = old_half_edge_count;
    cut_back_half_edge_index = old_half_edge_count + 1u;
    cut_face_index = old_face_count;

    mesh_entity->half_edges[cut_forward_half_edge_index].next_half_edge = half_edges[2];
    mesh_entity->half_edges[cut_forward_half_edge_index].pair_half_edge = cut_back_half_edge_index;
    mesh_entity->half_edges[cut_forward_half_edge_index].vertex = mesh_entity->half_edges[half_edges[0]].vertex;
    mesh_entity->half_edges[cut_forward_half_edge_index].face = cut_face_index;

    mesh_entity->half_edges[cut_back_half_edge_index].next_half_edge = half_edges[0];
    mesh_entity->half_edges[cut_back_half_edge_index].pair_half_edge = cut_forward_half_edge_index;
    mesh_entity->half_edges[cut_back_half_edge_index].vertex = mesh_entity->half_edges[half_edges[2]].vertex;
    mesh_entity->half_edges[cut_back_half_edge_index].face = face_index;

    mesh_entity->half_edges[half_edges[0]].next_half_edge = half_edges[1];
    mesh_entity->half_edges[half_edges[0]].face = face_index;
    mesh_entity->half_edges[half_edges[1]].next_half_edge = cut_back_half_edge_index;
    mesh_entity->half_edges[half_edges[1]].face = face_index;

    mesh_entity->half_edges[half_edges[2]].next_half_edge = half_edges[3];
    mesh_entity->half_edges[half_edges[2]].face = cut_face_index;
    mesh_entity->half_edges[half_edges[3]].next_half_edge = cut_forward_half_edge_index;
    mesh_entity->half_edges[half_edges[3]].face = cut_face_index;

    mesh_entity->faces[face_index] = source_face;
    mesh_entity->faces[face_index].half_edge = half_edges[0];
    mesh_entity->faces[cut_face_index] = source_face;
    mesh_entity->faces[cut_face_index].half_edge = half_edges[2];
    mesh_entity->half_edge_count = old_half_edge_count + 2u;
    mesh_entity->face_count = old_face_count + 1u;

    return pd_core_mesh_validator_check(mesh_entity);
}
