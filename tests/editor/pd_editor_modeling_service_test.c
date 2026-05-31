#include "prismdraft/editor/pd_editor_modeling_service.h"

#include "prismdraft/core/pd_core_cube_fixture.h"
#include "prismdraft/core/pd_core_mesh_storage_controller.h"
#include "prismdraft/core/pd_core_mesh_validator.h"
#include "prismdraft/editor/pd_editor_pick_service.h"

#include <assert.h>

static void pd_editor_modeling_service_test_local_applies_modeling_tool(void)
{
    PdCoreMeshEntity mesh_entity = { 0 };
    PdEditorModelingServiceConfig config = pd_editor_modeling_service_config_default();

    assert(pd_core_cube_fixture_build(&mesh_entity) == PD_CORE_RESULT_OK);
    assert(pd_editor_modeling_service_apply(&mesh_entity, 1u, PD_EDITOR_TOOL_KIND_INSET, config) ==
           PD_CORE_RESULT_OK);
    assert(mesh_entity.face_count > 6u);
    assert(pd_core_mesh_validator_check(&mesh_entity) == PD_CORE_RESULT_OK);

    pd_core_mesh_storage_controller_free(&mesh_entity);
}

static void pd_editor_modeling_service_test_local_keeps_modeled_mesh_pickable(PdEditorToolKind tool_kind)
{
    PdCoreMeshEntity mesh_entity = { 0 };
    PdEditorModelingServiceConfig config = pd_editor_modeling_service_config_default();
    PdEditorPickServiceHit hit;
    float ray_origin[3] = { 0.0f, 0.0f, 5.0f };
    float ray_direction[3] = { 0.0f, 0.0f, -1.0f };

    assert(pd_core_cube_fixture_build(&mesh_entity) == PD_CORE_RESULT_OK);
    assert(pd_editor_modeling_service_apply(&mesh_entity, 1u, tool_kind, config) == PD_CORE_RESULT_OK);
    assert(pd_core_mesh_validator_check(&mesh_entity) == PD_CORE_RESULT_OK);
    assert(pd_editor_pick_service_pick_face(&mesh_entity, ray_origin, ray_direction, &hit) == PD_CORE_RESULT_OK);

    pd_core_mesh_storage_controller_free(&mesh_entity);
}

static void pd_editor_modeling_service_test_local_keeps_all_tools_pickable(void)
{
    pd_editor_modeling_service_test_local_keeps_modeled_mesh_pickable(PD_EDITOR_TOOL_KIND_INSET);
    pd_editor_modeling_service_test_local_keeps_modeled_mesh_pickable(PD_EDITOR_TOOL_KIND_EXTRUDE);
    pd_editor_modeling_service_test_local_keeps_modeled_mesh_pickable(PD_EDITOR_TOOL_KIND_BEVEL);
    pd_editor_modeling_service_test_local_keeps_modeled_mesh_pickable(PD_EDITOR_TOOL_KIND_LOOP_CUT);
}

static void pd_editor_modeling_service_test_local_rejects_invalid_arguments(void)
{
    PdCoreMeshEntity mesh_entity = { 0 };
    PdEditorModelingServiceConfig config = pd_editor_modeling_service_config_default();

    assert(pd_editor_modeling_service_apply(0, 1u, PD_EDITOR_TOOL_KIND_INSET, config) ==
           PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);
    assert(pd_core_cube_fixture_build(&mesh_entity) == PD_CORE_RESULT_OK);
    assert(pd_editor_modeling_service_apply(
               &mesh_entity,
               PD_CORE_MESH_ENTITY_INVALID_INDEX,
               PD_EDITOR_TOOL_KIND_INSET,
               config) == PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);
    assert(pd_editor_modeling_service_apply(&mesh_entity, 1u, PD_EDITOR_TOOL_KIND_VIEW, config) ==
           PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);

    pd_core_mesh_storage_controller_free(&mesh_entity);
}

int main(void)
{
    pd_editor_modeling_service_test_local_applies_modeling_tool();
    pd_editor_modeling_service_test_local_keeps_all_tools_pickable();
    pd_editor_modeling_service_test_local_rejects_invalid_arguments();
    return 0;
}
