#ifndef PRISMDRAFT_CORE_PD_CORE_MESH_STORAGE_CONFIG_H
#define PRISMDRAFT_CORE_PD_CORE_MESH_STORAGE_CONFIG_H

#include <stdint.h>

#define PD_CORE_MESH_STORAGE_CONFIG_DEFAULT_VERTEX_CAPACITY 64u
#define PD_CORE_MESH_STORAGE_CONFIG_DEFAULT_HALF_EDGE_CAPACITY 192u
#define PD_CORE_MESH_STORAGE_CONFIG_DEFAULT_FACE_CAPACITY 64u

typedef struct PdCoreMeshStorageConfig {
    uint32_t vertex_capacity;
    uint32_t half_edge_capacity;
    uint32_t face_capacity;
} PdCoreMeshStorageConfig;

static inline PdCoreMeshStorageConfig pd_core_mesh_storage_config_default(void)
{
    PdCoreMeshStorageConfig config;
    config.vertex_capacity = PD_CORE_MESH_STORAGE_CONFIG_DEFAULT_VERTEX_CAPACITY;
    config.half_edge_capacity = PD_CORE_MESH_STORAGE_CONFIG_DEFAULT_HALF_EDGE_CAPACITY;
    config.face_capacity = PD_CORE_MESH_STORAGE_CONFIG_DEFAULT_FACE_CAPACITY;
    return config;
}

#endif
