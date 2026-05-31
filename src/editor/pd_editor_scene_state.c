#include "prismdraft/editor/pd_editor_scene_state.h"

#include "prismdraft/core/pd_core_cube_fixture.h"
#include "prismdraft/core/pd_core_mesh_storage_controller.h"

static void pd_editor_scene_state_local_clear_object(PdEditorSceneObjectEntity* object_entity)
{
    if (object_entity == 0) {
        return;
    }

    *object_entity = (PdEditorSceneObjectEntity){ 0 };
}

static PdCoreResult pd_editor_scene_state_local_init_object(
    PdEditorSceneObjectEntity* object_entity,
    uint32_t object_id,
    uint32_t object_index)
{
    PdCoreResult result;

    if (object_entity == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    pd_editor_scene_state_local_clear_object(object_entity);
    result = pd_core_cube_fixture_build(&object_entity->mesh);
    if (result != PD_CORE_RESULT_OK) {
        pd_core_mesh_storage_controller_free(&object_entity->mesh);
        pd_editor_scene_state_local_clear_object(object_entity);
        return result;
    }

    result = pd_editor_transform_state_init(&object_entity->transform_state);
    if (result != PD_CORE_RESULT_OK) {
        pd_core_mesh_storage_controller_free(&object_entity->mesh);
        pd_editor_scene_state_local_clear_object(object_entity);
        return result;
    }

    object_entity->object_id = object_id;
    object_entity->transform_state.position[0] = (float)object_index * 1.45f;
    object_entity->is_allocated = 1;
    return PD_CORE_RESULT_OK;
}

PdCoreResult pd_editor_scene_state_init(PdEditorSceneState* scene_state)
{
    if (scene_state == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    *scene_state = (PdEditorSceneState){ 0 };
    scene_state->active_object_index = 0u;
    scene_state->next_object_id = 1u;
    return pd_editor_scene_state_create_cube(scene_state, 0);
}

void pd_editor_scene_state_free(PdEditorSceneState* scene_state)
{
    uint32_t object_index;

    if (scene_state == 0) {
        return;
    }

    for (object_index = 0u; object_index < scene_state->object_count; object_index++) {
        pd_core_mesh_storage_controller_free(&scene_state->objects[object_index].mesh);
        pd_editor_scene_state_local_clear_object(&scene_state->objects[object_index]);
    }

    scene_state->object_count = 0u;
    scene_state->active_object_index = 0u;
    scene_state->next_object_id = 1u;
}

PdCoreResult pd_editor_scene_state_create_cube(PdEditorSceneState* scene_state, uint32_t* object_index)
{
    uint32_t next_index;
    PdCoreResult result;

    if (scene_state == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (scene_state->object_count >= PD_EDITOR_SCENE_STATE_MAX_OBJECTS) {
        return PD_CORE_RESULT_ERROR_OUT_OF_MEMORY;
    }

    next_index = scene_state->object_count;
    result = pd_editor_scene_state_local_init_object(
        &scene_state->objects[next_index],
        scene_state->next_object_id,
        next_index);
    if (result != PD_CORE_RESULT_OK) {
        return result;
    }

    scene_state->object_count++;
    scene_state->next_object_id++;
    scene_state->active_object_index = next_index;
    if (object_index != 0) {
        *object_index = next_index;
    }

    return PD_CORE_RESULT_OK;
}

PdCoreResult pd_editor_scene_state_set_active(PdEditorSceneState* scene_state, uint32_t object_index)
{
    if (scene_state == 0 || object_index >= scene_state->object_count ||
        !scene_state->objects[object_index].is_allocated) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    scene_state->active_object_index = object_index;
    return PD_CORE_RESULT_OK;
}

PdEditorSceneObjectEntity* pd_editor_scene_state_get_active(PdEditorSceneState* scene_state)
{
    if (scene_state == 0 || scene_state->active_object_index >= scene_state->object_count) {
        return 0;
    }

    if (!scene_state->objects[scene_state->active_object_index].is_allocated) {
        return 0;
    }

    return &scene_state->objects[scene_state->active_object_index];
}

const PdEditorSceneObjectEntity* pd_editor_scene_state_get_active_const(const PdEditorSceneState* scene_state)
{
    if (scene_state == 0 || scene_state->active_object_index >= scene_state->object_count) {
        return 0;
    }

    if (!scene_state->objects[scene_state->active_object_index].is_allocated) {
        return 0;
    }

    return &scene_state->objects[scene_state->active_object_index];
}
