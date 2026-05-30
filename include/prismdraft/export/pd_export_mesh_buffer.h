#ifndef PRISMDRAFT_EXPORT_PD_EXPORT_MESH_BUFFER_H
#define PRISMDRAFT_EXPORT_PD_EXPORT_MESH_BUFFER_H

#include "prismdraft/core/pd_core_mesh_entity.h"
#include "prismdraft/core/pd_core_result_entity.h"

#include <stdint.h>

typedef struct PdExportMeshBufferVertex {
    float position[3];
    float normal[3];
    float color[4];
} PdExportMeshBufferVertex;

typedef struct PdExportMeshBuffer {
    PdExportMeshBufferVertex* vertices;
    uint32_t vertex_count;
    uint32_t vertex_capacity;
} PdExportMeshBuffer;

void pd_export_mesh_buffer_free(PdExportMeshBuffer* export_mesh_buffer);

PdCoreResult pd_export_mesh_buffer_build_from_mesh(
    PdExportMeshBuffer* export_mesh_buffer,
    const PdCoreMeshEntity* mesh_entity);

#endif
