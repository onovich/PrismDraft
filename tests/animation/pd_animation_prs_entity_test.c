#include "prismdraft/animation/pd_animation_prs_entity.h"

#include <assert.h>

static PdAnimationPrsKeyframeEntity pd_animation_prs_entity_test_local_keyframe(float time_seconds, float position_x)
{
    PdAnimationPrsKeyframeEntity keyframe_entity;

    keyframe_entity.time_seconds = time_seconds;
    pd_animation_prs_entity_pose_set_identity(&keyframe_entity.pose);
    keyframe_entity.pose.position[0] = position_x;
    return keyframe_entity;
}

static void pd_animation_prs_entity_test_local_sets_identity_pose(void)
{
    PdAnimationPrsPoseEntity pose_entity;

    pd_animation_prs_entity_pose_set_identity(&pose_entity);

    assert(pose_entity.position[0] == 0.0f);
    assert(pose_entity.position[1] == 0.0f);
    assert(pose_entity.position[2] == 0.0f);
    assert(pose_entity.rotation[0] == 0.0f);
    assert(pose_entity.rotation[1] == 0.0f);
    assert(pose_entity.rotation[2] == 0.0f);
    assert(pose_entity.rotation[3] == 1.0f);
    assert(pose_entity.scale[0] == 1.0f);
    assert(pose_entity.scale[1] == 1.0f);
    assert(pose_entity.scale[2] == 1.0f);
}

static void pd_animation_prs_entity_test_local_tracks_ordered_keyframes(void)
{
    PdAnimationPrsTrackEntity track_entity;
    PdAnimationPrsKeyframeEntity first_keyframe = pd_animation_prs_entity_test_local_keyframe(0.0f, 1.0f);
    PdAnimationPrsKeyframeEntity second_keyframe = pd_animation_prs_entity_test_local_keyframe(1.0f, 2.0f);
    PdAnimationPrsKeyframeEntity duplicate_keyframe = pd_animation_prs_entity_test_local_keyframe(1.0f, 3.0f);

    assert(pd_animation_prs_entity_track_init(0, 0u) == PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);
    assert(pd_animation_prs_entity_track_init(&track_entity, PD_ANIMATION_PRS_ENTITY_INVALID_OBJECT_INDEX) ==
           PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);
    assert(pd_animation_prs_entity_track_init(&track_entity, 7u) == PD_CORE_RESULT_OK);
    assert(track_entity.object_index == 7u);
    assert(track_entity.keyframes == 0);
    assert(track_entity.keyframe_count == 0u);
    assert(track_entity.keyframe_capacity == 0u);

    assert(pd_animation_prs_entity_track_reserve(&track_entity, 2u) == PD_CORE_RESULT_OK);
    assert(track_entity.keyframe_capacity >= 2u);

    assert(pd_animation_prs_entity_track_append_keyframe(&track_entity, &first_keyframe) == PD_CORE_RESULT_OK);
    assert(pd_animation_prs_entity_track_append_keyframe(&track_entity, &second_keyframe) == PD_CORE_RESULT_OK);
    assert(pd_animation_prs_entity_track_append_keyframe(&track_entity, &duplicate_keyframe) ==
           PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);

    assert(track_entity.keyframe_count == 2u);
    assert(track_entity.keyframes[0].time_seconds == 0.0f);
    assert(track_entity.keyframes[1].time_seconds == 1.0f);
    assert(track_entity.keyframes[1].pose.position[0] == 2.0f);

    pd_animation_prs_entity_track_free(&track_entity);
    assert(track_entity.object_index == PD_ANIMATION_PRS_ENTITY_INVALID_OBJECT_INDEX);
    assert(track_entity.keyframes == 0);
    assert(track_entity.keyframe_count == 0u);
    assert(track_entity.keyframe_capacity == 0u);
}

static void pd_animation_prs_entity_test_local_rejects_negative_time(void)
{
    PdAnimationPrsTrackEntity track_entity;
    PdAnimationPrsKeyframeEntity keyframe_entity = pd_animation_prs_entity_test_local_keyframe(-1.0f, 0.0f);

    assert(pd_animation_prs_entity_track_init(&track_entity, 1u) == PD_CORE_RESULT_OK);
    assert(pd_animation_prs_entity_track_append_keyframe(&track_entity, &keyframe_entity) ==
           PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);
    pd_animation_prs_entity_track_free(&track_entity);
}

int main(void)
{
    pd_animation_prs_entity_test_local_sets_identity_pose();
    pd_animation_prs_entity_test_local_tracks_ordered_keyframes();
    pd_animation_prs_entity_test_local_rejects_negative_time();
    return 0;
}
