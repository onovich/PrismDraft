#include "prismdraft/core/pd_core_mesh_storage_controller.h"

#include <assert.h>

static void pd_core_mesh_storage_controller_test_local_init_allocates_default_storage(void)
{
    PdCoreMeshEntity mesh_entity;
    PdCoreMeshStorageConfig config = pd_core_mesh_storage_config_default();

    assert(pd_core_mesh_storage_controller_init(&mesh_entity, config) == PD_CORE_RESULT_OK);
    assert(mesh_entity.vertices != NULL);
    assert(mesh_entity.half_edges != NULL);
    assert(mesh_entity.faces != NULL);
    assert(mesh_entity.vertex_count == 0u);
    assert(mesh_entity.vertex_capacity == config.vertex_capacity);
    assert(mesh_entity.half_edge_count == 0u);
    assert(mesh_entity.half_edge_capacity == config.half_edge_capacity);
    assert(mesh_entity.face_count == 0u);
    assert(mesh_entity.face_capacity == config.face_capacity);

    pd_core_mesh_storage_controller_free(&mesh_entity);
}

static void pd_core_mesh_storage_controller_test_local_free_clears_storage(void)
{
    PdCoreMeshEntity mesh_entity;

    assert(pd_core_mesh_storage_controller_init(&mesh_entity, pd_core_mesh_storage_config_default()) == PD_CORE_RESULT_OK);
    pd_core_mesh_storage_controller_free(&mesh_entity);

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

static void pd_core_mesh_storage_controller_test_local_reserve_preserves_vertex_data(void)
{
    PdCoreMeshEntity mesh_entity;

    assert(pd_core_mesh_storage_controller_init(&mesh_entity, pd_core_mesh_storage_config_default()) == PD_CORE_RESULT_OK);
    mesh_entity.vertices[0].position[0] = 7.0f;
    mesh_entity.vertex_count = 1u;

    assert(pd_core_mesh_storage_controller_reserve_vertices(&mesh_entity, mesh_entity.vertex_capacity + 8u) == PD_CORE_RESULT_OK);
    assert(mesh_entity.vertex_count == 1u);
    assert(mesh_entity.vertices[0].position[0] == 7.0f);

    pd_core_mesh_storage_controller_free(&mesh_entity);
}

static void pd_core_mesh_storage_controller_test_local_rejects_null_mesh(void)
{
    assert(pd_core_mesh_storage_controller_init(NULL, pd_core_mesh_storage_config_default()) == PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);
    assert(pd_core_mesh_storage_controller_reserve_vertices(NULL, 1u) == PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);
    assert(pd_core_mesh_storage_controller_reserve_half_edges(NULL, 1u) == PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);
    assert(pd_core_mesh_storage_controller_reserve_faces(NULL, 1u) == PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);
    pd_core_mesh_storage_controller_free(NULL);
}

int main(void)
{
    pd_core_mesh_storage_controller_test_local_init_allocates_default_storage();
    pd_core_mesh_storage_controller_test_local_free_clears_storage();
    pd_core_mesh_storage_controller_test_local_reserve_preserves_vertex_data();
    pd_core_mesh_storage_controller_test_local_rejects_null_mesh();
    return 0;
}
