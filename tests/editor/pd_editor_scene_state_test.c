#include "prismdraft/editor/pd_editor_scene_state.h"

#include "prismdraft/core/pd_core_mesh_validator.h"

#include <assert.h>

static void pd_editor_scene_state_test_local_initializes_with_one_cube(void)
{
    PdEditorSceneState scene_state;
    PdEditorSceneObjectEntity* active_object;

    assert(pd_editor_scene_state_init(0) == PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);
    assert(pd_editor_scene_state_init(&scene_state) == PD_CORE_RESULT_OK);
    assert(scene_state.object_count == 1u);
    assert(scene_state.active_object_index == 0u);
    active_object = pd_editor_scene_state_get_active(&scene_state);
    assert(active_object != 0);
    assert(active_object->object_id == 1u);
    assert(active_object->mesh.vertex_count == 8u);
    assert(active_object->transform_state.position[0] == 0.0f);
    assert(pd_core_mesh_validator_check(&active_object->mesh) == PD_CORE_RESULT_OK);

    pd_editor_scene_state_free(&scene_state);
}

static void pd_editor_scene_state_test_local_creates_independent_cubes(void)
{
    PdEditorSceneState scene_state;
    PdEditorSceneObjectEntity* first_object;
    PdEditorSceneObjectEntity* second_object;
    uint32_t object_index = 0u;

    assert(pd_editor_scene_state_init(&scene_state) == PD_CORE_RESULT_OK);
    first_object = pd_editor_scene_state_get_active(&scene_state);
    assert(first_object != 0);
    first_object->transform_state.position[0] = -0.5f;

    assert(pd_editor_scene_state_create_cube(&scene_state, &object_index) == PD_CORE_RESULT_OK);
    assert(object_index == 1u);
    assert(scene_state.object_count == 2u);
    assert(scene_state.active_object_index == 1u);
    second_object = pd_editor_scene_state_get_active(&scene_state);
    assert(second_object != 0);
    assert(second_object->object_id == 2u);
    assert(second_object->transform_state.position[0] > first_object->transform_state.position[0]);
    assert(first_object->mesh.vertices != second_object->mesh.vertices);
    assert(pd_editor_scene_state_set_active(&scene_state, 0u) == PD_CORE_RESULT_OK);
    assert(pd_editor_scene_state_get_active(&scene_state)->object_id == 1u);
    assert(pd_editor_scene_state_set_active(&scene_state, 99u) == PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);

    pd_editor_scene_state_free(&scene_state);
}

int main(void)
{
    pd_editor_scene_state_test_local_initializes_with_one_cube();
    pd_editor_scene_state_test_local_creates_independent_cubes();
    return 0;
}
