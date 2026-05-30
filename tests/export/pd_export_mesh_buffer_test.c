#include "prismdraft/export/pd_export_mesh_buffer.h"

#include "prismdraft/core/pd_core_cube_fixture.h"
#include "prismdraft/core/pd_core_mesh_storage_controller.h"

#include <assert.h>

static int pd_export_mesh_buffer_test_local_near(float left, float right)
{
    float difference = left - right;
    if (difference < 0.0f) {
        difference = -difference;
    }

    return difference < 0.0001f;
}

static void pd_export_mesh_buffer_test_local_flattens_cube_for_export(void)
{
    PdCoreMeshEntity mesh_entity = { 0 };
    PdExportMeshBuffer export_mesh_buffer = { 0 };

    assert(pd_core_cube_fixture_build(&mesh_entity) == PD_CORE_RESULT_OK);
    assert(pd_export_mesh_buffer_build_from_mesh(&export_mesh_buffer, &mesh_entity) == PD_CORE_RESULT_OK);

    assert(export_mesh_buffer.vertices != 0);
    assert(export_mesh_buffer.vertex_count == 36u);
    assert(export_mesh_buffer.vertex_capacity >= export_mesh_buffer.vertex_count);
    assert(export_mesh_buffer.vertices[0].normal[2] == mesh_entity.faces[0].face_normal[2]);
    assert(pd_export_mesh_buffer_test_local_near(
        export_mesh_buffer.vertices[0].color[0],
        (float)mesh_entity.faces[0].base_color[0] / 255.0f));
    assert(pd_export_mesh_buffer_test_local_near(export_mesh_buffer.vertices[0].color[3], 1.0f));

    pd_export_mesh_buffer_free(&export_mesh_buffer);
    pd_core_mesh_storage_controller_free(&mesh_entity);
}

static void pd_export_mesh_buffer_test_local_rejects_invalid_arguments(void)
{
    PdCoreMeshEntity mesh_entity = { 0 };
    PdExportMeshBuffer export_mesh_buffer = { 0 };

    assert(pd_export_mesh_buffer_build_from_mesh(0, &mesh_entity) == PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);
    assert(pd_export_mesh_buffer_build_from_mesh(&export_mesh_buffer, 0) == PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);
    pd_export_mesh_buffer_free(0);
}

int main(void)
{
    pd_export_mesh_buffer_test_local_flattens_cube_for_export();
    pd_export_mesh_buffer_test_local_rejects_invalid_arguments();
    return 0;
}
