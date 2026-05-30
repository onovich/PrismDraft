#include "prismdraft/core/pd_core_cube_fixture.h"
#include "prismdraft/core/pd_core_mesh_storage_controller.h"
#include "prismdraft/core/pd_core_mesh_validator.h"

#include <assert.h>

static void pd_core_cube_fixture_test_local_builds_valid_cube_counts(void)
{
    PdCoreMeshEntity mesh_entity = { 0 };

    assert(pd_core_cube_fixture_build(&mesh_entity) == PD_CORE_RESULT_OK);
    assert(mesh_entity.vertex_count == 8u);
    assert(mesh_entity.half_edge_count == 24u);
    assert(mesh_entity.face_count == 6u);
    assert(pd_core_mesh_validator_check(&mesh_entity) == PD_CORE_RESULT_OK);

    pd_core_mesh_storage_controller_free(&mesh_entity);
}

static void pd_core_cube_fixture_test_local_pairs_are_symmetric(void)
{
    uint32_t half_edge_index;
    PdCoreMeshEntity mesh_entity = { 0 };

    assert(pd_core_cube_fixture_build(&mesh_entity) == PD_CORE_RESULT_OK);

    for (half_edge_index = 0u; half_edge_index < mesh_entity.half_edge_count; half_edge_index++) {
        uint32_t pair_index = mesh_entity.half_edges[half_edge_index].pair_half_edge;
        assert(pair_index < mesh_entity.half_edge_count);
        assert(mesh_entity.half_edges[pair_index].pair_half_edge == half_edge_index);
    }

    pd_core_mesh_storage_controller_free(&mesh_entity);
}

static void pd_core_cube_fixture_test_local_faces_are_quads(void)
{
    uint32_t face_index;
    PdCoreMeshEntity mesh_entity = { 0 };

    assert(pd_core_cube_fixture_build(&mesh_entity) == PD_CORE_RESULT_OK);

    for (face_index = 0u; face_index < mesh_entity.face_count; face_index++) {
        uint32_t start_half_edge_index = mesh_entity.faces[face_index].half_edge;
        uint32_t current_half_edge_index = start_half_edge_index;
        uint32_t step_count = 0u;

        do {
            current_half_edge_index = mesh_entity.half_edges[current_half_edge_index].next_half_edge;
            step_count++;
        } while (current_half_edge_index != start_half_edge_index);

        assert(step_count == 4u);
    }

    pd_core_mesh_storage_controller_free(&mesh_entity);
}

static void pd_core_cube_fixture_test_local_rejects_null_mesh(void)
{
    assert(pd_core_cube_fixture_build(NULL) == PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);
}

int main(void)
{
    pd_core_cube_fixture_test_local_builds_valid_cube_counts();
    pd_core_cube_fixture_test_local_pairs_are_symmetric();
    pd_core_cube_fixture_test_local_faces_are_quads();
    pd_core_cube_fixture_test_local_rejects_null_mesh();
    return 0;
}
