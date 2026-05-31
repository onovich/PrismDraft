#include "prismdraft/app/pd_app_lifecycle_controller.h"

#include "prismdraft/editor/pd_editor_panel_state.h"
#include "prismdraft/editor/pd_editor_scene_state.h"
#include "prismdraft/editor/pd_editor_selection_state.h"
#include "prismdraft/editor/pd_editor_tool_state.h"
#include "prismdraft/editor/pd_editor_visual_state.h"

PdCoreResult pd_app_lifecycle_controller_init(PdAppContextEntity* app_context)
{
    PdCoreResult result;

    if (app_context == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    *app_context = (PdAppContextEntity){ 0 };

    result = pd_editor_panel_state_init(&app_context->panel_state);
    if (result != PD_CORE_RESULT_OK) {
        pd_app_lifecycle_controller_shutdown(app_context);
        return result;
    }

    result = pd_editor_scene_state_init(&app_context->scene_state);
    if (result != PD_CORE_RESULT_OK) {
        pd_app_lifecycle_controller_shutdown(app_context);
        return result;
    }

    result = pd_editor_selection_state_init(&app_context->selection_state);
    if (result != PD_CORE_RESULT_OK) {
        pd_app_lifecycle_controller_shutdown(app_context);
        return result;
    }

    result = pd_editor_tool_state_init(&app_context->tool_state);
    if (result != PD_CORE_RESULT_OK) {
        pd_app_lifecycle_controller_shutdown(app_context);
        return result;
    }

    result = pd_editor_visual_state_init(&app_context->visual_state);
    if (result != PD_CORE_RESULT_OK) {
        pd_app_lifecycle_controller_shutdown(app_context);
        return result;
    }
    app_context->visual_state.shadow_plane_y = app_context->visual_state.ground_y;

    app_context->is_running = 1;
    return PD_CORE_RESULT_OK;
}

void pd_app_lifecycle_controller_shutdown(PdAppContextEntity* app_context)
{
    if (app_context == 0) {
        return;
    }

    pd_editor_scene_state_free(&app_context->scene_state);
    pd_editor_selection_state_clear(&app_context->selection_state);
    app_context->is_running = 0;
}

PdCoreResult pd_app_lifecycle_controller_request_quit(PdAppContextEntity* app_context)
{
    if (app_context == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    app_context->is_running = 0;
    return PD_CORE_RESULT_OK;
}
