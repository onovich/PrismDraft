#include "prismdraft/animation/pd_animation_prs_entity.h"

#include <stdlib.h>
#include <string.h>

static PdCoreResult pd_animation_prs_entity_local_checked_keyframe_bytes(
    uint32_t keyframe_capacity,
    size_t* byte_count)
{
    size_t capacity = (size_t)keyframe_capacity;

    if (byte_count == 0 || capacity > (SIZE_MAX / sizeof(PdAnimationPrsKeyframeEntity))) {
        return PD_CORE_RESULT_ERROR_OUT_OF_MEMORY;
    }

    *byte_count = capacity * sizeof(PdAnimationPrsKeyframeEntity);
    return PD_CORE_RESULT_OK;
}

void pd_animation_prs_entity_pose_set_identity(PdAnimationPrsPoseEntity* pose_entity)
{
    if (pose_entity == 0) {
        return;
    }

    pose_entity->position[0] = 0.0f;
    pose_entity->position[1] = 0.0f;
    pose_entity->position[2] = 0.0f;
    pose_entity->rotation[0] = 0.0f;
    pose_entity->rotation[1] = 0.0f;
    pose_entity->rotation[2] = 0.0f;
    pose_entity->rotation[3] = 1.0f;
    pose_entity->scale[0] = 1.0f;
    pose_entity->scale[1] = 1.0f;
    pose_entity->scale[2] = 1.0f;
}

PdCoreResult pd_animation_prs_entity_track_init(PdAnimationPrsTrackEntity* track_entity, uint32_t object_index)
{
    if (track_entity == 0 || object_index == PD_ANIMATION_PRS_ENTITY_INVALID_OBJECT_INDEX) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    track_entity->object_index = object_index;
    track_entity->keyframes = 0;
    track_entity->keyframe_count = 0u;
    track_entity->keyframe_capacity = 0u;
    return PD_CORE_RESULT_OK;
}

void pd_animation_prs_entity_track_free(PdAnimationPrsTrackEntity* track_entity)
{
    if (track_entity == 0) {
        return;
    }

    free(track_entity->keyframes);
    track_entity->object_index = PD_ANIMATION_PRS_ENTITY_INVALID_OBJECT_INDEX;
    track_entity->keyframes = 0;
    track_entity->keyframe_count = 0u;
    track_entity->keyframe_capacity = 0u;
}

PdCoreResult pd_animation_prs_entity_track_reserve(
    PdAnimationPrsTrackEntity* track_entity,
    uint32_t keyframe_capacity)
{
    PdAnimationPrsKeyframeEntity* keyframes;
    size_t byte_count;

    if (track_entity == 0 || track_entity->object_index == PD_ANIMATION_PRS_ENTITY_INVALID_OBJECT_INDEX) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (keyframe_capacity <= track_entity->keyframe_capacity) {
        return PD_CORE_RESULT_OK;
    }

    if (pd_animation_prs_entity_local_checked_keyframe_bytes(keyframe_capacity, &byte_count) != PD_CORE_RESULT_OK) {
        return PD_CORE_RESULT_ERROR_OUT_OF_MEMORY;
    }

    keyframes = (PdAnimationPrsKeyframeEntity*)realloc(track_entity->keyframes, byte_count);
    if (keyframes == 0) {
        return PD_CORE_RESULT_ERROR_OUT_OF_MEMORY;
    }

    track_entity->keyframes = keyframes;
    track_entity->keyframe_capacity = keyframe_capacity;
    return PD_CORE_RESULT_OK;
}

PdCoreResult pd_animation_prs_entity_track_append_keyframe(
    PdAnimationPrsTrackEntity* track_entity,
    const PdAnimationPrsKeyframeEntity* keyframe_entity)
{
    uint32_t requested_capacity;
    PdCoreResult reserve_result;

    if (track_entity == 0 || keyframe_entity == 0 ||
        track_entity->object_index == PD_ANIMATION_PRS_ENTITY_INVALID_OBJECT_INDEX ||
        keyframe_entity->time_seconds < 0.0f) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (track_entity->keyframe_count > 0u &&
        keyframe_entity->time_seconds <= track_entity->keyframes[track_entity->keyframe_count - 1u].time_seconds) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (track_entity->keyframe_count == track_entity->keyframe_capacity) {
        if (track_entity->keyframe_capacity > UINT32_MAX / 2u) {
            return PD_CORE_RESULT_ERROR_OUT_OF_MEMORY;
        }

        requested_capacity = track_entity->keyframe_capacity == 0u ? 4u : track_entity->keyframe_capacity * 2u;
        reserve_result = pd_animation_prs_entity_track_reserve(track_entity, requested_capacity);
        if (reserve_result != PD_CORE_RESULT_OK) {
            return reserve_result;
        }
    }

    memcpy(&track_entity->keyframes[track_entity->keyframe_count], keyframe_entity, sizeof(PdAnimationPrsKeyframeEntity));
    track_entity->keyframe_count++;
    return PD_CORE_RESULT_OK;
}
