#ifndef PRISMDRAFT_EDITOR_PD_EDITOR_SELECTION_STATE_H
#define PRISMDRAFT_EDITOR_PD_EDITOR_SELECTION_STATE_H

#include "prismdraft/core/pd_core_result_entity.h"

#include <stdint.h>

typedef enum PdEditorSelectionKind {
    PD_EDITOR_SELECTION_KIND_NONE = 0,
    PD_EDITOR_SELECTION_KIND_VERTEX,
    PD_EDITOR_SELECTION_KIND_EDGE,
    PD_EDITOR_SELECTION_KIND_FACE
} PdEditorSelectionKind;

typedef struct PdEditorSelectionState {
    PdEditorSelectionKind kind;
    uint32_t primary_index;
    uint32_t selected_vertex_count;
    uint32_t selected_edge_count;
    uint32_t selected_face_count;
} PdEditorSelectionState;

PdCoreResult pd_editor_selection_state_init(PdEditorSelectionState* selection_state);

void pd_editor_selection_state_clear(PdEditorSelectionState* selection_state);

PdCoreResult pd_editor_selection_state_select_vertex(PdEditorSelectionState* selection_state, uint32_t vertex_index);

PdCoreResult pd_editor_selection_state_select_edge(PdEditorSelectionState* selection_state, uint32_t edge_index);

PdCoreResult pd_editor_selection_state_select_face(PdEditorSelectionState* selection_state, uint32_t face_index);

int pd_editor_selection_state_has_selection(const PdEditorSelectionState* selection_state);

#endif
