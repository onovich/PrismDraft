#ifndef PRISMDRAFT_RENDER_PD_RENDER_MESH_BUFFER_H
#define PRISMDRAFT_RENDER_PD_RENDER_MESH_BUFFER_H

#include <stdint.h>

#include "prismdraft/core/pd_core_mesh_entity.h"
#include "prismdraft/core/pd_core_result_entity.h"

typedef struct PdRenderMeshBufferVertex {
    float position[3];
    float normal[3];
    uint8_t color[4];
} PdRenderMeshBufferVertex;

typedef struct PdRenderMeshBuffer {
    PdRenderMeshBufferVertex* vertices;
    uint32_t vertex_count;
    uint32_t vertex_capacity;
} PdRenderMeshBuffer;

void pd_render_mesh_buffer_free(PdRenderMeshBuffer* render_mesh_buffer);

PdCoreResult pd_render_mesh_buffer_build_from_mesh(
    PdRenderMeshBuffer* render_mesh_buffer,
    const PdCoreMeshEntity* mesh_entity);

#endif
