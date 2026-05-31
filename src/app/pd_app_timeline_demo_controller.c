#include "prismdraft/animation/pd_animation_prs_entity.h"
#include "prismdraft/animation/pd_animation_timeline_controller.h"
#include "prismdraft/app/pd_app_lifecycle_controller.h"

#include <math.h>

static int pd_app_timeline_demo_controller_local_near(float left, float right)
{
    return fabsf(left - right) < 0.0001f;
}

static PdAnimationPrsKeyframeEntity pd_app_timeline_demo_controller_local_keyframe(
    float time_seconds,
    float position_x,
    float position_y,
    float position_z,
    float scale)
{
    PdAnimationPrsKeyframeEntity keyframe_entity;

    keyframe_entity.time_seconds = time_seconds;
    pd_animation_prs_entity_pose_set_identity(&keyframe_entity.pose);
    keyframe_entity.pose.position[0] = position_x;
    keyframe_entity.pose.position[1] = position_y;
    keyframe_entity.pose.position[2] = position_z;
    keyframe_entity.pose.scale[0] = scale;
    keyframe_entity.pose.scale[1] = scale;
    keyframe_entity.pose.scale[2] = scale;
    return keyframe_entity;
}

static void pd_app_timeline_demo_controller_local_apply_pose(
    PdAppContextEntity* app_context,
    const PdAnimationPrsPoseEntity* pose_entity)
{
    int component_index;

    if (app_context == 0 || pose_entity == 0) {
        return;
    }

    for (component_index = 0; component_index < 3; component_index++) {
        app_context->transform_state.position[component_index] = pose_entity->position[component_index];
        app_context->transform_state.scale[component_index] = pose_entity->scale[component_index];
    }
}

int main(void)
{
    PdAppContextEntity app_context;
    PdAnimationTimelineController timeline_controller;
    PdAnimationPrsTrackEntity track_entity;
    PdAnimationPrsPoseEntity pose_entity;
    PdAnimationPrsKeyframeEntity first_keyframe =
        pd_app_timeline_demo_controller_local_keyframe(0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    PdAnimationPrsKeyframeEntity second_keyframe =
        pd_app_timeline_demo_controller_local_keyframe(2.0f, 0.8f, 0.4f, -0.6f, 1.4f);
    int run_result = 0;

    if (pd_app_lifecycle_controller_init(&app_context) != PD_CORE_RESULT_OK) {
        return 1;
    }

    if (app_context.panel_state.active_panel != PD_EDITOR_PANEL_KIND_MODELING || !app_context.panel_state.is_open) {
        pd_app_lifecycle_controller_shutdown(&app_context);
        return 1;
    }

    if (!app_context.visual_state.is_ground_visible || app_context.visual_state.ground_y >= 0.0f) {
        pd_app_lifecycle_controller_shutdown(&app_context);
        return 1;
    }

    if (pd_animation_timeline_controller_init(&timeline_controller, 2.0f) != PD_CORE_RESULT_OK) {
        pd_app_lifecycle_controller_shutdown(&app_context);
        return 1;
    }

    if (pd_animation_prs_entity_track_init(&track_entity, 0u) != PD_CORE_RESULT_OK) {
        pd_app_lifecycle_controller_shutdown(&app_context);
        return 1;
    }

    if (pd_animation_prs_entity_track_append_keyframe(&track_entity, &first_keyframe) != PD_CORE_RESULT_OK ||
        pd_animation_prs_entity_track_append_keyframe(&track_entity, &second_keyframe) != PD_CORE_RESULT_OK) {
        pd_animation_prs_entity_track_free(&track_entity);
        pd_app_lifecycle_controller_shutdown(&app_context);
        return 1;
    }

    pd_animation_timeline_controller_play(&timeline_controller);
    if (pd_animation_timeline_controller_update(&timeline_controller, 1.0f) != PD_CORE_RESULT_OK ||
        pd_animation_timeline_controller_sample_prs_track(&timeline_controller, &track_entity, &pose_entity) !=
            PD_CORE_RESULT_OK) {
        run_result = 1;
    }

    if (run_result == 0) {
        pd_app_timeline_demo_controller_local_apply_pose(&app_context, &pose_entity);
        if (!pd_app_timeline_demo_controller_local_near(app_context.transform_state.position[0], 0.4f) ||
            !pd_app_timeline_demo_controller_local_near(app_context.transform_state.position[1], 0.2f) ||
            !pd_app_timeline_demo_controller_local_near(app_context.transform_state.position[2], -0.3f) ||
            !pd_app_timeline_demo_controller_local_near(app_context.transform_state.scale[0], 1.2f)) {
            run_result = 1;
        }
    }

    pd_animation_prs_entity_track_free(&track_entity);
    pd_app_lifecycle_controller_shutdown(&app_context);
    return run_result;
}
