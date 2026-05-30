#ifndef PRISMDRAFT_EDITOR_PD_EDITOR_TOOL_STATE_H
#define PRISMDRAFT_EDITOR_PD_EDITOR_TOOL_STATE_H

#include "prismdraft/core/pd_core_result_entity.h"

typedef enum PdEditorToolKind {
    PD_EDITOR_TOOL_KIND_VIEW,
    PD_EDITOR_TOOL_KIND_TRANSFORM,
    PD_EDITOR_TOOL_KIND_COLOR,
    PD_EDITOR_TOOL_KIND_INSET,
    PD_EDITOR_TOOL_KIND_EXTRUDE,
    PD_EDITOR_TOOL_KIND_BEVEL,
    PD_EDITOR_TOOL_KIND_LOOP_CUT
} PdEditorToolKind;

typedef struct PdEditorToolState {
    PdEditorToolKind active_tool;
    PdCoreResult last_result;
} PdEditorToolState;

PdCoreResult pd_editor_tool_state_init(PdEditorToolState* tool_state);

PdCoreResult pd_editor_tool_state_set_active(PdEditorToolState* tool_state, PdEditorToolKind tool_kind);

const char* pd_editor_tool_state_get_name(PdEditorToolKind tool_kind);

#endif
