#include "prismdraft/render/pd_render_target_controller.h"

static RenderTexture2D pd_render_target_controller_local_load_color_depth_target(int width, int height)
{
    return LoadRenderTexture(width, height);
}

PdCoreResult pd_render_target_controller_init(PdRenderTargetController* target_controller, PdRenderTargetConfig config)
{
    if (target_controller == 0 || !pd_render_target_config_is_valid(config)) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    *target_controller = (PdRenderTargetController){ 0 };
    target_controller->config = config;
    target_controller->color_depth_target =
        pd_render_target_controller_local_load_color_depth_target(config.width, config.height);

    if (!IsRenderTextureValid(target_controller->color_depth_target)) {
        pd_render_target_controller_free(target_controller);
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    target_controller->normal_target = LoadRenderTexture(config.width, config.height);
    if (!IsRenderTextureValid(target_controller->normal_target)) {
        pd_render_target_controller_free(target_controller);
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    target_controller->depth_target = LoadRenderTexture(config.width, config.height);
    if (!IsRenderTextureValid(target_controller->depth_target)) {
        pd_render_target_controller_free(target_controller);
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    return PD_CORE_RESULT_OK;
}

void pd_render_target_controller_free(PdRenderTargetController* target_controller)
{
    if (target_controller == 0) {
        return;
    }

    if (IsRenderTextureValid(target_controller->depth_target)) {
        UnloadRenderTexture(target_controller->depth_target);
    }

    if (IsRenderTextureValid(target_controller->normal_target)) {
        UnloadRenderTexture(target_controller->normal_target);
    }

    if (IsRenderTextureValid(target_controller->color_depth_target)) {
        UnloadRenderTexture(target_controller->color_depth_target);
    }

    *target_controller = (PdRenderTargetController){ 0 };
}

int pd_render_target_controller_is_ready(const PdRenderTargetController* target_controller)
{
    if (target_controller == 0) {
        return 0;
    }

    if (!pd_render_target_config_is_valid(target_controller->config)) {
        return 0;
    }

    if (target_controller->color_depth_target.id <= 0u || target_controller->color_depth_target.texture.id <= 0u ||
        target_controller->color_depth_target.depth.id <= 0u) {
        return 0;
    }

    if (target_controller->normal_target.id <= 0u || target_controller->normal_target.texture.id <= 0u) {
        return 0;
    }

    if (target_controller->depth_target.id <= 0u || target_controller->depth_target.texture.id <= 0u) {
        return 0;
    }

    return 1;
}
