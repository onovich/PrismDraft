#ifndef PRISMDRAFT_EDITOR_PD_EDITOR_PANEL_STATE_H
#define PRISMDRAFT_EDITOR_PD_EDITOR_PANEL_STATE_H

#include "prismdraft/core/pd_core_result_entity.h"

typedef enum PdEditorPanelKind {
    PD_EDITOR_PANEL_KIND_MODELING,
    PD_EDITOR_PANEL_KIND_TRANSFORM,
    PD_EDITOR_PANEL_KIND_VISUAL,
    PD_EDITOR_PANEL_KIND_LIGHTING
} PdEditorPanelKind;

typedef struct PdEditorPanelState {
    PdEditorPanelKind active_panel;
    int is_open;
} PdEditorPanelState;

PdCoreResult pd_editor_panel_state_init(PdEditorPanelState* panel_state);

PdCoreResult pd_editor_panel_state_set_active(PdEditorPanelState* panel_state, PdEditorPanelKind panel_kind);

void pd_editor_panel_state_toggle(PdEditorPanelState* panel_state);

const char* pd_editor_panel_state_get_name(PdEditorPanelKind panel_kind);

#endif
