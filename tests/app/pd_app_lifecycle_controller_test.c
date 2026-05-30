#include "prismdraft/app/pd_app_lifecycle_controller.h"

#include "prismdraft/core/pd_core_mesh_validator.h"

#include <assert.h>

static void pd_app_lifecycle_controller_test_local_init_builds_active_cube_mesh(void)
{
    PdAppContextEntity app_context;

    assert(pd_app_lifecycle_controller_init(&app_context) == PD_CORE_RESULT_OK);
    assert(app_context.is_running == 1);
    assert(app_context.active_mesh.vertex_count == 8u);
    assert(app_context.active_mesh.half_edge_count == 24u);
    assert(app_context.active_mesh.face_count == 6u);
    assert(app_context.selection_state.kind == PD_EDITOR_SELECTION_KIND_NONE);
    assert(app_context.transform_state.position[0] == 0.0f);
    assert(app_context.transform_state.rotation_degrees[1] == 0.0f);
    assert(app_context.transform_state.scale[2] == 1.0f);
    assert(app_context.visual_state.background_color[3] == 255u);
    assert(app_context.visual_state.dark_intensity > 0.0f);
    assert(app_context.visual_state.shadow_color[3] > 0u);
    assert(app_context.visual_state.shadow_half_width > 0.0f);
    assert(pd_core_mesh_validator_check(&app_context.active_mesh) == PD_CORE_RESULT_OK);

    pd_app_lifecycle_controller_shutdown(&app_context);
}

static void pd_app_lifecycle_controller_test_local_shutdown_clears_owned_state(void)
{
    PdAppContextEntity app_context;

    assert(pd_app_lifecycle_controller_init(&app_context) == PD_CORE_RESULT_OK);
    pd_app_lifecycle_controller_shutdown(&app_context);

    assert(app_context.is_running == 0);
    assert(app_context.active_mesh.vertices == 0);
    assert(app_context.active_mesh.half_edges == 0);
    assert(app_context.active_mesh.faces == 0);
    assert(!pd_editor_selection_state_has_selection(&app_context.selection_state));
}

static void pd_app_lifecycle_controller_test_local_quit_marks_context_not_running(void)
{
    PdAppContextEntity app_context;

    assert(pd_app_lifecycle_controller_init(&app_context) == PD_CORE_RESULT_OK);
    assert(pd_app_lifecycle_controller_request_quit(&app_context) == PD_CORE_RESULT_OK);
    assert(app_context.is_running == 0);

    pd_app_lifecycle_controller_shutdown(&app_context);
}

static void pd_app_lifecycle_controller_test_local_rejects_null_context(void)
{
    assert(pd_app_lifecycle_controller_init(0) == PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);
    assert(pd_app_lifecycle_controller_request_quit(0) == PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);
    pd_app_lifecycle_controller_shutdown(0);
}

int main(void)
{
    pd_app_lifecycle_controller_test_local_init_builds_active_cube_mesh();
    pd_app_lifecycle_controller_test_local_shutdown_clears_owned_state();
    pd_app_lifecycle_controller_test_local_quit_marks_context_not_running();
    pd_app_lifecycle_controller_test_local_rejects_null_context();
    return 0;
}
