#include "prismdraft/animation/pd_animation_timeline_controller.h"

#include <assert.h>

static PdAnimationPrsKeyframeEntity pd_animation_timeline_controller_test_local_keyframe(
    float time_seconds,
    float position_x,
    float scale_x)
{
    PdAnimationPrsKeyframeEntity keyframe_entity;

    keyframe_entity.time_seconds = time_seconds;
    pd_animation_prs_entity_pose_set_identity(&keyframe_entity.pose);
    keyframe_entity.pose.position[0] = position_x;
    keyframe_entity.pose.scale[0] = scale_x;
    return keyframe_entity;
}

static void pd_animation_timeline_controller_test_local_updates_playback(void)
{
    PdAnimationTimelineController timeline_controller;

    assert(pd_animation_timeline_controller_init(0, 1.0f) == PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);
    assert(pd_animation_timeline_controller_init(&timeline_controller, -1.0f) == PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);
    assert(pd_animation_timeline_controller_init(&timeline_controller, 2.0f) == PD_CORE_RESULT_OK);
    assert(timeline_controller.time_seconds == 0.0f);
    assert(timeline_controller.duration_seconds == 2.0f);
    assert(timeline_controller.playback_speed == 1.0f);
    assert(!timeline_controller.is_playing);
    assert(!timeline_controller.loop_enabled);

    assert(pd_animation_timeline_controller_update(&timeline_controller, 1.0f) == PD_CORE_RESULT_OK);
    assert(timeline_controller.time_seconds == 0.0f);

    pd_animation_timeline_controller_play(&timeline_controller);
    assert(timeline_controller.is_playing);
    assert(pd_animation_timeline_controller_update(&timeline_controller, 0.75f) == PD_CORE_RESULT_OK);
    assert(timeline_controller.time_seconds == 0.75f);

    assert(pd_animation_timeline_controller_set_playback_speed(&timeline_controller, 2.0f) == PD_CORE_RESULT_OK);
    assert(pd_animation_timeline_controller_update(&timeline_controller, 1.0f) == PD_CORE_RESULT_OK);
    assert(timeline_controller.time_seconds == 2.0f);
    assert(!timeline_controller.is_playing);

    pd_animation_timeline_controller_stop(&timeline_controller);
    assert(timeline_controller.time_seconds == 0.0f);
    assert(!timeline_controller.is_playing);
}

static void pd_animation_timeline_controller_test_local_wraps_when_looping(void)
{
    PdAnimationTimelineController timeline_controller;

    assert(pd_animation_timeline_controller_init(&timeline_controller, 2.0f) == PD_CORE_RESULT_OK);
    pd_animation_timeline_controller_set_loop_enabled(&timeline_controller, 1);
    pd_animation_timeline_controller_play(&timeline_controller);
    assert(pd_animation_timeline_controller_update(&timeline_controller, 2.5f) == PD_CORE_RESULT_OK);
    assert(timeline_controller.time_seconds == 0.5f);
    assert(timeline_controller.is_playing);
}

static void pd_animation_timeline_controller_test_local_samples_prs_track(void)
{
    PdAnimationTimelineController timeline_controller;
    PdAnimationPrsTrackEntity track_entity;
    PdAnimationPrsPoseEntity pose_entity;
    PdAnimationPrsKeyframeEntity first_keyframe =
        pd_animation_timeline_controller_test_local_keyframe(0.0f, 0.0f, 1.0f);
    PdAnimationPrsKeyframeEntity second_keyframe =
        pd_animation_timeline_controller_test_local_keyframe(2.0f, 10.0f, 3.0f);

    assert(pd_animation_timeline_controller_init(&timeline_controller, 2.0f) == PD_CORE_RESULT_OK);
    assert(pd_animation_prs_entity_track_init(&track_entity, 3u) == PD_CORE_RESULT_OK);
    assert(pd_animation_prs_entity_track_append_keyframe(&track_entity, &first_keyframe) == PD_CORE_RESULT_OK);
    assert(pd_animation_prs_entity_track_append_keyframe(&track_entity, &second_keyframe) == PD_CORE_RESULT_OK);

    timeline_controller.time_seconds = 1.0f;
    assert(pd_animation_timeline_controller_sample_prs_track(&timeline_controller, &track_entity, &pose_entity) ==
           PD_CORE_RESULT_OK);
    assert(pose_entity.position[0] == 5.0f);
    assert(pose_entity.scale[0] == 2.0f);
    assert(pose_entity.rotation[3] == 1.0f);

    timeline_controller.time_seconds = 4.0f;
    assert(pd_animation_timeline_controller_sample_prs_track(&timeline_controller, &track_entity, &pose_entity) ==
           PD_CORE_RESULT_OK);
    assert(pose_entity.position[0] == 10.0f);

    pd_animation_prs_entity_track_free(&track_entity);
}

static void pd_animation_timeline_controller_test_local_rejects_invalid_inputs(void)
{
    PdAnimationTimelineController timeline_controller;
    PdAnimationPrsTrackEntity track_entity;
    PdAnimationPrsPoseEntity pose_entity;

    assert(pd_animation_timeline_controller_init(&timeline_controller, 1.0f) == PD_CORE_RESULT_OK);
    assert(pd_animation_timeline_controller_set_playback_speed(&timeline_controller, 0.0f) ==
           PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);
    assert(pd_animation_timeline_controller_update(&timeline_controller, -0.1f) ==
           PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);
    assert(pd_animation_prs_entity_track_init(&track_entity, 1u) == PD_CORE_RESULT_OK);
    assert(pd_animation_timeline_controller_sample_prs_track(&timeline_controller, &track_entity, &pose_entity) ==
           PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);
    pd_animation_prs_entity_track_free(&track_entity);
}

int main(void)
{
    pd_animation_timeline_controller_test_local_updates_playback();
    pd_animation_timeline_controller_test_local_wraps_when_looping();
    pd_animation_timeline_controller_test_local_samples_prs_track();
    pd_animation_timeline_controller_test_local_rejects_invalid_inputs();
    return 0;
}
