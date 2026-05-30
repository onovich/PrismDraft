#ifndef PRISMDRAFT_APP_PD_APP_LIFECYCLE_CONTROLLER_H
#define PRISMDRAFT_APP_PD_APP_LIFECYCLE_CONTROLLER_H

#include "prismdraft/app/pd_app_context_entity.h"
#include "prismdraft/core/pd_core_result_entity.h"

PdCoreResult pd_app_lifecycle_controller_init(PdAppContextEntity* app_context);

void pd_app_lifecycle_controller_shutdown(PdAppContextEntity* app_context);

PdCoreResult pd_app_lifecycle_controller_request_quit(PdAppContextEntity* app_context);

#endif
