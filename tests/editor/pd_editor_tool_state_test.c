#include "prismdraft/editor/pd_editor_tool_state.h"

#include <assert.h>
#include <string.h>

static void pd_editor_tool_state_test_local_initializes_view_tool(void)
{
    PdEditorToolState tool_state;

    assert(pd_editor_tool_state_init(0) == PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);
    assert(pd_editor_tool_state_init(&tool_state) == PD_CORE_RESULT_OK);
    assert(tool_state.active_tool == PD_EDITOR_TOOL_KIND_VIEW);
    assert(tool_state.last_result == PD_CORE_RESULT_OK);
    assert(strcmp(pd_editor_tool_state_get_name(tool_state.active_tool), "view") == 0);
}

static void pd_editor_tool_state_test_local_sets_active_tool(void)
{
    PdEditorToolState tool_state;

    assert(pd_editor_tool_state_init(&tool_state) == PD_CORE_RESULT_OK);
    assert(pd_editor_tool_state_set_active(&tool_state, PD_EDITOR_TOOL_KIND_EXTRUDE) == PD_CORE_RESULT_OK);
    assert(tool_state.active_tool == PD_EDITOR_TOOL_KIND_EXTRUDE);
    assert(strcmp(pd_editor_tool_state_get_name(tool_state.active_tool), "extrude") == 0);
    assert(pd_editor_tool_state_set_active(0, PD_EDITOR_TOOL_KIND_EXTRUDE) == PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);
}

int main(void)
{
    pd_editor_tool_state_test_local_initializes_view_tool();
    pd_editor_tool_state_test_local_sets_active_tool();
    return 0;
}
