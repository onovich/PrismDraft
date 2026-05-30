#include "prismdraft/animation/pd_animation_timeline_controller.h"

static float pd_animation_timeline_controller_local_lerp(float start_value, float end_value, float alpha)
{
    return start_value + ((end_value - start_value) * alpha);
}

static void pd_animation_timeline_controller_local_copy_pose(
    PdAnimationPrsPoseEntity* target_pose,
    const PdAnimationPrsPoseEntity* source_pose)
{
    int component_index;

    for (component_index = 0; component_index < 3; component_index++) {
        target_pose->position[component_index] = source_pose->position[component_index];
        target_pose->scale[component_index] = source_pose->scale[component_index];
    }

    for (component_index = 0; component_index < 4; component_index++) {
        target_pose->rotation[component_index] = source_pose->rotation[component_index];
    }
}

static void pd_animation_timeline_controller_local_lerp_pose(
    PdAnimationPrsPoseEntity* target_pose,
    const PdAnimationPrsPoseEntity* start_pose,
    const PdAnimationPrsPoseEntity* end_pose,
    float alpha)
{
    int component_index;

    for (component_index = 0; component_index < 3; component_index++) {
        target_pose->position[component_index] = pd_animation_timeline_controller_local_lerp(
            start_pose->position[component_index],
            end_pose->position[component_index],
            alpha);
        target_pose->scale[component_index] = pd_animation_timeline_controller_local_lerp(
            start_pose->scale[component_index],
            end_pose->scale[component_index],
            alpha);
    }

    for (component_index = 0; component_index < 4; component_index++) {
        target_pose->rotation[component_index] = pd_animation_timeline_controller_local_lerp(
            start_pose->rotation[component_index],
            end_pose->rotation[component_index],
            alpha);
    }
}

PdCoreResult pd_animation_timeline_controller_init(
    PdAnimationTimelineController* timeline_controller,
    float duration_seconds)
{
    if (timeline_controller == 0 || duration_seconds < 0.0f) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    timeline_controller->time_seconds = 0.0f;
    timeline_controller->duration_seconds = duration_seconds;
    timeline_controller->playback_speed = 1.0f;
    timeline_controller->is_playing = 0;
    timeline_controller->loop_enabled = 0;
    return PD_CORE_RESULT_OK;
}

void pd_animation_timeline_controller_play(PdAnimationTimelineController* timeline_controller)
{
    if (timeline_controller == 0) {
        return;
    }

    timeline_controller->is_playing = 1;
}

void pd_animation_timeline_controller_pause(PdAnimationTimelineController* timeline_controller)
{
    if (timeline_controller == 0) {
        return;
    }

    timeline_controller->is_playing = 0;
}

void pd_animation_timeline_controller_stop(PdAnimationTimelineController* timeline_controller)
{
    if (timeline_controller == 0) {
        return;
    }

    timeline_controller->time_seconds = 0.0f;
    timeline_controller->is_playing = 0;
}

void pd_animation_timeline_controller_set_loop_enabled(
    PdAnimationTimelineController* timeline_controller,
    int loop_enabled)
{
    if (timeline_controller == 0) {
        return;
    }

    timeline_controller->loop_enabled = loop_enabled ? 1 : 0;
}

PdCoreResult pd_animation_timeline_controller_set_playback_speed(
    PdAnimationTimelineController* timeline_controller,
    float playback_speed)
{
    if (timeline_controller == 0 || playback_speed <= 0.0f) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    timeline_controller->playback_speed = playback_speed;
    return PD_CORE_RESULT_OK;
}

PdCoreResult pd_animation_timeline_controller_update(
    PdAnimationTimelineController* timeline_controller,
    float delta_seconds)
{
    float next_time_seconds;

    if (timeline_controller == 0 || delta_seconds < 0.0f || timeline_controller->playback_speed <= 0.0f) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (!timeline_controller->is_playing) {
        return PD_CORE_RESULT_OK;
    }

    if (timeline_controller->duration_seconds <= 0.0f) {
        timeline_controller->time_seconds = 0.0f;
        timeline_controller->is_playing = 0;
        return PD_CORE_RESULT_OK;
    }

    next_time_seconds = timeline_controller->time_seconds + (delta_seconds * timeline_controller->playback_speed);
    if (timeline_controller->loop_enabled) {
        while (next_time_seconds >= timeline_controller->duration_seconds) {
            next_time_seconds -= timeline_controller->duration_seconds;
        }
        timeline_controller->time_seconds = next_time_seconds;
        return PD_CORE_RESULT_OK;
    }

    if (next_time_seconds >= timeline_controller->duration_seconds) {
        timeline_controller->time_seconds = timeline_controller->duration_seconds;
        timeline_controller->is_playing = 0;
        return PD_CORE_RESULT_OK;
    }

    timeline_controller->time_seconds = next_time_seconds;
    return PD_CORE_RESULT_OK;
}

PdCoreResult pd_animation_timeline_controller_sample_prs_track(
    const PdAnimationTimelineController* timeline_controller,
    const PdAnimationPrsTrackEntity* track_entity,
    PdAnimationPrsPoseEntity* pose_entity)
{
    uint32_t keyframe_index;
    float time_seconds;

    if (timeline_controller == 0 || track_entity == 0 || pose_entity == 0 || track_entity->keyframes == 0 ||
        track_entity->keyframe_count == 0u) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    time_seconds = timeline_controller->time_seconds;
    if (time_seconds <= track_entity->keyframes[0].time_seconds) {
        pd_animation_timeline_controller_local_copy_pose(pose_entity, &track_entity->keyframes[0].pose);
        return PD_CORE_RESULT_OK;
    }

    if (time_seconds >= track_entity->keyframes[track_entity->keyframe_count - 1u].time_seconds) {
        pd_animation_timeline_controller_local_copy_pose(
            pose_entity,
            &track_entity->keyframes[track_entity->keyframe_count - 1u].pose);
        return PD_CORE_RESULT_OK;
    }

    for (keyframe_index = 1u; keyframe_index < track_entity->keyframe_count; keyframe_index++) {
        const PdAnimationPrsKeyframeEntity* end_keyframe = &track_entity->keyframes[keyframe_index];

        if (time_seconds <= end_keyframe->time_seconds) {
            const PdAnimationPrsKeyframeEntity* start_keyframe = &track_entity->keyframes[keyframe_index - 1u];
            float span_seconds = end_keyframe->time_seconds - start_keyframe->time_seconds;
            float alpha = (time_seconds - start_keyframe->time_seconds) / span_seconds;

            pd_animation_timeline_controller_local_lerp_pose(
                pose_entity,
                &start_keyframe->pose,
                &end_keyframe->pose,
                alpha);
            return PD_CORE_RESULT_OK;
        }
    }

    pd_animation_timeline_controller_local_copy_pose(
        pose_entity,
        &track_entity->keyframes[track_entity->keyframe_count - 1u].pose);
    return PD_CORE_RESULT_OK;
}
