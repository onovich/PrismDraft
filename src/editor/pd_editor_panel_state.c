#include "prismdraft/editor/pd_editor_panel_state.h"

PdCoreResult pd_editor_panel_state_init(PdEditorPanelState* panel_state)
{
    if (panel_state == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    panel_state->active_panel = PD_EDITOR_PANEL_KIND_MODELING;
    panel_state->is_open = 1;
    return PD_CORE_RESULT_OK;
}

PdCoreResult pd_editor_panel_state_set_active(PdEditorPanelState* panel_state, PdEditorPanelKind panel_kind)
{
    if (panel_state == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (panel_kind < PD_EDITOR_PANEL_KIND_MODELING || panel_kind > PD_EDITOR_PANEL_KIND_VIEW) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    panel_state->active_panel = panel_kind;
    panel_state->is_open = 1;
    return PD_CORE_RESULT_OK;
}

void pd_editor_panel_state_toggle(PdEditorPanelState* panel_state)
{
    if (panel_state == 0) {
        return;
    }

    panel_state->is_open = !panel_state->is_open;
}

const char* pd_editor_panel_state_get_name(PdEditorPanelKind panel_kind)
{
    switch (panel_kind) {
        case PD_EDITOR_PANEL_KIND_MODELING:
            return "Model";
        case PD_EDITOR_PANEL_KIND_TRANSFORM:
            return "Transform";
        case PD_EDITOR_PANEL_KIND_VISUAL:
            return "Visual";
        case PD_EDITOR_PANEL_KIND_LIGHTING:
            return "Light";
        case PD_EDITOR_PANEL_KIND_VIEW:
            return "View";
        default:
            return "Unknown";
    }
}
