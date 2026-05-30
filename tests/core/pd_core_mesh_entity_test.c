#include "prismdraft/core/pd_core_mesh_entity.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

static void pd_core_mesh_entity_test_local_exposes_invalid_index(void)
{
    assert(PD_CORE_MESH_ENTITY_INVALID_INDEX == UINT32_MAX);
}

static void pd_core_mesh_entity_test_local_supports_count_capacity_ownership(void)
{
    PdCoreMeshEntity mesh_entity = { 0 };

    assert(mesh_entity.vertices == NULL);
    assert(mesh_entity.half_edges == NULL);
    assert(mesh_entity.faces == NULL);
    assert(mesh_entity.vertex_count == 0u);
    assert(mesh_entity.vertex_capacity == 0u);
    assert(mesh_entity.half_edge_count == 0u);
    assert(mesh_entity.half_edge_capacity == 0u);
    assert(mesh_entity.face_count == 0u);
    assert(mesh_entity.face_capacity == 0u);
}

static void pd_core_mesh_entity_test_local_preserves_expected_index_width(void)
{
    assert(sizeof(((PdCoreVertexEntity*)0)->half_edge) == sizeof(uint32_t));
    assert(sizeof(((PdCoreHalfEdgeEntity*)0)->next_half_edge) == sizeof(uint32_t));
    assert(sizeof(((PdCoreHalfEdgeEntity*)0)->pair_half_edge) == sizeof(uint32_t));
    assert(sizeof(((PdCoreHalfEdgeEntity*)0)->vertex) == sizeof(uint32_t));
    assert(sizeof(((PdCoreHalfEdgeEntity*)0)->face) == sizeof(uint32_t));
    assert(sizeof(((PdCoreFaceEntity*)0)->half_edge) == sizeof(uint32_t));
}

int main(void)
{
    pd_core_mesh_entity_test_local_exposes_invalid_index();
    pd_core_mesh_entity_test_local_supports_count_capacity_ownership();
    pd_core_mesh_entity_test_local_preserves_expected_index_width();
    return 0;
}
