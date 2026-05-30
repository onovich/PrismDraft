#include "prismdraft/app/pd_app_lifecycle_controller.h"
#include "prismdraft/engine/pd_engine_camera_controller.h"
#include "prismdraft/engine/pd_engine_window_config.h"
#include "prismdraft/render/pd_render_depth_shader.h"
#include "prismdraft/render/pd_render_edge_shader.h"
#include "prismdraft/render/pd_render_face_highlight_buffer.h"
#include "prismdraft/render/pd_render_face_highlight_config.h"
#include "prismdraft/render/pd_render_hardstep_shader.h"
#include "prismdraft/render/pd_render_mesh_buffer.h"
#include "prismdraft/render/pd_render_normal_shader.h"
#include "prismdraft/render/pd_render_shadow_config.h"
#include "prismdraft/render/pd_render_target_config.h"
#include "prismdraft/render/pd_render_target_controller.h"
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

static void pd_app_viewport_controller_local_draw_shadow(PdRenderShadowConfig shadow_config)
{
    Vector3 near_left = { -shadow_config.half_width + shadow_config.offset_x,
                          shadow_config.plane_y,
                          -shadow_config.half_depth + shadow_config.offset_z };
    Vector3 near_right = { shadow_config.half_width + shadow_config.offset_x,
                           shadow_config.plane_y,
                           -shadow_config.half_depth + shadow_config.offset_z };
    Vector3 far_right = { shadow_config.half_width + shadow_config.offset_x + shadow_config.skew_x,
                          shadow_config.plane_y,
                          shadow_config.half_depth + shadow_config.offset_z };
    Vector3 far_left = { -shadow_config.half_width + shadow_config.offset_x + shadow_config.skew_x,
                         shadow_config.plane_y,
                         shadow_config.half_depth + shadow_config.offset_z };

    BeginBlendMode(BLEND_ALPHA);
    DrawTriangle3D(near_left, far_left, far_right, shadow_config.color);
    DrawTriangle3D(near_left, far_right, near_right, shadow_config.color);
    EndBlendMode();
}

int main(void)
{
    PdAppContextEntity app_context;
    PdEngineWindowConfig window_config = pd_engine_window_config_default();
    PdEngineCameraState camera_state = pd_engine_camera_controller_make_default();
    PdRenderVisualConfig visual_config = pd_render_visual_config_default();
    PdRenderFaceHighlightConfig face_highlight_config = pd_render_face_highlight_config_default();
    PdRenderShadowConfig shadow_config = pd_render_shadow_config_default();
    PdRenderHardstepShaderConfig shader_config = pd_render_hardstep_shader_config_default();
    PdRenderDepthShaderConfig depth_shader_config = pd_render_depth_shader_config_default();
    PdRenderNormalShaderConfig normal_shader_config = pd_render_normal_shader_config_default();
    PdRenderEdgeShaderConfig edge_shader_config = pd_render_edge_shader_config_default();
    PdRenderTargetController target_controller = { 0 };
    PdRenderMeshBuffer render_mesh_buffer = { 0 };
    PdRenderMeshBuffer face_highlight_buffer = { 0 };
    Mesh cube_mesh;
    Mesh face_highlight_mesh = { 0 };
    Model cube_model;
    Model face_highlight_model = { 0 };
    Shader hardstep_shader;
    Shader depth_shader;
    Shader normal_shader;
    Shader edge_shader;
    int light_direction_location;
    int dark_intensity_location;
    int edge_normal_texture_location;
    int edge_depth_texture_location;
    int edge_texel_size_location;
    int edge_depth_threshold_location;
    int edge_normal_threshold_location;
    Vector2 edge_texel_size;
    Rectangle screen_source;
    Vector2 screen_position = { 0.0f, 0.0f };
    Vector3 origin = { 0.0f, 0.0f, 0.0f };
    Color normal_background = { 128u, 128u, 255u, 255u };
    Color depth_background = { 255u, 255u, 255u, 255u };

    if (pd_app_lifecycle_controller_init(&app_context) != PD_CORE_RESULT_OK) {
        return 1;
    }

    if (pd_render_mesh_buffer_build_from_mesh(&render_mesh_buffer, &app_context.active_mesh) != PD_CORE_RESULT_OK) {
        pd_app_lifecycle_controller_shutdown(&app_context);
        return 1;
    }

    if (pd_editor_selection_state_select_face(&app_context.selection_state, 1u) != PD_CORE_RESULT_OK) {
        pd_render_mesh_buffer_free(&render_mesh_buffer);
        pd_app_lifecycle_controller_shutdown(&app_context);
        return 1;
    }

    if (pd_render_face_highlight_buffer_build_for_face(
            &face_highlight_buffer,
            &app_context.active_mesh,
            app_context.selection_state.primary_index,
            face_highlight_config) != PD_CORE_RESULT_OK) {
        pd_render_mesh_buffer_free(&render_mesh_buffer);
        pd_app_lifecycle_controller_shutdown(&app_context);
        return 1;
    }

    SetConfigFlags(FLAG_WINDOW_HIDDEN | FLAG_MSAA_4X_HINT);
    InitWindow(window_config.width, window_config.height, window_config.title);
    SetTargetFPS(60);

    if (pd_render_target_controller_init(
            &target_controller, pd_render_target_config_make(window_config.width, window_config.height)) !=
        PD_CORE_RESULT_OK) {
        CloseWindow();
        pd_render_mesh_buffer_free(&face_highlight_buffer);
        pd_render_mesh_buffer_free(&render_mesh_buffer);
        pd_app_lifecycle_controller_shutdown(&app_context);
        return 1;
    }

    hardstep_shader = LoadShader(shader_config.vertex_shader_path, shader_config.fragment_shader_path);
    depth_shader = LoadShader(depth_shader_config.vertex_shader_path, depth_shader_config.fragment_shader_path);
    normal_shader = LoadShader(normal_shader_config.vertex_shader_path, normal_shader_config.fragment_shader_path);
    edge_shader = LoadShader(edge_shader_config.vertex_shader_path, edge_shader_config.fragment_shader_path);
    light_direction_location = GetShaderLocation(hardstep_shader, "lightDirection");
    dark_intensity_location = GetShaderLocation(hardstep_shader, "darkIntensity");
    SetShaderValue(hardstep_shader, light_direction_location, &shader_config.light_direction, SHADER_UNIFORM_VEC3);
    SetShaderValue(hardstep_shader, dark_intensity_location, &shader_config.dark_intensity, SHADER_UNIFORM_FLOAT);
    edge_normal_texture_location = GetShaderLocation(edge_shader, "normalTexture");
    edge_depth_texture_location = GetShaderLocation(edge_shader, "depthTexture");
    edge_texel_size_location = GetShaderLocation(edge_shader, "texelSize");
    edge_depth_threshold_location = GetShaderLocation(edge_shader, "edgeDepthThreshold");
    edge_normal_threshold_location = GetShaderLocation(edge_shader, "edgeNormalThreshold");
    edge_texel_size.x = 1.0f / (float)window_config.width;
    edge_texel_size.y = 1.0f / (float)window_config.height;
    SetShaderValue(edge_shader, edge_texel_size_location, &edge_texel_size, SHADER_UNIFORM_VEC2);
    SetShaderValue(
        edge_shader, edge_depth_threshold_location, &edge_shader_config.edge_depth_threshold, SHADER_UNIFORM_FLOAT);
    SetShaderValue(
        edge_shader, edge_normal_threshold_location, &edge_shader_config.edge_normal_threshold, SHADER_UNIFORM_FLOAT);

    cube_mesh = pd_app_viewport_controller_local_make_mesh(&render_mesh_buffer);
    cube_model = LoadModelFromMesh(cube_mesh);
    face_highlight_mesh = pd_app_viewport_controller_local_make_mesh(&face_highlight_buffer);
    face_highlight_model = LoadModelFromMesh(face_highlight_mesh);

    cube_model.materials[0].shader = hardstep_shader;
    face_highlight_model.materials[0].shader = hardstep_shader;
    BeginTextureMode(target_controller.color_depth_target);
    ClearBackground(visual_config.background_color);
    BeginMode3D(camera_state.camera);
    pd_app_viewport_controller_local_draw_shadow(shadow_config);
    DrawModel(cube_model, origin, 1.0f, WHITE);
    BeginBlendMode(BLEND_ALPHA);
    DrawModel(face_highlight_model, origin, 1.0f, WHITE);
    EndBlendMode();
    EndMode3D();
    EndTextureMode();

    cube_model.materials[0].shader = depth_shader;
    BeginTextureMode(target_controller.depth_target);
    ClearBackground(depth_background);
    BeginMode3D(camera_state.camera);
    DrawModel(cube_model, origin, 1.0f, WHITE);
    EndMode3D();
    EndTextureMode();

    cube_model.materials[0].shader = normal_shader;
    BeginTextureMode(target_controller.normal_target);
    ClearBackground(normal_background);
    BeginMode3D(camera_state.camera);
    DrawModel(cube_model, origin, 1.0f, WHITE);
    EndMode3D();
    EndTextureMode();

    screen_source = (Rectangle){ 0.0f,
                                 0.0f,
                                 (float)target_controller.color_depth_target.texture.width,
                                 -(float)target_controller.color_depth_target.texture.height };

    BeginDrawing();
    ClearBackground(visual_config.background_color);
    BeginShaderMode(edge_shader);
    SetShaderValueTexture(edge_shader, edge_normal_texture_location, target_controller.normal_target.texture);
    SetShaderValueTexture(edge_shader, edge_depth_texture_location, target_controller.depth_target.texture);
    DrawTextureRec(target_controller.color_depth_target.texture, screen_source, screen_position, WHITE);
    EndShaderMode();
    EndDrawing();

    MakeDirectory("captures");
    TakeScreenshot("captures/phase2_cube.png");

    cube_model.materials[0].shader = (Shader){ 0 };
    face_highlight_model.materials[0].shader = (Shader){ 0 };
    UnloadModel(face_highlight_model);
    UnloadModel(cube_model);
    UnloadShader(edge_shader);
    UnloadShader(normal_shader);
    UnloadShader(depth_shader);
    UnloadShader(hardstep_shader);
    pd_render_target_controller_free(&target_controller);
    CloseWindow();

    pd_render_mesh_buffer_free(&face_highlight_buffer);
    pd_render_mesh_buffer_free(&render_mesh_buffer);
    pd_app_lifecycle_controller_shutdown(&app_context);
    return 0;
}
