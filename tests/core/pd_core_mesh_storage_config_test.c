#include "prismdraft/core/pd_core_mesh_storage_config.h"

#include <assert.h>

static void pd_core_mesh_storage_config_test_local_default_has_room_for_basic_fixture(void)
{
    PdCoreMeshStorageConfig config = pd_core_mesh_storage_config_default();

    assert(config.vertex_capacity >= 8u);
    assert(config.half_edge_capacity >= 24u);
    assert(config.face_capacity >= 6u);
}

static void pd_core_mesh_storage_config_test_local_half_edges_have_extra_growth_room(void)
{
    PdCoreMeshStorageConfig config = pd_core_mesh_storage_config_default();

    assert(config.half_edge_capacity >= config.vertex_capacity * 3u);
}

int main(void)
{
    pd_core_mesh_storage_config_test_local_default_has_room_for_basic_fixture();
    pd_core_mesh_storage_config_test_local_half_edges_have_extra_growth_room();
    return 0;
}
