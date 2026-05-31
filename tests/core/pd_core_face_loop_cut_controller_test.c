#include "prismdraft/core/pd_core_face_loop_cut_controller.h"

#include "prismdraft/core/pd_core_cube_fixture.h"
#include "prismdraft/core/pd_core_mesh_storage_controller.h"
#include "prismdraft/core/pd_core_mesh_validator.h"

#include <assert.h>

static uint32_t pd_core_face_loop_cut_controller_test_local_count_face_edges(
    const PdCoreMeshEntity* mesh_entity,
    uint32_t face_index)
{
    uint32_t start_half_edge_index = mesh_entity->faces[face_index].half_edge;
    uint32_t current_half_edge_index = start_half_edge_index;
    uint32_t edge_count = 0u;

    do {
        edge_count++;
        current_half_edge_index = mesh_entity->half_edges[current_half_edge_index].next_half_edge;
    } while (current_half_edge_index != start_half_edge_index);

    return edge_count;
}

static int pd_core_face_loop_cut_controller_test_local_near(float left, float right)
{
    float difference = left - right;
    if (difference < 0.0f) {
        difference = -difference;
    }

    return difference < 0.0001f;
}

static void pd_core_face_loop_cut_controller_test_local_cuts_cube_quad_face(void)
{
    PdCoreMeshEntity mesh_entity = { 0 };

    assert(pd_core_cube_fixture_build(&mesh_entity) == PD_CORE_RESULT_OK);
    assert(pd_core_face_loop_cut_controller_apply_quad(&mesh_entity, 1u) == PD_CORE_RESULT_OK);

    assert(mesh_entity.vertex_count == 10u);
    assert(mesh_entity.half_edge_count == 28u);
    assert(mesh_entity.face_count == 7u);
    assert(mesh_entity.faces[1].half_edge == 4u);
    assert(mesh_entity.faces[6].half_edge == 24u);
    assert(pd_core_face_loop_cut_controller_test_local_count_face_edges(&mesh_entity, 1u) == 4u);
    assert(pd_core_face_loop_cut_controller_test_local_count_face_edges(&mesh_entity, 6u) == 4u);
    assert(mesh_entity.half_edges[26].pair_half_edge == 27u);
    assert(mesh_entity.half_edges[27].pair_half_edge == 26u);
    assert(mesh_entity.half_edges[26].vertex == 8u);
    assert(mesh_entity.half_edges[27].vertex == 9u);
    assert(pd_core_face_loop_cut_controller_test_local_near(mesh_entity.vertices[8].position[0], 1.0f));
    assert(pd_core_face_loop_cut_controller_test_local_near(mesh_entity.vertices[8].position[1], 0.0f));
    assert(pd_core_face_loop_cut_controller_test_local_near(mesh_entity.vertices[8].position[2], 1.0f));
    assert(pd_core_face_loop_cut_controller_test_local_near(mesh_entity.vertices[9].position[0], -1.0f));
    assert(pd_core_face_loop_cut_controller_test_local_near(mesh_entity.vertices[9].position[1], 0.0f));
    assert(pd_core_face_loop_cut_controller_test_local_near(mesh_entity.vertices[9].position[2], 1.0f));
    assert(pd_core_mesh_validator_check(&mesh_entity) == PD_CORE_RESULT_OK);

    pd_core_mesh_storage_controller_free(&mesh_entity);
}

static void pd_core_face_loop_cut_controller_test_local_rejects_invalid_arguments(void)
{
    PdCoreMeshEntity mesh_entity = { 0 };

    assert(pd_core_cube_fixture_build(&mesh_entity) == PD_CORE_RESULT_OK);
    assert(pd_core_face_loop_cut_controller_apply_quad(0, 1u) == PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);
    assert(pd_core_face_loop_cut_controller_apply_quad(&mesh_entity, PD_CORE_MESH_ENTITY_INVALID_INDEX) ==
           PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);

    pd_core_mesh_storage_controller_free(&mesh_entity);
}

int main(void)
{
    pd_core_face_loop_cut_controller_test_local_cuts_cube_quad_face();
    pd_core_face_loop_cut_controller_test_local_rejects_invalid_arguments();
    return 0;
}
