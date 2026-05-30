#include "prismdraft/app/pd_app_lifecycle_controller.h"
#include "prismdraft/engine/pd_engine_camera_controller.h"
#include "prismdraft/engine/pd_engine_window_config.h"
#include "prismdraft/render/pd_render_hardstep_shader.h"
#include "prismdraft/render/pd_render_mesh_buffer.h"
#include "prismdraft/render/pd_render_visual_config.h"

#include "raylib.h"

#include <stddef.h>

static Mesh pd_app_viewport_controller_local_make_mesh(const PdRenderMeshBuffer* render_mesh_buffer)
{
    Mesh mesh = { 0 };
    int vertex_index;

    mesh.vertexCount = (int)render_mesh_buffer->vertex_count;
    mesh.triangleCount = mesh.vertexCount / 3;
    mesh.vertices = (float*)MemAlloc((unsigned int)mesh.vertexCount * 3u * sizeof(float));
    mesh.normals = (float*)MemAlloc((unsigned int)mesh.vertexCount * 3u * sizeof(float));
    mesh.colors = (unsigned char*)MemAlloc((unsigned int)mesh.vertexCount * 4u * sizeof(unsigned char));

    for (vertex_index = 0; vertex_index < mesh.vertexCount; vertex_index++) {
        const PdRenderMeshBufferVertex* source_vertex = &render_mesh_buffer->vertices[vertex_index];
        mesh.vertices[(vertex_index * 3) + 0] = source_vertex->position[0];
        mesh.vertices[(vertex_index * 3) + 1] = source_vertex->position[1];
        mesh.vertices[(vertex_index * 3) + 2] = source_vertex->position[2];
        mesh.normals[(vertex_index * 3) + 0] = source_vertex->normal[0];
        mesh.normals[(vertex_index * 3) + 1] = source_vertex->normal[1];
        mesh.normals[(vertex_index * 3) + 2] = source_vertex->normal[2];
        mesh.colors[(vertex_index * 4) + 0] = source_vertex->color[0];
        mesh.colors[(vertex_index * 4) + 1] = source_vertex->color[1];
        mesh.colors[(vertex_index * 4) + 2] = source_vertex->color[2];
        mesh.colors[(vertex_index * 4) + 3] = source_vertex->color[3];
    }

    UploadMesh(&mesh, false);
    return mesh;
}

int main(void)
{
    PdAppContextEntity app_context;
    PdEngineWindowConfig window_config = pd_engine_window_config_default();
    PdEngineCameraState camera_state = pd_engine_camera_controller_make_default();
    PdRenderVisualConfig visual_config = pd_render_visual_config_default();
    PdRenderHardstepShaderConfig shader_config = pd_render_hardstep_shader_config_default();
    PdRenderMeshBuffer render_mesh_buffer = { 0 };
    Mesh cube_mesh;
    Model cube_model;
    Shader hardstep_shader;
    int light_direction_location;
    int dark_intensity_location;
    Vector3 origin = { 0.0f, 0.0f, 0.0f };

    if (pd_app_lifecycle_controller_init(&app_context) != PD_CORE_RESULT_OK) {
        return 1;
    }

    if (pd_render_mesh_buffer_build_from_mesh(&render_mesh_buffer, &app_context.active_mesh) != PD_CORE_RESULT_OK) {
        pd_app_lifecycle_controller_shutdown(&app_context);
        return 1;
    }

    SetConfigFlags(FLAG_WINDOW_HIDDEN | FLAG_MSAA_4X_HINT);
    InitWindow(window_config.width, window_config.height, window_config.title);
    SetTargetFPS(60);

    hardstep_shader = LoadShader(shader_config.vertex_shader_path, shader_config.fragment_shader_path);
    light_direction_location = GetShaderLocation(hardstep_shader, "lightDirection");
    dark_intensity_location = GetShaderLocation(hardstep_shader, "darkIntensity");
    SetShaderValue(hardstep_shader, light_direction_location, &shader_config.light_direction, SHADER_UNIFORM_VEC3);
    SetShaderValue(hardstep_shader, dark_intensity_location, &shader_config.dark_intensity, SHADER_UNIFORM_FLOAT);

    cube_mesh = pd_app_viewport_controller_local_make_mesh(&render_mesh_buffer);
    cube_model = LoadModelFromMesh(cube_mesh);
    cube_model.materials[0].shader = hardstep_shader;

    BeginDrawing();
    ClearBackground(visual_config.background_color);
    BeginMode3D(camera_state.camera);
    DrawModel(cube_model, origin, 1.0f, WHITE);
    EndMode3D();
    EndDrawing();

    MakeDirectory("captures");
    TakeScreenshot("captures/phase1_cube.png");

    cube_model.materials[0].shader = (Shader){ 0 };
    UnloadModel(cube_model);
    UnloadShader(hardstep_shader);
    CloseWindow();

    pd_render_mesh_buffer_free(&render_mesh_buffer);
    pd_app_lifecycle_controller_shutdown(&app_context);
    return 0;
}
