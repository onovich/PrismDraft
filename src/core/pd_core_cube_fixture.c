#include "prismdraft/core/pd_core_cube_fixture.h"

#include "prismdraft/core/pd_core_mesh_storage_controller.h"
#include "prismdraft/core/pd_core_mesh_validator.h"

static void pd_core_cube_fixture_local_set_vertex(
    PdCoreVertexEntity* vertex,
    float position_x,
    float position_y,
    float position_z,
    uint32_t half_edge)
{
    vertex->position[0] = position_x;
    vertex->position[1] = position_y;
    vertex->position[2] = position_z;
    vertex->normal[0] = 0.0f;
    vertex->normal[1] = 0.0f;
    vertex->normal[2] = 0.0f;
    vertex->color[0] = 220u;
    vertex->color[1] = 220u;
    vertex->color[2] = 220u;
    vertex->color[3] = 255u;
    vertex->half_edge = half_edge;
}

static void pd_core_cube_fixture_local_set_half_edge(
    PdCoreHalfEdgeEntity* half_edge,
    uint32_t next_half_edge,
    uint32_t pair_half_edge,
    uint32_t vertex,
    uint32_t face)
{
    half_edge->next_half_edge = next_half_edge;
    half_edge->pair_half_edge = pair_half_edge;
    half_edge->vertex = vertex;
    half_edge->face = face;
}

static void pd_core_cube_fixture_local_set_face(
    PdCoreFaceEntity* face,
    uint32_t half_edge,
    float normal_x,
    float normal_y,
    float normal_z,
    uint8_t color_r,
    uint8_t color_g,
    uint8_t color_b)
{
    face->half_edge = half_edge;
    face->face_normal[0] = normal_x;
    face->face_normal[1] = normal_y;
    face->face_normal[2] = normal_z;
    face->base_color[0] = color_r;
    face->base_color[1] = color_g;
    face->base_color[2] = color_b;
    face->base_color[3] = 255u;
}

PdCoreResult pd_core_cube_fixture_build(PdCoreMeshEntity* mesh_entity)
{
    PdCoreResult result;

    /*
     * Topology proof for the deterministic cube fixture:
     * Input invariants: mesh_entity is valid storage or empty storage owned by the caller.
     * Allocations: reserve at least 8 vertices, 24 half-edges, and 6 quad faces.
     * next_half_edge plan: each face owns four consecutive half-edges wired as a closed quad loop.
     * pair_half_edge plan: every cube edge appears twice with opposite direction; each pair points back to its opposite.
     * Output invariants: counts are exactly 8/24/6 and pd_core_mesh_validator_check must return OK.
     */

    if (mesh_entity == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    result = pd_core_mesh_storage_controller_reserve_vertices(mesh_entity, 8u);
    if (result != PD_CORE_RESULT_OK) {
        return result;
    }

    result = pd_core_mesh_storage_controller_reserve_half_edges(mesh_entity, 24u);
    if (result != PD_CORE_RESULT_OK) {
        return result;
    }

    result = pd_core_mesh_storage_controller_reserve_faces(mesh_entity, 6u);
    if (result != PD_CORE_RESULT_OK) {
        return result;
    }

    mesh_entity->vertex_count = 8u;
    mesh_entity->half_edge_count = 24u;
    mesh_entity->face_count = 6u;

    pd_core_cube_fixture_local_set_vertex(&mesh_entity->vertices[0], -1.0f, -1.0f, -1.0f, 0u);
    pd_core_cube_fixture_local_set_vertex(&mesh_entity->vertices[1], 1.0f, -1.0f, -1.0f, 3u);
    pd_core_cube_fixture_local_set_vertex(&mesh_entity->vertices[2], 1.0f, 1.0f, -1.0f, 2u);
    pd_core_cube_fixture_local_set_vertex(&mesh_entity->vertices[3], -1.0f, 1.0f, -1.0f, 1u);
    pd_core_cube_fixture_local_set_vertex(&mesh_entity->vertices[4], -1.0f, -1.0f, 1.0f, 4u);
    pd_core_cube_fixture_local_set_vertex(&mesh_entity->vertices[5], 1.0f, -1.0f, 1.0f, 5u);
    pd_core_cube_fixture_local_set_vertex(&mesh_entity->vertices[6], 1.0f, 1.0f, 1.0f, 6u);
    pd_core_cube_fixture_local_set_vertex(&mesh_entity->vertices[7], -1.0f, 1.0f, 1.0f, 7u);

    pd_core_cube_fixture_local_set_half_edge(&mesh_entity->half_edges[0], 1u, 11u, 0u, 0u);
    pd_core_cube_fixture_local_set_half_edge(&mesh_entity->half_edges[1], 2u, 23u, 3u, 0u);
    pd_core_cube_fixture_local_set_half_edge(&mesh_entity->half_edges[2], 3u, 12u, 2u, 0u);
    pd_core_cube_fixture_local_set_half_edge(&mesh_entity->half_edges[3], 0u, 16u, 1u, 0u);

    pd_core_cube_fixture_local_set_half_edge(&mesh_entity->half_edges[4], 5u, 18u, 4u, 1u);
    pd_core_cube_fixture_local_set_half_edge(&mesh_entity->half_edges[5], 6u, 14u, 5u, 1u);
    pd_core_cube_fixture_local_set_half_edge(&mesh_entity->half_edges[6], 7u, 21u, 6u, 1u);
    pd_core_cube_fixture_local_set_half_edge(&mesh_entity->half_edges[7], 4u, 9u, 7u, 1u);

    pd_core_cube_fixture_local_set_half_edge(&mesh_entity->half_edges[8], 9u, 19u, 0u, 2u);
    pd_core_cube_fixture_local_set_half_edge(&mesh_entity->half_edges[9], 10u, 7u, 4u, 2u);
    pd_core_cube_fixture_local_set_half_edge(&mesh_entity->half_edges[10], 11u, 20u, 7u, 2u);
    pd_core_cube_fixture_local_set_half_edge(&mesh_entity->half_edges[11], 8u, 0u, 3u, 2u);

    pd_core_cube_fixture_local_set_half_edge(&mesh_entity->half_edges[12], 13u, 2u, 1u, 3u);
    pd_core_cube_fixture_local_set_half_edge(&mesh_entity->half_edges[13], 14u, 22u, 2u, 3u);
    pd_core_cube_fixture_local_set_half_edge(&mesh_entity->half_edges[14], 15u, 5u, 6u, 3u);
    pd_core_cube_fixture_local_set_half_edge(&mesh_entity->half_edges[15], 12u, 17u, 5u, 3u);

    pd_core_cube_fixture_local_set_half_edge(&mesh_entity->half_edges[16], 17u, 3u, 0u, 4u);
    pd_core_cube_fixture_local_set_half_edge(&mesh_entity->half_edges[17], 18u, 15u, 1u, 4u);
    pd_core_cube_fixture_local_set_half_edge(&mesh_entity->half_edges[18], 19u, 4u, 5u, 4u);
    pd_core_cube_fixture_local_set_half_edge(&mesh_entity->half_edges[19], 16u, 8u, 4u, 4u);

    pd_core_cube_fixture_local_set_half_edge(&mesh_entity->half_edges[20], 21u, 10u, 3u, 5u);
    pd_core_cube_fixture_local_set_half_edge(&mesh_entity->half_edges[21], 22u, 6u, 7u, 5u);
    pd_core_cube_fixture_local_set_half_edge(&mesh_entity->half_edges[22], 23u, 13u, 6u, 5u);
    pd_core_cube_fixture_local_set_half_edge(&mesh_entity->half_edges[23], 20u, 1u, 2u, 5u);

    pd_core_cube_fixture_local_set_face(&mesh_entity->faces[0], 0u, 0.0f, 0.0f, -1.0f, 190u, 190u, 210u);
    pd_core_cube_fixture_local_set_face(&mesh_entity->faces[1], 4u, 0.0f, 0.0f, 1.0f, 230u, 230u, 245u);
    pd_core_cube_fixture_local_set_face(&mesh_entity->faces[2], 8u, -1.0f, 0.0f, 0.0f, 180u, 180u, 205u);
    pd_core_cube_fixture_local_set_face(&mesh_entity->faces[3], 12u, 1.0f, 0.0f, 0.0f, 215u, 215u, 235u);
    pd_core_cube_fixture_local_set_face(&mesh_entity->faces[4], 16u, 0.0f, -1.0f, 0.0f, 160u, 160u, 185u);
    pd_core_cube_fixture_local_set_face(&mesh_entity->faces[5], 20u, 0.0f, 1.0f, 0.0f, 240u, 240u, 250u);

    return pd_core_mesh_validator_check(mesh_entity);
}
