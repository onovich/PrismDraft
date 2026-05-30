#include "prismdraft/editor/pd_editor_modeling_service.h"

#include "prismdraft/core/pd_core_cube_fixture.h"
#include "prismdraft/core/pd_core_mesh_storage_controller.h"
#include "prismdraft/core/pd_core_mesh_validator.h"

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
    pd_editor_modeling_service_test_local_rejects_invalid_arguments();
    return 0;
}
