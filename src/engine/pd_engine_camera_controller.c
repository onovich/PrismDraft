#include "prismdraft/engine/pd_engine_camera_controller.h"

PdEngineCameraState pd_engine_camera_controller_make_default(void)
{
    PdEngineCameraState camera_state;

    camera_state.camera.position = (Vector3){ 4.0f, 4.0f, 4.0f };
    camera_state.camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera_state.camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera_state.camera.fovy = 35.0f;
    camera_state.camera.projection = CAMERA_ORTHOGRAPHIC;
    camera_state.orbit_distance = 6.0f;

    return camera_state;
}
