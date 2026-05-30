#ifndef PRISMDRAFT_RENDER_PD_RENDER_FACE_HIGHLIGHT_BUFFER_H
#define PRISMDRAFT_RENDER_PD_RENDER_FACE_HIGHLIGHT_BUFFER_H

#include "prismdraft/core/pd_core_mesh_entity.h"
#include "prismdraft/core/pd_core_result_entity.h"
#include "prismdraft/render/pd_render_face_highlight_config.h"
#include "prismdraft/render/pd_render_mesh_buffer.h"

PdCoreResult pd_render_face_highlight_buffer_build_for_face(
    PdRenderMeshBuffer* render_mesh_buffer,
    const PdCoreMeshEntity* mesh_entity,
    uint32_t face_index,
    PdRenderFaceHighlightConfig config);

#endif
