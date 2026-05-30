#include "prismdraft/core/pd_core_mesh_storage_controller.h"
#include "prismdraft/core/pd_core_mesh_validator.h"

#include <assert.h>

static void pd_core_mesh_validator_test_local_make_triangle(PdCoreMeshEntity* mesh_entity)
{
    PdCoreMeshStorageConfig config;

    config.vertex_capacity = 3u;
    config.half_edge_capacity = 3u;
    config.face_capacity = 1u;

    assert(pd_core_mesh_storage_controller_init(mesh_entity, config) == PD_CORE_RESULT_OK);

    mesh_entity->vertex_count = 3u;
    mesh_entity->half_edge_count = 3u;
    mesh_entity->face_count = 1u;

    mesh_entity->vertices[0].half_edge = 0u;
    mesh_entity->vertices[1].half_edge = 1u;
    mesh_entity->vertices[2].half_edge = 2u;

    mesh_entity->half_edges[0].next_half_edge = 1u;
    mesh_entity->half_edges[0].pair_half_edge = PD_CORE_MESH_ENTITY_INVALID_INDEX;
    mesh_entity->half_edges[0].vertex = 0u;
    mesh_entity->half_edges[0].face = 0u;

    mesh_entity->half_edges[1].next_half_edge = 2u;
    mesh_entity->half_edges[1].pair_half_edge = PD_CORE_MESH_ENTITY_INVALID_INDEX;
    mesh_entity->half_edges[1].vertex = 1u;
    mesh_entity->half_edges[1].face = 0u;

    mesh_entity->half_edges[2].next_half_edge = 0u;
    mesh_entity->half_edges[2].pair_half_edge = PD_CORE_MESH_ENTITY_INVALID_INDEX;
    mesh_entity->half_edges[2].vertex = 2u;
    mesh_entity->half_edges[2].face = 0u;

    mesh_entity->faces[0].half_edge = 0u;
}

static void pd_core_mesh_validator_test_local_accepts_empty_mesh(void)
{
    PdCoreMeshEntity mesh_entity = { 0 };

    assert(pd_core_mesh_validator_check(&mesh_entity) == PD_CORE_RESULT_OK);
}

static void pd_core_mesh_validator_test_local_accepts_valid_triangle_loop(void)
{
    PdCoreMeshEntity mesh_entity;

    pd_core_mesh_validator_test_local_make_triangle(&mesh_entity);
    assert(pd_core_mesh_validator_check(&mesh_entity) == PD_CORE_RESULT_OK);

    pd_core_mesh_storage_controller_free(&mesh_entity);
}

static void pd_core_mesh_validator_test_local_rejects_null_mesh(void)
{
    assert(pd_core_mesh_validator_check(NULL) == PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);
}

static void pd_core_mesh_validator_test_local_rejects_count_over_capacity(void)
{
    PdCoreMeshEntity mesh_entity = { 0 };

    mesh_entity.vertex_count = 1u;
    mesh_entity.vertex_capacity = 0u;

    assert(pd_core_mesh_validator_check(&mesh_entity) == PD_CORE_RESULT_ERROR_TOPOLOGY_INVALID);
}

static void pd_core_mesh_validator_test_local_rejects_broken_pair_symmetry(void)
{
    PdCoreMeshEntity mesh_entity;

    pd_core_mesh_validator_test_local_make_triangle(&mesh_entity);
    mesh_entity.half_edges[0].pair_half_edge = 1u;
    mesh_entity.half_edges[1].pair_half_edge = PD_CORE_MESH_ENTITY_INVALID_INDEX;

    assert(pd_core_mesh_validator_check(&mesh_entity) == PD_CORE_RESULT_ERROR_TOPOLOGY_INVALID);

    pd_core_mesh_storage_controller_free(&mesh_entity);
}

static void pd_core_mesh_validator_test_local_rejects_open_face_loop(void)
{
    PdCoreMeshEntity mesh_entity;

    pd_core_mesh_validator_test_local_make_triangle(&mesh_entity);
    mesh_entity.half_edges[2].next_half_edge = 1u;

    assert(pd_core_mesh_validator_check(&mesh_entity) == PD_CORE_RESULT_ERROR_TOPOLOGY_INVALID);

    pd_core_mesh_storage_controller_free(&mesh_entity);
}

int main(void)
{
    pd_core_mesh_validator_test_local_accepts_empty_mesh();
    pd_core_mesh_validator_test_local_accepts_valid_triangle_loop();
    pd_core_mesh_validator_test_local_rejects_null_mesh();
    pd_core_mesh_validator_test_local_rejects_count_over_capacity();
    pd_core_mesh_validator_test_local_rejects_broken_pair_symmetry();
    pd_core_mesh_validator_test_local_rejects_open_face_loop();
    return 0;
}
