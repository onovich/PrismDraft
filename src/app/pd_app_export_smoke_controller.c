#include "prismdraft/app/pd_app_lifecycle_controller.h"
#include "prismdraft/editor/pd_editor_modeling_service.h"
#include "prismdraft/editor/pd_editor_selection_state.h"
#include "prismdraft/export/pd_export_glb_writer.h"
#include "prismdraft/export/pd_export_gltf_writer.h"
#include "prismdraft/export/pd_export_mesh_buffer.h"

int main(void)
{
    PdAppContextEntity app_context;
    PdEditorSceneObjectEntity* active_object;
    PdExportMeshBuffer export_mesh_buffer = { 0 };
    PdEditorModelingServiceConfig modeling_config = pd_editor_modeling_service_config_default();
    int run_result = 0;

    if (pd_app_lifecycle_controller_init(&app_context) != PD_CORE_RESULT_OK) {
        return 1;
    }

    if (app_context.panel_state.active_panel != PD_EDITOR_PANEL_KIND_MODELING || !app_context.panel_state.is_open) {
        pd_app_lifecycle_controller_shutdown(&app_context);
        return 1;
    }

    if (pd_editor_selection_state_select_face(&app_context.selection_state, 1u) != PD_CORE_RESULT_OK) {
        pd_app_lifecycle_controller_shutdown(&app_context);
        return 1;
    }

    active_object = pd_editor_scene_state_get_active(&app_context.scene_state);
    if (active_object == 0) {
        pd_app_lifecycle_controller_shutdown(&app_context);
        return 1;
    }

    if (pd_editor_modeling_service_apply(
            &active_object->mesh,
            app_context.selection_state.primary_index,
            PD_EDITOR_TOOL_KIND_EXTRUDE,
            modeling_config) != PD_CORE_RESULT_OK) {
        pd_app_lifecycle_controller_shutdown(&app_context);
        return 1;
    }

    if (pd_export_mesh_buffer_build_from_mesh(&export_mesh_buffer, &active_object->mesh) != PD_CORE_RESULT_OK) {
        pd_app_lifecycle_controller_shutdown(&app_context);
        return 1;
    }

    if (pd_export_gltf_writer_write_file("captures/phase6_export_smoke.gltf", &export_mesh_buffer) !=
        PD_CORE_RESULT_OK) {
        run_result = 1;
    }

    if (run_result == 0 &&
        pd_export_glb_writer_write_file("captures/phase6_export_smoke.glb", &export_mesh_buffer) !=
            PD_CORE_RESULT_OK) {
        run_result = 1;
    }

    pd_export_mesh_buffer_free(&export_mesh_buffer);
    pd_app_lifecycle_controller_shutdown(&app_context);
    return run_result;
}
