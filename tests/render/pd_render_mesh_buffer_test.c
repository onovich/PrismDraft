#include "prismdraft/render/pd_render_mesh_buffer.h"

#include "prismdraft/core/pd_core_cube_fixture.h"
#include "prismdraft/core/pd_core_mesh_storage_controller.h"

#include <assert.h>

static void pd_render_mesh_buffer_test_local_flattens_cube_to_triangles(void)
{
    PdCoreMeshEntity mesh_entity = { 0 };
    PdRenderMeshBuffer render_mesh_buffer = { 0 };

    assert(pd_core_cube_fixture_build(&mesh_entity) == PD_CORE_RESULT_OK);
    assert(pd_render_mesh_buffer_build_from_mesh(&render_mesh_buffer, &mesh_entity) == PD_CORE_RESULT_OK);

    assert(render_mesh_buffer.vertices != 0);
    assert(render_mesh_buffer.vertex_count == 36u);
    assert(render_mesh_buffer.vertex_capacity >= render_mesh_buffer.vertex_count);

    pd_render_mesh_buffer_free(&render_mesh_buffer);
    pd_core_mesh_storage_controller_free(&mesh_entity);
}

static void pd_render_mesh_buffer_test_local_uses_face_normals_and_colors(void)
{
    PdCoreMeshEntity mesh_entity = { 0 };
    PdRenderMeshBuffer render_mesh_buffer = { 0 };

    assert(pd_core_cube_fixture_build(&mesh_entity) == PD_CORE_RESULT_OK);
    assert(pd_render_mesh_buffer_build_from_mesh(&render_mesh_buffer, &mesh_entity) == PD_CORE_RESULT_OK);

    assert(render_mesh_buffer.vertices[0].normal[0] == mesh_entity.faces[0].face_normal[0]);
    assert(render_mesh_buffer.vertices[0].normal[1] == mesh_entity.faces[0].face_normal[1]);
    assert(render_mesh_buffer.vertices[0].normal[2] == mesh_entity.faces[0].face_normal[2]);
    assert(render_mesh_buffer.vertices[0].color[0] == mesh_entity.faces[0].base_color[0]);
    assert(render_mesh_buffer.vertices[0].color[1] == mesh_entity.faces[0].base_color[1]);
    assert(render_mesh_buffer.vertices[0].color[2] == mesh_entity.faces[0].base_color[2]);
    assert(render_mesh_buffer.vertices[0].color[3] == mesh_entity.faces[0].base_color[3]);

    pd_render_mesh_buffer_free(&render_mesh_buffer);
    pd_core_mesh_storage_controller_free(&mesh_entity);
}

static void pd_render_mesh_buffer_test_local_rejects_invalid_arguments(void)
{
    PdCoreMeshEntity mesh_entity = { 0 };
    PdRenderMeshBuffer render_mesh_buffer = { 0 };

    assert(pd_render_mesh_buffer_build_from_mesh(0, &mesh_entity) == PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);
    assert(pd_render_mesh_buffer_build_from_mesh(&render_mesh_buffer, 0) == PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);
    pd_render_mesh_buffer_free(0);
}

int main(void)
{
    pd_render_mesh_buffer_test_local_flattens_cube_to_triangles();
    pd_render_mesh_buffer_test_local_uses_face_normals_and_colors();
    pd_render_mesh_buffer_test_local_rejects_invalid_arguments();
    return 0;
}
