#ifndef PRISMDRAFT_RENDER_PD_RENDER_TARGET_CONTROLLER_H
#define PRISMDRAFT_RENDER_PD_RENDER_TARGET_CONTROLLER_H

#include "prismdraft/core/pd_core_result_entity.h"
#include "prismdraft/render/pd_render_target_config.h"

#include "raylib.h"

typedef struct PdRenderTargetController {
    PdRenderTargetConfig config;
    RenderTexture2D color_depth_target;
    RenderTexture2D normal_target;
    RenderTexture2D depth_target;
} PdRenderTargetController;

PdCoreResult pd_render_target_controller_init(PdRenderTargetController* target_controller, PdRenderTargetConfig config);

void pd_render_target_controller_free(PdRenderTargetController* target_controller);

int pd_render_target_controller_is_ready(const PdRenderTargetController* target_controller);

#endif
