#include "prismdraft/editor/pd_editor_selection_state.h"

#include "prismdraft/core/pd_core_mesh_entity.h"

#include <assert.h>

int main(void)
{
    PdEditorSelectionState selection_state;

    assert(pd_editor_selection_state_init(0) == PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);
    assert(pd_editor_selection_state_init(&selection_state) == PD_CORE_RESULT_OK);
    assert(selection_state.kind == PD_EDITOR_SELECTION_KIND_NONE);
    assert(selection_state.primary_index == PD_CORE_MESH_ENTITY_INVALID_INDEX);
    assert(!pd_editor_selection_state_has_selection(&selection_state));

    assert(pd_editor_selection_state_select_vertex(&selection_state, 3u) == PD_CORE_RESULT_OK);
    assert(selection_state.kind == PD_EDITOR_SELECTION_KIND_VERTEX);
    assert(selection_state.primary_index == 3u);
    assert(selection_state.selected_vertex_count == 1u);
    assert(selection_state.selected_edge_count == 0u);
    assert(selection_state.selected_face_count == 0u);
    assert(pd_editor_selection_state_has_selection(&selection_state));

    assert(pd_editor_selection_state_select_edge(&selection_state, 5u) == PD_CORE_RESULT_OK);
    assert(selection_state.kind == PD_EDITOR_SELECTION_KIND_EDGE);
    assert(selection_state.primary_index == 5u);
    assert(selection_state.selected_vertex_count == 0u);
    assert(selection_state.selected_edge_count == 1u);
    assert(selection_state.selected_face_count == 0u);

    assert(pd_editor_selection_state_select_face(&selection_state, 2u) == PD_CORE_RESULT_OK);
    assert(selection_state.kind == PD_EDITOR_SELECTION_KIND_FACE);
    assert(selection_state.primary_index == 2u);
    assert(selection_state.selected_vertex_count == 0u);
    assert(selection_state.selected_edge_count == 0u);
    assert(selection_state.selected_face_count == 1u);

    assert(pd_editor_selection_state_select_face(&selection_state, PD_CORE_MESH_ENTITY_INVALID_INDEX) ==
           PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);

    pd_editor_selection_state_clear(&selection_state);
    assert(selection_state.kind == PD_EDITOR_SELECTION_KIND_NONE);
    assert(selection_state.primary_index == PD_CORE_MESH_ENTITY_INVALID_INDEX);
    assert(!pd_editor_selection_state_has_selection(&selection_state));

    return 0;
}
