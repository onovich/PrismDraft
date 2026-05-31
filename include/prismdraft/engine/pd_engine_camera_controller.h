#ifndef PRISMDRAFT_ENGINE_PD_ENGINE_CAMERA_CONTROLLER_H
#define PRISMDRAFT_ENGINE_PD_ENGINE_CAMERA_CONTROLLER_H

#include "prismdraft/engine/pd_engine_camera_state.h"

PdEngineCameraState pd_engine_camera_controller_make_default(void);

void pd_engine_camera_controller_set_projection(PdEngineCameraState* camera_state, int projection);

#endif
