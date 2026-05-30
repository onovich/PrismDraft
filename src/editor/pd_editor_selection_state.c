#include "prismdraft/editor/pd_editor_selection_state.h"

#include "prismdraft/core/pd_core_mesh_entity.h"

static void pd_editor_selection_state_local_set_none(PdEditorSelectionState* selection_state)
{
    selection_state->kind = PD_EDITOR_SELECTION_KIND_NONE;
    selection_state->primary_index = PD_CORE_MESH_ENTITY_INVALID_INDEX;
    selection_state->selected_vertex_count = 0u;
    selection_state->selected_edge_count = 0u;
    selection_state->selected_face_count = 0u;
}

PdCoreResult pd_editor_selection_state_init(PdEditorSelectionState* selection_state)
{
    if (selection_state == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    pd_editor_selection_state_local_set_none(selection_state);
    return PD_CORE_RESULT_OK;
}

void pd_editor_selection_state_clear(PdEditorSelectionState* selection_state)
{
    if (selection_state == 0) {
        return;
    }

    pd_editor_selection_state_local_set_none(selection_state);
}

PdCoreResult pd_editor_selection_state_select_vertex(PdEditorSelectionState* selection_state, uint32_t vertex_index)
{
    if (selection_state == 0 || vertex_index == PD_CORE_MESH_ENTITY_INVALID_INDEX) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    selection_state->kind = PD_EDITOR_SELECTION_KIND_VERTEX;
    selection_state->primary_index = vertex_index;
    selection_state->selected_vertex_count = 1u;
    selection_state->selected_edge_count = 0u;
    selection_state->selected_face_count = 0u;

    return PD_CORE_RESULT_OK;
}

PdCoreResult pd_editor_selection_state_select_edge(PdEditorSelectionState* selection_state, uint32_t edge_index)
{
    if (selection_state == 0 || edge_index == PD_CORE_MESH_ENTITY_INVALID_INDEX) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    selection_state->kind = PD_EDITOR_SELECTION_KIND_EDGE;
    selection_state->primary_index = edge_index;
    selection_state->selected_vertex_count = 0u;
    selection_state->selected_edge_count = 1u;
    selection_state->selected_face_count = 0u;

    return PD_CORE_RESULT_OK;
}

PdCoreResult pd_editor_selection_state_select_face(PdEditorSelectionState* selection_state, uint32_t face_index)
{
    if (selection_state == 0 || face_index == PD_CORE_MESH_ENTITY_INVALID_INDEX) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    selection_state->kind = PD_EDITOR_SELECTION_KIND_FACE;
    selection_state->primary_index = face_index;
    selection_state->selected_vertex_count = 0u;
    selection_state->selected_edge_count = 0u;
    selection_state->selected_face_count = 1u;

    return PD_CORE_RESULT_OK;
}

int pd_editor_selection_state_has_selection(const PdEditorSelectionState* selection_state)
{
    if (selection_state == 0) {
        return 0;
    }

    return selection_state->kind != PD_EDITOR_SELECTION_KIND_NONE &&
           selection_state->primary_index != PD_CORE_MESH_ENTITY_INVALID_INDEX;
}
