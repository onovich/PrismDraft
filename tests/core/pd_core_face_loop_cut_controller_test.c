#include "prismdraft/core/pd_core_face_loop_cut_controller.h"

#include "prismdraft/core/pd_core_cube_fixture.h"
#include "prismdraft/core/pd_core_mesh_storage_controller.h"
#include "prismdraft/core/pd_core_mesh_validator.h"

#include <assert.h>

static void pd_core_face_loop_cut_controller_test_local_cuts_cube_quad_face(void)
{
    PdCoreMeshEntity mesh_entity = { 0 };

    assert(pd_core_cube_fixture_build(&mesh_entity) == PD_CORE_RESULT_OK);
    assert(pd_core_face_loop_cut_controller_apply_quad(&mesh_entity, 1u) == PD_CORE_RESULT_OK);

    assert(mesh_entity.vertex_count == 8u);
    assert(mesh_entity.half_edge_count == 26u);
    assert(mesh_entity.face_count == 7u);
    assert(mesh_entity.faces[1].half_edge == 4u);
    assert(mesh_entity.faces[6].half_edge == 6u);
    assert(mesh_entity.half_edges[24].pair_half_edge == 25u);
    assert(mesh_entity.half_edges[25].pair_half_edge == 24u);
    assert(mesh_entity.half_edges[24].vertex == 4u);
    assert(mesh_entity.half_edges[25].vertex == 6u);
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
