#include "prismdraft/editor/pd_editor_panel_state.h"

#include <assert.h>

static void pd_editor_panel_state_test_local_initializes_default_panel(void)
{
    PdEditorPanelState panel_state;

    assert(pd_editor_panel_state_init(&panel_state) == PD_CORE_RESULT_OK);
    assert(panel_state.active_panel == PD_EDITOR_PANEL_KIND_MODELING);
    assert(panel_state.is_open);
}

static void pd_editor_panel_state_test_local_sets_active_panel(void)
{
    PdEditorPanelState panel_state;

    assert(pd_editor_panel_state_init(&panel_state) == PD_CORE_RESULT_OK);
    assert(pd_editor_panel_state_set_active(&panel_state, PD_EDITOR_PANEL_KIND_LIGHTING) == PD_CORE_RESULT_OK);
    assert(panel_state.active_panel == PD_EDITOR_PANEL_KIND_LIGHTING);
    assert(panel_state.is_open);
    assert(pd_editor_panel_state_set_active(&panel_state, PD_EDITOR_PANEL_KIND_VIEW) == PD_CORE_RESULT_OK);
    assert(panel_state.active_panel == PD_EDITOR_PANEL_KIND_VIEW);
}

static void pd_editor_panel_state_test_local_toggles_panel(void)
{
    PdEditorPanelState panel_state;

    assert(pd_editor_panel_state_init(&panel_state) == PD_CORE_RESULT_OK);
    pd_editor_panel_state_toggle(&panel_state);
    assert(!panel_state.is_open);
    pd_editor_panel_state_toggle(&panel_state);
    assert(panel_state.is_open);
}

static void pd_editor_panel_state_test_local_rejects_invalid_arguments(void)
{
    PdEditorPanelState panel_state;

    assert(pd_editor_panel_state_init(0) == PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);
    assert(pd_editor_panel_state_set_active(0, PD_EDITOR_PANEL_KIND_MODELING) ==
           PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);
    assert(pd_editor_panel_state_init(&panel_state) == PD_CORE_RESULT_OK);
    assert(pd_editor_panel_state_set_active(&panel_state, (PdEditorPanelKind)99) ==
           PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);
}

static void pd_editor_panel_state_test_local_names_panels(void)
{
    assert(pd_editor_panel_state_get_name(PD_EDITOR_PANEL_KIND_MODELING) != 0);
    assert(pd_editor_panel_state_get_name(PD_EDITOR_PANEL_KIND_TRANSFORM) != 0);
    assert(pd_editor_panel_state_get_name(PD_EDITOR_PANEL_KIND_VISUAL) != 0);
    assert(pd_editor_panel_state_get_name(PD_EDITOR_PANEL_KIND_LIGHTING) != 0);
    assert(pd_editor_panel_state_get_name(PD_EDITOR_PANEL_KIND_VIEW) != 0);
    assert(pd_editor_panel_state_get_name((PdEditorPanelKind)99) != 0);
}

int main(void)
{
    pd_editor_panel_state_test_local_initializes_default_panel();
    pd_editor_panel_state_test_local_sets_active_panel();
    pd_editor_panel_state_test_local_toggles_panel();
    pd_editor_panel_state_test_local_rejects_invalid_arguments();
    pd_editor_panel_state_test_local_names_panels();
    return 0;
}
