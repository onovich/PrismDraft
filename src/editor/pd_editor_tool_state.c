#include "prismdraft/editor/pd_editor_tool_state.h"

PdCoreResult pd_editor_tool_state_init(PdEditorToolState* tool_state)
{
    if (tool_state == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    tool_state->active_tool = PD_EDITOR_TOOL_KIND_VIEW;
    tool_state->last_result = PD_CORE_RESULT_OK;
    return PD_CORE_RESULT_OK;
}

PdCoreResult pd_editor_tool_state_set_active(PdEditorToolState* tool_state, PdEditorToolKind tool_kind)
{
    if (tool_state == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (tool_kind < PD_EDITOR_TOOL_KIND_VIEW || tool_kind > PD_EDITOR_TOOL_KIND_LOOP_CUT) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    tool_state->active_tool = tool_kind;
    tool_state->last_result = PD_CORE_RESULT_OK;
    return PD_CORE_RESULT_OK;
}

const char* pd_editor_tool_state_get_name(PdEditorToolKind tool_kind)
{
    switch (tool_kind) {
        case PD_EDITOR_TOOL_KIND_VIEW:
            return "view";
        case PD_EDITOR_TOOL_KIND_TRANSFORM:
            return "transform";
        case PD_EDITOR_TOOL_KIND_COLOR:
            return "color";
        case PD_EDITOR_TOOL_KIND_INSET:
            return "inset";
        case PD_EDITOR_TOOL_KIND_EXTRUDE:
            return "extrude";
        case PD_EDITOR_TOOL_KIND_BEVEL:
            return "bevel";
        case PD_EDITOR_TOOL_KIND_LOOP_CUT:
            return "loop_cut";
        default:
            return "unknown";
    }
}
