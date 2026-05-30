#ifndef PRISMDRAFT_ANIMATION_PD_ANIMATION_TIMELINE_CONTROLLER_H
#define PRISMDRAFT_ANIMATION_PD_ANIMATION_TIMELINE_CONTROLLER_H

#include "prismdraft/animation/pd_animation_prs_entity.h"
#include "prismdraft/core/pd_core_result_entity.h"

typedef struct PdAnimationTimelineController {
    float time_seconds;
    float duration_seconds;
    float playback_speed;
    int is_playing;
    int loop_enabled;
} PdAnimationTimelineController;

PdCoreResult pd_animation_timeline_controller_init(
    PdAnimationTimelineController* timeline_controller,
    float duration_seconds);

void pd_animation_timeline_controller_play(PdAnimationTimelineController* timeline_controller);

void pd_animation_timeline_controller_pause(PdAnimationTimelineController* timeline_controller);

void pd_animation_timeline_controller_stop(PdAnimationTimelineController* timeline_controller);

void pd_animation_timeline_controller_set_loop_enabled(
    PdAnimationTimelineController* timeline_controller,
    int loop_enabled);

PdCoreResult pd_animation_timeline_controller_set_playback_speed(
    PdAnimationTimelineController* timeline_controller,
    float playback_speed);

PdCoreResult pd_animation_timeline_controller_update(
    PdAnimationTimelineController* timeline_controller,
    float delta_seconds);

PdCoreResult pd_animation_timeline_controller_sample_prs_track(
    const PdAnimationTimelineController* timeline_controller,
    const PdAnimationPrsTrackEntity* track_entity,
    PdAnimationPrsPoseEntity* pose_entity);

#endif
