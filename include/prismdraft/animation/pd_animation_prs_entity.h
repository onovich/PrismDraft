#ifndef PRISMDRAFT_ANIMATION_PD_ANIMATION_PRS_ENTITY_H
#define PRISMDRAFT_ANIMATION_PD_ANIMATION_PRS_ENTITY_H

#include "prismdraft/core/pd_core_result_entity.h"

#include <stdint.h>

#define PD_ANIMATION_PRS_ENTITY_INVALID_OBJECT_INDEX UINT32_MAX

typedef struct PdAnimationPrsPoseEntity {
    float position[3];
    float rotation[4];
    float scale[3];
} PdAnimationPrsPoseEntity;

typedef struct PdAnimationPrsKeyframeEntity {
    float time_seconds;
    PdAnimationPrsPoseEntity pose;
} PdAnimationPrsKeyframeEntity;

typedef struct PdAnimationPrsTrackEntity {
    uint32_t object_index;
    PdAnimationPrsKeyframeEntity* keyframes;
    uint32_t keyframe_count;
    uint32_t keyframe_capacity;
} PdAnimationPrsTrackEntity;

void pd_animation_prs_entity_pose_set_identity(PdAnimationPrsPoseEntity* pose_entity);

PdCoreResult pd_animation_prs_entity_track_init(
    PdAnimationPrsTrackEntity* track_entity,
    uint32_t object_index);

void pd_animation_prs_entity_track_free(PdAnimationPrsTrackEntity* track_entity);

PdCoreResult pd_animation_prs_entity_track_reserve(
    PdAnimationPrsTrackEntity* track_entity,
    uint32_t keyframe_capacity);

PdCoreResult pd_animation_prs_entity_track_append_keyframe(
    PdAnimationPrsTrackEntity* track_entity,
    const PdAnimationPrsKeyframeEntity* keyframe_entity);

#endif
