#ifndef PRISMDRAFT_ENGINE_PD_ENGINE_CAMERA_STATE_H
#define PRISMDRAFT_ENGINE_PD_ENGINE_CAMERA_STATE_H

#include "raylib.h"

typedef struct PdEngineCameraState {
    Camera3D camera;
    float orbit_distance;
} PdEngineCameraState;

#endif
