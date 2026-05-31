#ifndef PRISMDRAFT_APP_PD_APP_CONTEXT_ENTITY_H
#define PRISMDRAFT_APP_PD_APP_CONTEXT_ENTITY_H

#include "prismdraft/editor/pd_editor_panel_state.h"
#include "prismdraft/editor/pd_editor_scene_state.h"
#include "prismdraft/editor/pd_editor_selection_state.h"
#include "prismdraft/editor/pd_editor_tool_state.h"
#include "prismdraft/editor/pd_editor_visual_state.h"

typedef struct PdAppContextEntity {
    PdEditorPanelState panel_state;
    PdEditorSceneState scene_state;
    PdEditorSelectionState selection_state;
    PdEditorToolState tool_state;
    PdEditorVisualState visual_state;
    int is_running;
} PdAppContextEntity;

#endif
