#include "prismdraft/app/pd_app_lifecycle_controller.h"

#include "prismdraft/core/pd_core_cube_fixture.h"
#include "prismdraft/core/pd_core_mesh_storage_controller.h"

PdCoreResult pd_app_lifecycle_controller_init(PdAppContextEntity* app_context)
{
    PdCoreResult result;

    if (app_context == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    *app_context = (PdAppContextEntity){ 0 };

    result = pd_core_cube_fixture_build(&app_context->active_mesh);
    if (result != PD_CORE_RESULT_OK) {
        pd_app_lifecycle_controller_shutdown(app_context);
        return result;
    }

    app_context->is_running = 1;
    return PD_CORE_RESULT_OK;
}

void pd_app_lifecycle_controller_shutdown(PdAppContextEntity* app_context)
{
    if (app_context == 0) {
        return;
    }

    pd_core_mesh_storage_controller_free(&app_context->active_mesh);
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
