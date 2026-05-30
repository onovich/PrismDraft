#include "prismdraft/editor/pd_editor_pick_service.h"

#include "prismdraft/core/pd_core_cube_fixture.h"
#include "prismdraft/core/pd_core_mesh_storage_controller.h"

#include <assert.h>
#include <math.h>

static int pd_editor_pick_service_test_local_near(float left, float right)
{
    return fabsf(left - right) < 0.0001f;
}

static void pd_editor_pick_service_test_local_hits_front_cube_face(void)
{
    PdCoreMeshEntity mesh_entity = { 0 };
    PdEditorPickServiceHit hit;
    float ray_origin[3] = { 0.0f, 0.0f, 5.0f };
    float ray_direction[3] = { 0.0f, 0.0f, -1.0f };

    assert(pd_core_cube_fixture_build(&mesh_entity) == PD_CORE_RESULT_OK);
    assert(pd_editor_pick_service_pick_face(&mesh_entity, ray_origin, ray_direction, &hit) == PD_CORE_RESULT_OK);
    assert(hit.has_hit);
    assert(hit.face_index == 1u);
    assert(pd_editor_pick_service_test_local_near(hit.distance, 4.0f));
    assert(pd_editor_pick_service_test_local_near(hit.position[0], 0.0f));
    assert(pd_editor_pick_service_test_local_near(hit.position[1], 0.0f));
    assert(pd_editor_pick_service_test_local_near(hit.position[2], 1.0f));

    pd_core_mesh_storage_controller_free(&mesh_entity);
}

static void pd_editor_pick_service_test_local_misses_cube(void)
{
    PdCoreMeshEntity mesh_entity = { 0 };
    PdEditorPickServiceHit hit;
    float ray_origin[3] = { 3.0f, 3.0f, 5.0f };
    float ray_direction[3] = { 0.0f, 0.0f, -1.0f };

    assert(pd_core_cube_fixture_build(&mesh_entity) == PD_CORE_RESULT_OK);
    assert(pd_editor_pick_service_pick_face(&mesh_entity, ray_origin, ray_direction, &hit) == PD_CORE_RESULT_OK);
    assert(!hit.has_hit);
    assert(hit.face_index == PD_CORE_MESH_ENTITY_INVALID_INDEX);

    pd_core_mesh_storage_controller_free(&mesh_entity);
}

static void pd_editor_pick_service_test_local_rejects_null_arguments(void)
{
    PdCoreMeshEntity mesh_entity = { 0 };
    PdEditorPickServiceHit hit;
    float ray_origin[3] = { 0.0f, 0.0f, 5.0f };
    float ray_direction[3] = { 0.0f, 0.0f, -1.0f };

    assert(pd_editor_pick_service_pick_face(0, ray_origin, ray_direction, &hit) ==
           PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);
    assert(pd_editor_pick_service_pick_face(&mesh_entity, 0, ray_direction, &hit) ==
           PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);
    assert(pd_editor_pick_service_pick_face(&mesh_entity, ray_origin, 0, &hit) ==
           PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);
    assert(pd_editor_pick_service_pick_face(&mesh_entity, ray_origin, ray_direction, 0) ==
           PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);
}

int main(void)
{
    pd_editor_pick_service_test_local_hits_front_cube_face();
    pd_editor_pick_service_test_local_misses_cube();
    pd_editor_pick_service_test_local_rejects_null_arguments();
    return 0;
}
