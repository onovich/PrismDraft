#ifndef PRISMDRAFT_CORE_PD_CORE_MESH_ENTITY_H
#define PRISMDRAFT_CORE_PD_CORE_MESH_ENTITY_H

#include <stdint.h>

#define PD_CORE_MESH_ENTITY_INVALID_INDEX UINT32_MAX

typedef struct PdCoreVertexEntity {
    float position[3];
    float normal[3];
    uint8_t color[4];
    uint32_t half_edge;
} PdCoreVertexEntity;

typedef struct PdCoreHalfEdgeEntity {
    uint32_t next_half_edge;
    uint32_t pair_half_edge;
    uint32_t vertex;
    uint32_t face;
} PdCoreHalfEdgeEntity;

typedef struct PdCoreFaceEntity {
    uint32_t half_edge;
    float face_normal[3];
    uint8_t base_color[4];
} PdCoreFaceEntity;

typedef struct PdCoreMeshEntity {
    PdCoreVertexEntity* vertices;
    PdCoreHalfEdgeEntity* half_edges;
    PdCoreFaceEntity* faces;

    uint32_t vertex_count;
    uint32_t vertex_capacity;

    uint32_t half_edge_count;
    uint32_t half_edge_capacity;

    uint32_t face_count;
    uint32_t face_capacity;
} PdCoreMeshEntity;

#endif
