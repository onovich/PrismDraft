#include "prismdraft/core/pd_core_face_extrude_controller.h"

#include "prismdraft/core/pd_core_cube_fixture.h"
#include "prismdraft/core/pd_core_mesh_storage_controller.h"
#include "prismdraft/core/pd_core_mesh_validator.h"

#include <assert.h>

static int pd_core_face_extrude_controller_test_local_near(float left, float right)
{
    float difference = left - right;
    if (difference < 0.0f) {
        difference = -difference;
    }

    return difference < 0.0001f;
}

static void pd_core_face_extrude_controller_test_local_extrudes_cube_front_face(void)
{
    PdCoreMeshEntity mesh_entity = { 0 };

    assert(pd_core_cube_fixture_build(&mesh_entity) == PD_CORE_RESULT_OK);
    assert(pd_core_face_extrude_controller_apply(&mesh_entity, 1u, 0.5f) == PD_CORE_RESULT_OK);

    assert(mesh_entity.vertex_count == 12u);
    assert(mesh_entity.half_edge_count == 40u);
    assert(mesh_entity.face_count == 10u);
    assert(mesh_entity.faces[1].half_edge == 24u);
    assert(mesh_entity.vertices[8].half_edge == 24u);
    assert(pd_core_face_extrude_controller_test_local_near(mesh_entity.vertices[8].position[0], -1.0f));
    assert(pd_core_face_extrude_controller_test_local_near(mesh_entity.vertices[8].position[1], -1.0f));
    assert(pd_core_face_extrude_controller_test_local_near(mesh_entity.vertices[8].position[2], 1.5f));
    assert(pd_core_face_extrude_controller_test_local_near(mesh_entity.faces[6].face_normal[1], -1.0f));
    assert(pd_core_mesh_validator_check(&mesh_entity) == PD_CORE_RESULT_OK);

    pd_core_mesh_storage_controller_free(&mesh_entity);
}

static void pd_core_face_extrude_controller_test_local_rejects_invalid_arguments(void)
{
    PdCoreMeshEntity mesh_entity = { 0 };

    assert(pd_core_cube_fixture_build(&mesh_entity) == PD_CORE_RESULT_OK);
    assert(pd_core_face_extrude_controller_apply(0, 1u, 0.5f) == PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);
    assert(pd_core_face_extrude_controller_apply(&mesh_entity, PD_CORE_MESH_ENTITY_INVALID_INDEX, 0.5f) ==
           PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);
    assert(pd_core_face_extrude_controller_apply(&mesh_entity, 1u, 0.0f) ==
           PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);

    pd_core_mesh_storage_controller_free(&mesh_entity);
}

int main(void)
{
    pd_core_face_extrude_controller_test_local_extrudes_cube_front_face();
    pd_core_face_extrude_controller_test_local_rejects_invalid_arguments();
    return 0;
}
