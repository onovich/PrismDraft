#include "prismdraft/render/pd_render_face_highlight_buffer.h"

#include "prismdraft/core/pd_core_cube_fixture.h"
#include "prismdraft/core/pd_core_mesh_storage_controller.h"

#include <assert.h>

static void pd_render_face_highlight_buffer_test_local_builds_one_cube_face(void)
{
    PdCoreMeshEntity mesh_entity = { 0 };
    PdRenderMeshBuffer highlight_buffer = { 0 };
    PdRenderFaceHighlightConfig config = pd_render_face_highlight_config_default();

    assert(pd_core_cube_fixture_build(&mesh_entity) == PD_CORE_RESULT_OK);
    assert(pd_render_face_highlight_buffer_build_for_face(&highlight_buffer, &mesh_entity, 1u, config) ==
           PD_CORE_RESULT_OK);

    assert(highlight_buffer.vertices != 0);
    assert(highlight_buffer.vertex_count == 6u);
    assert(highlight_buffer.vertex_capacity >= highlight_buffer.vertex_count);
    assert(highlight_buffer.vertices[0].color[0] == config.color[0]);
    assert(highlight_buffer.vertices[0].color[1] == config.color[1]);
    assert(highlight_buffer.vertices[0].color[2] == config.color[2]);
    assert(highlight_buffer.vertices[0].color[3] == config.color[3]);
    assert(highlight_buffer.vertices[0].position[2] == 1.0f + config.surface_offset);

    pd_render_mesh_buffer_free(&highlight_buffer);
    pd_core_mesh_storage_controller_free(&mesh_entity);
}

static void pd_render_face_highlight_buffer_test_local_rejects_invalid_arguments(void)
{
    PdCoreMeshEntity mesh_entity = { 0 };
    PdRenderMeshBuffer highlight_buffer = { 0 };
    PdRenderFaceHighlightConfig config = pd_render_face_highlight_config_default();

    assert(pd_core_cube_fixture_build(&mesh_entity) == PD_CORE_RESULT_OK);
    assert(pd_render_face_highlight_buffer_build_for_face(0, &mesh_entity, 0u, config) ==
           PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);
    assert(pd_render_face_highlight_buffer_build_for_face(&highlight_buffer, 0, 0u, config) ==
           PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);
    assert(pd_render_face_highlight_buffer_build_for_face(
               &highlight_buffer,
               &mesh_entity,
               mesh_entity.face_count,
               config) == PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);

    config.color[3] = 0u;
    assert(pd_render_face_highlight_buffer_build_for_face(&highlight_buffer, &mesh_entity, 0u, config) ==
           PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);

    pd_render_mesh_buffer_free(&highlight_buffer);
    pd_core_mesh_storage_controller_free(&mesh_entity);
}

int main(void)
{
    pd_render_face_highlight_buffer_test_local_builds_one_cube_face();
    pd_render_face_highlight_buffer_test_local_rejects_invalid_arguments();
    return 0;
}
