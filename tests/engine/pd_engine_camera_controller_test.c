#include "prismdraft/engine/pd_engine_camera_controller.h"

#include <assert.h>

int main(void)
{
    PdEngineCameraState camera_state = pd_engine_camera_controller_make_default();

    assert(camera_state.camera.projection == CAMERA_ORTHOGRAPHIC);
    assert(camera_state.camera.position.x == 4.0f);
    assert(camera_state.camera.position.y == 4.0f);
    assert(camera_state.camera.position.z == 4.0f);
    assert(camera_state.camera.target.x == 0.0f);
    assert(camera_state.camera.target.y == 0.0f);
    assert(camera_state.camera.target.z == 0.0f);
    assert(camera_state.camera.fovy == 4.5f);
    assert(camera_state.orbit_distance == 6.0f);

    return 0;
}
