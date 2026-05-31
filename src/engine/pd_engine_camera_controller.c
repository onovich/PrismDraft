#include "prismdraft/engine/pd_engine_camera_controller.h"

static const float PD_ENGINE_CAMERA_CONTROLLER_ORTHOGRAPHIC_SIZE = 4.5f;
static const float PD_ENGINE_CAMERA_CONTROLLER_PERSPECTIVE_FOVY = 35.0f;

PdEngineCameraState pd_engine_camera_controller_make_default(void)
{
    PdEngineCameraState camera_state;

    camera_state.camera.position = (Vector3){ 4.0f, 4.0f, 4.0f };
    camera_state.camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera_state.camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera_state.camera.projection = CAMERA_ORTHOGRAPHIC;
    camera_state.camera.fovy = PD_ENGINE_CAMERA_CONTROLLER_ORTHOGRAPHIC_SIZE;
    camera_state.orbit_distance = 6.0f;

    return camera_state;
}

void pd_engine_camera_controller_set_projection(PdEngineCameraState* camera_state, int projection)
{
    if (camera_state == 0) {
        return;
    }

    if (projection == CAMERA_PERSPECTIVE) {
        camera_state->camera.projection = CAMERA_PERSPECTIVE;
        camera_state->camera.fovy = PD_ENGINE_CAMERA_CONTROLLER_PERSPECTIVE_FOVY;
        return;
    }

    camera_state->camera.projection = CAMERA_ORTHOGRAPHIC;
    camera_state->camera.fovy = PD_ENGINE_CAMERA_CONTROLLER_ORTHOGRAPHIC_SIZE;
}
