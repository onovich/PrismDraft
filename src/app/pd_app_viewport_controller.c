#include "prismdraft/app/pd_app_lifecycle_controller.h"
#include "prismdraft/core/pd_core_mesh_entity.h"
#include "prismdraft/editor/pd_editor_modeling_service.h"
#include "prismdraft/editor/pd_editor_panel_state.h"
#include "prismdraft/editor/pd_editor_pick_service.h"
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
#include "raymath.h"

#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

static const int PD_APP_VIEWPORT_CONTROLLER_RENDER_SCALE = 2;
static const float PD_APP_VIEWPORT_CONTROLLER_ORBIT_SPEED = 0.01f;
static const float PD_APP_VIEWPORT_CONTROLLER_MIN_CAMERA_PITCH = -1.2f;
static const float PD_APP_VIEWPORT_CONTROLLER_MAX_CAMERA_PITCH = 1.2f;
static const float PD_APP_VIEWPORT_CONTROLLER_MIN_CAMERA_FOVY = 1.8f;
static const float PD_APP_VIEWPORT_CONTROLLER_MAX_CAMERA_FOVY = 8.0f;
static const float PD_APP_VIEWPORT_CONTROLLER_ZOOM_SPEED = 0.28f;
static const float PD_APP_VIEWPORT_CONTROLLER_MOVE_STEP = 0.035f;
static const float PD_APP_VIEWPORT_CONTROLLER_ROTATE_STEP_DEGREES = 1.5f;
static const float PD_APP_VIEWPORT_CONTROLLER_SCALE_STEP = 0.015f;
static const float PD_APP_VIEWPORT_CONTROLLER_EDGE_SAMPLE_RADIUS_STEP = 0.25f;
static const float PD_APP_VIEWPORT_CONTROLLER_EDGE_DEPTH_STEP = 0.015f;
static const float PD_APP_VIEWPORT_CONTROLLER_EDGE_NORMAL_STEP = 0.12f;
static const float PD_APP_VIEWPORT_CONTROLLER_LIGHT_STEP = 0.04f;
static const float PD_APP_VIEWPORT_CONTROLLER_DARK_INTENSITY_STEP = 0.12f;
static const float PD_APP_VIEWPORT_CONTROLLER_SHADOW_OFFSET_STEP = 0.05f;
static const int PD_APP_VIEWPORT_CONTROLLER_SHADOW_ALPHA_STEP = 12;
static const int PD_APP_VIEWPORT_CONTROLLER_PANEL_WIDTH = 308;
static const int PD_APP_VIEWPORT_CONTROLLER_PANEL_MARGIN = 12;
static const unsigned int PD_APP_VIEWPORT_CONTROLLER_INTERACTIVE_WINDOW_FLAGS =
    FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN;

static int pd_app_viewport_controller_local_has_argument(int argc, char** argv, const char* expected_argument)
{
    int argument_index;

    if (argv == 0 || expected_argument == 0) {
        return 0;
    }

    for (argument_index = 1; argument_index < argc; argument_index++) {
        if (argv[argument_index] != 0 && strcmp(argv[argument_index], expected_argument) == 0) {
            return 1;
        }
    }

    return 0;
}

static const char* pd_app_viewport_controller_local_get_argument_value(
    int argc,
    char** argv,
    const char* expected_argument)
{
    int argument_index;

    if (argv == 0 || expected_argument == 0) {
        return 0;
    }

    for (argument_index = 1; argument_index < argc - 1; argument_index++) {
        if (argv[argument_index] != 0 && strcmp(argv[argument_index], expected_argument) == 0) {
            return argv[argument_index + 1];
        }
    }

    return 0;
}

static float pd_app_viewport_controller_local_clamp(float value, float min_value, float max_value)
{
    if (value < min_value) {
        return min_value;
    }

    if (value > max_value) {
        return max_value;
    }

    return value;
}

static Vector3 pd_app_viewport_controller_local_add(Vector3 left, Vector3 right)
{
    return (Vector3){ left.x + right.x, left.y + right.y, left.z + right.z };
}

static Vector3 pd_app_viewport_controller_local_subtract(Vector3 left, Vector3 right)
{
    return (Vector3){ left.x - right.x, left.y - right.y, left.z - right.z };
}

static Vector3 pd_app_viewport_controller_local_scale(Vector3 value, float scale)
{
    return (Vector3){ value.x * scale, value.y * scale, value.z * scale };
}

static float pd_app_viewport_controller_local_length(Vector3 value)
{
    return sqrtf((value.x * value.x) + (value.y * value.y) + (value.z * value.z));
}

static Vector3 pd_app_viewport_controller_local_cross(Vector3 left, Vector3 right)
{
    return (Vector3){ (left.y * right.z) - (left.z * right.y),
                      (left.z * right.x) - (left.x * right.z),
                      (left.x * right.y) - (left.y * right.x) };
}

static Vector3 pd_app_viewport_controller_local_normalize(Vector3 value)
{
    float length = pd_app_viewport_controller_local_length(value);

    if (length <= 0.000001f) {
        return (Vector3){ 0.0f, 0.0f, 0.0f };
    }

    return pd_app_viewport_controller_local_scale(value, 1.0f / length);
}

static void pd_app_viewport_controller_local_orbit_camera(PdEngineCameraState* camera_state, Vector2 mouse_delta)
{
    Vector3 offset;
    float radius;
    float yaw;
    float pitch;
    float pitch_cosine;

    if (camera_state == 0) {
        return;
    }

    offset = pd_app_viewport_controller_local_subtract(camera_state->camera.position, camera_state->camera.target);
    radius = pd_app_viewport_controller_local_length(offset);
    if (radius <= 0.000001f) {
        return;
    }

    yaw = atan2f(offset.z, offset.x) - (mouse_delta.x * PD_APP_VIEWPORT_CONTROLLER_ORBIT_SPEED);
    pitch = asinf(offset.y / radius) + (mouse_delta.y * PD_APP_VIEWPORT_CONTROLLER_ORBIT_SPEED);
    pitch = pd_app_viewport_controller_local_clamp(
        pitch,
        PD_APP_VIEWPORT_CONTROLLER_MIN_CAMERA_PITCH,
        PD_APP_VIEWPORT_CONTROLLER_MAX_CAMERA_PITCH);
    pitch_cosine = cosf(pitch);

    camera_state->camera.position.x = camera_state->camera.target.x + (radius * pitch_cosine * cosf(yaw));
    camera_state->camera.position.y = camera_state->camera.target.y + (radius * sinf(pitch));
    camera_state->camera.position.z = camera_state->camera.target.z + (radius * pitch_cosine * sinf(yaw));
}

static void pd_app_viewport_controller_local_pan_camera(PdEngineCameraState* camera_state, Vector2 mouse_delta)
{
    Vector3 forward;
    Vector3 right;
    Vector3 up;
    Vector3 pan_delta;
    float pan_scale;

    if (camera_state == 0 || GetScreenHeight() <= 0) {
        return;
    }

    forward = pd_app_viewport_controller_local_normalize(
        pd_app_viewport_controller_local_subtract(camera_state->camera.target, camera_state->camera.position));
    right = pd_app_viewport_controller_local_normalize(
        pd_app_viewport_controller_local_cross(forward, camera_state->camera.up));
    up = pd_app_viewport_controller_local_normalize(pd_app_viewport_controller_local_cross(right, forward));
    pan_scale = camera_state->camera.fovy / (float)GetScreenHeight();
    pan_delta = pd_app_viewport_controller_local_add(
        pd_app_viewport_controller_local_scale(right, -mouse_delta.x * pan_scale),
        pd_app_viewport_controller_local_scale(up, mouse_delta.y * pan_scale));

    camera_state->camera.position = pd_app_viewport_controller_local_add(camera_state->camera.position, pan_delta);
    camera_state->camera.target = pd_app_viewport_controller_local_add(camera_state->camera.target, pan_delta);
}

static void pd_app_viewport_controller_local_zoom_camera(PdEngineCameraState* camera_state, float mouse_wheel_move)
{
    if (camera_state == 0 || mouse_wheel_move == 0.0f) {
        return;
    }

    camera_state->camera.fovy = pd_app_viewport_controller_local_clamp(
        camera_state->camera.fovy - (mouse_wheel_move * PD_APP_VIEWPORT_CONTROLLER_ZOOM_SPEED),
        PD_APP_VIEWPORT_CONTROLLER_MIN_CAMERA_FOVY,
        PD_APP_VIEWPORT_CONTROLLER_MAX_CAMERA_FOVY);
}

static void pd_app_viewport_controller_local_update_camera(PdEngineCameraState* camera_state)
{
    Vector2 mouse_delta = GetMouseDelta();

    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        pd_app_viewport_controller_local_orbit_camera(camera_state, mouse_delta);
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
        pd_app_viewport_controller_local_pan_camera(camera_state, mouse_delta);
    }

    pd_app_viewport_controller_local_zoom_camera(camera_state, GetMouseWheelMove());
}

static Matrix pd_app_viewport_controller_local_make_object_transform(const PdEditorTransformState* transform_state)
{
    Matrix scale_matrix;
    Matrix rotation_matrix;
    Matrix translation_matrix;
    Matrix scale_rotation_matrix;
    Vector3 rotation_radians;

    if (transform_state == 0) {
        return MatrixIdentity();
    }

    rotation_radians = (Vector3){ transform_state->rotation_degrees[0] * DEG2RAD,
                                  transform_state->rotation_degrees[1] * DEG2RAD,
                                  transform_state->rotation_degrees[2] * DEG2RAD };
    scale_matrix = MatrixScale(transform_state->scale[0], transform_state->scale[1], transform_state->scale[2]);
    rotation_matrix = MatrixRotateXYZ(rotation_radians);
    translation_matrix =
        MatrixTranslate(transform_state->position[0], transform_state->position[1], transform_state->position[2]);
    scale_rotation_matrix = MatrixMultiply(scale_matrix, rotation_matrix);
    return MatrixMultiply(scale_rotation_matrix, translation_matrix);
}

static void pd_app_viewport_controller_local_update_transform(PdEditorTransformState* transform_state)
{
    if (transform_state == 0) {
        return;
    }

    if (IsKeyDown(KEY_A)) {
        (void)pd_editor_transform_state_translate(transform_state, -PD_APP_VIEWPORT_CONTROLLER_MOVE_STEP, 0.0f, 0.0f);
    }

    if (IsKeyDown(KEY_D)) {
        (void)pd_editor_transform_state_translate(transform_state, PD_APP_VIEWPORT_CONTROLLER_MOVE_STEP, 0.0f, 0.0f);
    }

    if (IsKeyDown(KEY_W)) {
        (void)pd_editor_transform_state_translate(transform_state, 0.0f, 0.0f, -PD_APP_VIEWPORT_CONTROLLER_MOVE_STEP);
    }

    if (IsKeyDown(KEY_S)) {
        (void)pd_editor_transform_state_translate(transform_state, 0.0f, 0.0f, PD_APP_VIEWPORT_CONTROLLER_MOVE_STEP);
    }

    if (IsKeyDown(KEY_Q)) {
        (void)pd_editor_transform_state_translate(transform_state, 0.0f, PD_APP_VIEWPORT_CONTROLLER_MOVE_STEP, 0.0f);
    }

    if (IsKeyDown(KEY_E)) {
        (void)pd_editor_transform_state_translate(transform_state, 0.0f, -PD_APP_VIEWPORT_CONTROLLER_MOVE_STEP, 0.0f);
    }

    if (IsKeyDown(KEY_R)) {
        (void)pd_editor_transform_state_rotate_degrees(
            transform_state,
            0.0f,
            PD_APP_VIEWPORT_CONTROLLER_ROTATE_STEP_DEGREES,
            0.0f);
    }

    if (IsKeyDown(KEY_F)) {
        (void)pd_editor_transform_state_rotate_degrees(
            transform_state,
            0.0f,
            -PD_APP_VIEWPORT_CONTROLLER_ROTATE_STEP_DEGREES,
            0.0f);
    }

    if (IsKeyDown(KEY_X)) {
        (void)pd_editor_transform_state_scale_uniform(transform_state, PD_APP_VIEWPORT_CONTROLLER_SCALE_STEP);
    }

    if (IsKeyDown(KEY_Z)) {
        (void)pd_editor_transform_state_scale_uniform(transform_state, -PD_APP_VIEWPORT_CONTROLLER_SCALE_STEP);
    }

    if (IsKeyPressed(KEY_T)) {
        pd_editor_transform_state_reset(transform_state);
    }
}

static void pd_app_viewport_controller_local_update_transform_tool(PdEditorToolState* tool_state)
{
    if (tool_state == 0) {
        return;
    }

    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_D) || IsKeyDown(KEY_W) || IsKeyDown(KEY_S) || IsKeyDown(KEY_Q) ||
        IsKeyDown(KEY_E) || IsKeyDown(KEY_R) || IsKeyDown(KEY_F) || IsKeyDown(KEY_X) || IsKeyDown(KEY_Z) ||
        IsKeyPressed(KEY_T)) {
        (void)pd_editor_tool_state_set_active(tool_state, PD_EDITOR_TOOL_KIND_TRANSFORM);
    }
}

static Color pd_app_viewport_controller_local_make_color(const uint8_t color[4])
{
    if (color == 0) {
        return WHITE;
    }

    return (Color){ color[0], color[1], color[2], color[3] };
}

static uint8_t pd_app_viewport_controller_local_float_to_u8(float value)
{
    float clamped_value = pd_app_viewport_controller_local_clamp(value, 0.0f, 255.0f);

    return (uint8_t)(clamped_value + 0.5f);
}

static Rectangle pd_app_viewport_controller_local_get_panel_rect(void)
{
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();
    float panel_x = (float)(screen_width - PD_APP_VIEWPORT_CONTROLLER_PANEL_WIDTH -
                            PD_APP_VIEWPORT_CONTROLLER_PANEL_MARGIN);
    float panel_height = (float)(screen_height - (PD_APP_VIEWPORT_CONTROLLER_PANEL_MARGIN * 2));

    if (panel_x < (float)PD_APP_VIEWPORT_CONTROLLER_PANEL_MARGIN) {
        panel_x = (float)PD_APP_VIEWPORT_CONTROLLER_PANEL_MARGIN;
    }

    if (panel_height < 240.0f) {
        panel_height = 240.0f;
    }

    return (Rectangle){ panel_x,
                        (float)PD_APP_VIEWPORT_CONTROLLER_PANEL_MARGIN,
                        (float)PD_APP_VIEWPORT_CONTROLLER_PANEL_WIDTH,
                        panel_height };
}

static int pd_app_viewport_controller_local_point_in_rect(Vector2 point, Rectangle rectangle)
{
    return point.x >= rectangle.x && point.x <= rectangle.x + rectangle.width && point.y >= rectangle.y &&
           point.y <= rectangle.y + rectangle.height;
}

static int pd_app_viewport_controller_local_is_panel_mouse_target(const PdAppContextEntity* app_context)
{
    if (app_context == 0 || !app_context->panel_state.is_open) {
        return 0;
    }

    return pd_app_viewport_controller_local_point_in_rect(
        GetMousePosition(),
        pd_app_viewport_controller_local_get_panel_rect());
}

static void pd_app_viewport_controller_local_draw_panel_text(const char* text, float x, float y, int font_size)
{
    DrawText(text, (int)x, (int)y, font_size, (Color){ 236u, 240u, 244u, 235u });
}

static int pd_app_viewport_controller_local_panel_button(Rectangle rectangle, const char* label, int is_active)
{
    Vector2 mouse_position = GetMousePosition();
    int is_hovered = pd_app_viewport_controller_local_point_in_rect(mouse_position, rectangle);
    Color fill_color = is_active ? (Color){ 90u, 140u, 180u, 226u } : (Color){ 42u, 47u, 56u, 220u };
    Color border_color = is_hovered ? (Color){ 240u, 245u, 250u, 210u } : (Color){ 130u, 140u, 154u, 110u };

    if (is_hovered && !is_active) {
        fill_color = (Color){ 58u, 65u, 78u, 226u };
    }

    DrawRectangleRec(rectangle, fill_color);
    DrawRectangleLinesEx(rectangle, 1.0f, border_color);
    DrawText(label, (int)(rectangle.x + 8.0f), (int)(rectangle.y + 7.0f), 10, (Color){ 245u, 247u, 250u, 240u });

    return is_hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

static int pd_app_viewport_controller_local_panel_slider(
    Rectangle rectangle,
    const char* label,
    float min_value,
    float max_value,
    float* value)
{
    Vector2 mouse_position = GetMousePosition();
    float normalized_value;
    float knob_x;
    char value_text[64];
    int is_changed = 0;
    int is_hovered = pd_app_viewport_controller_local_point_in_rect(mouse_position, rectangle);

    if (value == 0 || max_value <= min_value) {
        return 0;
    }

    normalized_value = (*value - min_value) / (max_value - min_value);
    normalized_value = pd_app_viewport_controller_local_clamp(normalized_value, 0.0f, 1.0f);

    if (is_hovered && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        normalized_value = (mouse_position.x - rectangle.x) / rectangle.width;
        normalized_value = pd_app_viewport_controller_local_clamp(normalized_value, 0.0f, 1.0f);
        *value = min_value + (normalized_value * (max_value - min_value));
        is_changed = 1;
    }

    knob_x = rectangle.x + (normalized_value * rectangle.width);
    DrawText(label, (int)rectangle.x, (int)(rectangle.y - 14.0f), 10, (Color){ 236u, 240u, 244u, 220u });
    (void)snprintf(value_text, sizeof(value_text), "%.3f", (double)*value);
    DrawText(
        value_text,
        (int)(rectangle.x + rectangle.width - 48.0f),
        (int)(rectangle.y - 14.0f),
        10,
        (Color){ 198u, 207u, 220u, 220u });
    DrawRectangleRec(rectangle, (Color){ 32u, 37u, 46u, 230u });
    DrawRectangle(
        (int)rectangle.x,
        (int)rectangle.y,
        (int)(normalized_value * rectangle.width),
        (int)rectangle.height,
        (Color){ 92u, 143u, 184u, 230u });
    DrawRectangleLinesEx(rectangle, 1.0f, (Color){ 140u, 151u, 168u, 120u });
    DrawCircle((int)knob_x, (int)(rectangle.y + (rectangle.height * 0.5f)), 5.0f, (Color){ 245u, 248u, 250u, 245u });

    return is_changed;
}

static int pd_app_viewport_controller_local_panel_u8_slider(
    Rectangle rectangle,
    const char* label,
    uint8_t* value)
{
    float float_value;
    int is_changed;

    if (value == 0) {
        return 0;
    }

    float_value = (float)*value;
    is_changed = pd_app_viewport_controller_local_panel_slider(rectangle, label, 0.0f, 255.0f, &float_value);
    if (is_changed) {
        *value = pd_app_viewport_controller_local_float_to_u8(float_value);
    }

    return is_changed;
}

static void pd_app_viewport_controller_local_normalize_light_direction(float light_direction[3])
{
    float length;

    if (light_direction == 0) {
        return;
    }

    length = sqrtf(
        (light_direction[0] * light_direction[0]) +
        (light_direction[1] * light_direction[1]) +
        (light_direction[2] * light_direction[2]));
    if (length <= 0.000001f) {
        light_direction[0] = -0.45f;
        light_direction[1] = -0.75f;
        light_direction[2] = -0.5f;
        return;
    }

    light_direction[0] /= length;
    light_direction[1] /= length;
    light_direction[2] /= length;
}

static Vector3 pd_app_viewport_controller_local_make_vector3(const float value[3])
{
    if (value == 0) {
        return (Vector3){ 0.0f, -1.0f, 0.0f };
    }

    return (Vector3){ value[0], value[1], value[2] };
}

static PdRenderVisualConfig pd_app_viewport_controller_local_make_visual_config(
    const PdEditorVisualState* visual_state)
{
    PdRenderVisualConfig visual_config = pd_render_visual_config_default();

    if (visual_state == 0) {
        return visual_config;
    }

    visual_config.background_color = pd_app_viewport_controller_local_make_color(visual_state->background_color);
    visual_config.light_direction = pd_app_viewport_controller_local_make_vector3(visual_state->light_direction);
    visual_config.dark_intensity = visual_state->dark_intensity;
    visual_config.edge_sample_radius = visual_state->edge_sample_radius;
    visual_config.edge_depth_threshold = visual_state->edge_depth_threshold;
    visual_config.edge_normal_threshold = visual_state->edge_normal_threshold;
    return visual_config;
}

static PdRenderShadowConfig pd_app_viewport_controller_local_make_shadow_config(
    const PdEditorVisualState* visual_state)
{
    PdRenderShadowConfig shadow_config = pd_render_shadow_config_default();

    if (visual_state == 0) {
        return shadow_config;
    }

    shadow_config.color = pd_app_viewport_controller_local_make_color(visual_state->shadow_color);
    shadow_config.plane_y = visual_state->shadow_plane_y;
    shadow_config.offset_x = visual_state->shadow_offset_x;
    shadow_config.offset_z = visual_state->shadow_offset_z;
    shadow_config.half_width = visual_state->shadow_half_width;
    shadow_config.half_depth = visual_state->shadow_half_depth;
    shadow_config.skew_x = visual_state->shadow_skew_x;
    return shadow_config;
}

static PdRenderShadowConfig pd_app_viewport_controller_local_make_object_shadow_config(
    PdRenderShadowConfig shadow_config,
    const PdEditorTransformState* transform_state)
{
    float scale_x;
    float scale_z;

    if (transform_state == 0) {
        return shadow_config;
    }

    scale_x = fabsf(transform_state->scale[0]);
    scale_z = fabsf(transform_state->scale[2]);
    shadow_config.offset_x += transform_state->position[0];
    shadow_config.offset_z += transform_state->position[2];
    shadow_config.half_width *= scale_x;
    shadow_config.half_depth *= scale_z;
    shadow_config.skew_x *= scale_x;
    return shadow_config;
}

static const char* pd_app_viewport_controller_local_get_result_name(PdCoreResult result)
{
    switch (result) {
        case PD_CORE_RESULT_OK:
            return "ok";
        case PD_CORE_RESULT_ERROR_INVALID_ARGUMENT:
            return "invalid_arg";
        case PD_CORE_RESULT_ERROR_OUT_OF_MEMORY:
            return "out_of_memory";
        case PD_CORE_RESULT_ERROR_TOPOLOGY_INVALID:
            return "topology_invalid";
        default:
            return "unknown";
    }
}

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

static PdCoreResult pd_app_viewport_controller_local_rebuild_cube_model(
    PdRenderMeshBuffer* render_mesh_buffer,
    Model* cube_model,
    const PdCoreMeshEntity* mesh_entity)
{
    Mesh cube_mesh;

    if (render_mesh_buffer == 0 || cube_model == 0 || mesh_entity == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    cube_model->materials[0].shader = (Shader){ 0 };
    UnloadModel(*cube_model);
    pd_render_mesh_buffer_free(render_mesh_buffer);
    if (pd_render_mesh_buffer_build_from_mesh(render_mesh_buffer, mesh_entity) != PD_CORE_RESULT_OK) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    cube_mesh = pd_app_viewport_controller_local_make_mesh(render_mesh_buffer);
    *cube_model = LoadModelFromMesh(cube_mesh);
    return PD_CORE_RESULT_OK;
}

static PdCoreResult pd_app_viewport_controller_local_apply_selected_face_color(PdAppContextEntity* app_context)
{
    uint32_t face_index;

    if (app_context == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (app_context->selection_state.kind != PD_EDITOR_SELECTION_KIND_FACE) {
        return PD_CORE_RESULT_OK;
    }

    face_index = app_context->selection_state.primary_index;
    if (face_index >= app_context->active_mesh.face_count) {
        app_context->tool_state.last_result = PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
        pd_editor_selection_state_clear(&app_context->selection_state);
        return PD_CORE_RESULT_OK;
    }

    app_context->active_mesh.faces[face_index].base_color[0] = app_context->visual_state.face_color[0];
    app_context->active_mesh.faces[face_index].base_color[1] = app_context->visual_state.face_color[1];
    app_context->active_mesh.faces[face_index].base_color[2] = app_context->visual_state.face_color[2];
    app_context->active_mesh.faces[face_index].base_color[3] = app_context->visual_state.face_color[3];
    return PD_CORE_RESULT_OK;
}

static PdCoreResult pd_app_viewport_controller_local_get_selected_face(
    const PdAppContextEntity* app_context,
    uint32_t* face_index)
{
    if (app_context == 0 || face_index == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (app_context->selection_state.kind != PD_EDITOR_SELECTION_KIND_FACE ||
        app_context->selection_state.primary_index >= app_context->active_mesh.face_count) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    *face_index = app_context->selection_state.primary_index;
    return PD_CORE_RESULT_OK;
}

static PdCoreResult pd_app_viewport_controller_local_apply_modeling_command(
    PdAppContextEntity* app_context,
    PdEditorToolKind tool_kind)
{
    uint32_t face_index;
    PdCoreResult result;

    if (app_context == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    result = pd_editor_tool_state_set_active(&app_context->tool_state, tool_kind);
    if (result != PD_CORE_RESULT_OK) {
        app_context->tool_state.last_result = result;
        return result;
    }

    result = pd_app_viewport_controller_local_get_selected_face(app_context, &face_index);
    if (result != PD_CORE_RESULT_OK) {
        app_context->tool_state.last_result = result;
        return result;
    }

    result = pd_editor_modeling_service_apply(
        &app_context->active_mesh,
        face_index,
        tool_kind,
        pd_editor_modeling_service_config_default());

    app_context->tool_state.last_result = result;
    return result;
}

static PdCoreResult pd_app_viewport_controller_local_update_modeling_controls(
    PdAppContextEntity* app_context,
    int* needs_cube_model_rebuild,
    int* needs_face_highlight_rebuild)
{
    PdEditorToolKind tool_kind = PD_EDITOR_TOOL_KIND_VIEW;
    int has_modeling_command = 0;

    if (app_context == 0 || needs_cube_model_rebuild == 0 || needs_face_highlight_rebuild == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (IsKeyPressed(KEY_ONE)) {
        tool_kind = PD_EDITOR_TOOL_KIND_INSET;
        has_modeling_command = 1;
    }

    if (IsKeyPressed(KEY_TWO)) {
        tool_kind = PD_EDITOR_TOOL_KIND_EXTRUDE;
        has_modeling_command = 1;
    }

    if (IsKeyPressed(KEY_THREE)) {
        tool_kind = PD_EDITOR_TOOL_KIND_BEVEL;
        has_modeling_command = 1;
    }

    if (IsKeyPressed(KEY_FOUR)) {
        tool_kind = PD_EDITOR_TOOL_KIND_LOOP_CUT;
        has_modeling_command = 1;
    }

    if (!has_modeling_command) {
        return PD_CORE_RESULT_OK;
    }

    if (pd_app_viewport_controller_local_apply_modeling_command(app_context, tool_kind) != PD_CORE_RESULT_OK) {
        return PD_CORE_RESULT_OK;
    }

    *needs_cube_model_rebuild = 1;
    *needs_face_highlight_rebuild = 1;
    return PD_CORE_RESULT_OK;
}

static PdCoreResult pd_app_viewport_controller_local_apply_smoke_case(
    PdAppContextEntity* app_context,
    const char* smoke_case)
{
    if (app_context == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (smoke_case == 0 || smoke_case[0] == '\0') {
        return PD_CORE_RESULT_OK;
    }

    if (strcmp(smoke_case, "visual-default") == 0) {
        return PD_CORE_RESULT_OK;
    }

    if (strcmp(smoke_case, "visual-edge-low") == 0) {
        app_context->visual_state.edge_depth_threshold = 0.001f;
        app_context->visual_state.edge_normal_threshold = 1.0f;
        return PD_CORE_RESULT_OK;
    }

    if (strcmp(smoke_case, "visual-edge-high") == 0) {
        app_context->visual_state.edge_depth_threshold = 0.08f;
        app_context->visual_state.edge_normal_threshold = 1.0f;
        return PD_CORE_RESULT_OK;
    }

    if (strcmp(smoke_case, "visual-normal-low") == 0) {
        app_context->visual_state.edge_depth_threshold = 0.08f;
        app_context->visual_state.edge_normal_threshold = 0.02f;
        return PD_CORE_RESULT_OK;
    }

    if (strcmp(smoke_case, "visual-normal-high") == 0) {
        app_context->visual_state.edge_depth_threshold = 0.08f;
        app_context->visual_state.edge_normal_threshold = 1.0f;
        return PD_CORE_RESULT_OK;
    }

    if (strcmp(smoke_case, "visual-dark-low") == 0) {
        app_context->visual_state.dark_intensity = 0.05f;
        return PD_CORE_RESULT_OK;
    }

    if (strcmp(smoke_case, "visual-dark-high") == 0) {
        app_context->visual_state.dark_intensity = 0.9f;
        return PD_CORE_RESULT_OK;
    }

    if (strcmp(smoke_case, "transform-move-right") == 0) {
        app_context->transform_state.position[0] = 0.8f;
        return PD_CORE_RESULT_OK;
    }

    if (strcmp(smoke_case, "modeling-bevel") == 0) {
        return pd_app_viewport_controller_local_apply_modeling_command(app_context, PD_EDITOR_TOOL_KIND_BEVEL);
    }

    if (strcmp(smoke_case, "modeling-loop-cut") == 0) {
        return pd_app_viewport_controller_local_apply_modeling_command(app_context, PD_EDITOR_TOOL_KIND_LOOP_CUT);
    }

    return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
}

static PdCoreResult pd_app_viewport_controller_local_update_visual_controls(
    PdAppContextEntity* app_context,
    int* needs_cube_model_rebuild)
{
    if (app_context == 0 || needs_cube_model_rebuild == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    *needs_cube_model_rebuild = 0;

    if (IsKeyPressed(KEY_C)) {
        (void)pd_editor_tool_state_set_active(&app_context->tool_state, PD_EDITOR_TOOL_KIND_COLOR);
        if (pd_editor_visual_state_cycle_face_color(&app_context->visual_state, 1) != PD_CORE_RESULT_OK ||
            pd_app_viewport_controller_local_apply_selected_face_color(app_context) != PD_CORE_RESULT_OK) {
            return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
        }

        *needs_cube_model_rebuild = 1;
    }

    if (IsKeyPressed(KEY_B) &&
        pd_editor_visual_state_cycle_background_color(&app_context->visual_state, 1) != PD_CORE_RESULT_OK) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (IsKeyPressed(KEY_LEFT_BRACKET) &&
        pd_editor_visual_state_adjust_edge_sample_radius(
            &app_context->visual_state,
            -PD_APP_VIEWPORT_CONTROLLER_EDGE_SAMPLE_RADIUS_STEP) != PD_CORE_RESULT_OK) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (IsKeyPressed(KEY_RIGHT_BRACKET) &&
        pd_editor_visual_state_adjust_edge_sample_radius(
            &app_context->visual_state,
            PD_APP_VIEWPORT_CONTROLLER_EDGE_SAMPLE_RADIUS_STEP) != PD_CORE_RESULT_OK) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (IsKeyPressed(KEY_MINUS) &&
        pd_editor_visual_state_adjust_edge_depth_threshold(
            &app_context->visual_state,
            -PD_APP_VIEWPORT_CONTROLLER_EDGE_DEPTH_STEP) != PD_CORE_RESULT_OK) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (IsKeyPressed(KEY_EQUAL) &&
        pd_editor_visual_state_adjust_edge_depth_threshold(
            &app_context->visual_state,
            PD_APP_VIEWPORT_CONTROLLER_EDGE_DEPTH_STEP) != PD_CORE_RESULT_OK) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (IsKeyPressed(KEY_SEMICOLON) &&
        pd_editor_visual_state_adjust_edge_normal_threshold(
            &app_context->visual_state,
            -PD_APP_VIEWPORT_CONTROLLER_EDGE_NORMAL_STEP) != PD_CORE_RESULT_OK) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (IsKeyPressed(KEY_APOSTROPHE) &&
        pd_editor_visual_state_adjust_edge_normal_threshold(
            &app_context->visual_state,
            PD_APP_VIEWPORT_CONTROLLER_EDGE_NORMAL_STEP) != PD_CORE_RESULT_OK) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (IsKeyPressed(KEY_U) &&
        pd_editor_visual_state_adjust_dark_intensity(
            &app_context->visual_state,
            -PD_APP_VIEWPORT_CONTROLLER_DARK_INTENSITY_STEP) != PD_CORE_RESULT_OK) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (IsKeyPressed(KEY_I) &&
        pd_editor_visual_state_adjust_dark_intensity(
            &app_context->visual_state,
            PD_APP_VIEWPORT_CONTROLLER_DARK_INTENSITY_STEP) != PD_CORE_RESULT_OK) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (IsKeyPressed(KEY_O) &&
        pd_editor_visual_state_adjust_shadow_strength(
            &app_context->visual_state,
            -PD_APP_VIEWPORT_CONTROLLER_SHADOW_ALPHA_STEP) != PD_CORE_RESULT_OK) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (IsKeyPressed(KEY_P) &&
        pd_editor_visual_state_adjust_shadow_strength(
            &app_context->visual_state,
            PD_APP_VIEWPORT_CONTROLLER_SHADOW_ALPHA_STEP) != PD_CORE_RESULT_OK) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (IsKeyDown(KEY_LEFT) &&
        pd_editor_visual_state_adjust_light_direction(
            &app_context->visual_state,
            -PD_APP_VIEWPORT_CONTROLLER_LIGHT_STEP,
            0.0f,
            0.0f) != PD_CORE_RESULT_OK) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (IsKeyDown(KEY_RIGHT) &&
        pd_editor_visual_state_adjust_light_direction(
            &app_context->visual_state,
            PD_APP_VIEWPORT_CONTROLLER_LIGHT_STEP,
            0.0f,
            0.0f) != PD_CORE_RESULT_OK) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (IsKeyDown(KEY_UP) &&
        pd_editor_visual_state_adjust_light_direction(
            &app_context->visual_state,
            0.0f,
            PD_APP_VIEWPORT_CONTROLLER_LIGHT_STEP,
            0.0f) != PD_CORE_RESULT_OK) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (IsKeyDown(KEY_DOWN) &&
        pd_editor_visual_state_adjust_light_direction(
            &app_context->visual_state,
            0.0f,
            -PD_APP_VIEWPORT_CONTROLLER_LIGHT_STEP,
            0.0f) != PD_CORE_RESULT_OK) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (IsKeyPressed(KEY_K) &&
        pd_editor_visual_state_adjust_shadow_offset(
            &app_context->visual_state,
            -PD_APP_VIEWPORT_CONTROLLER_SHADOW_OFFSET_STEP,
            0.0f) != PD_CORE_RESULT_OK) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (IsKeyPressed(KEY_L) &&
        pd_editor_visual_state_adjust_shadow_offset(
            &app_context->visual_state,
            PD_APP_VIEWPORT_CONTROLLER_SHADOW_OFFSET_STEP,
            0.0f) != PD_CORE_RESULT_OK) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (IsKeyPressed(KEY_N) &&
        pd_editor_visual_state_adjust_shadow_offset(
            &app_context->visual_state,
            0.0f,
            -PD_APP_VIEWPORT_CONTROLLER_SHADOW_OFFSET_STEP) != PD_CORE_RESULT_OK) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (IsKeyPressed(KEY_M) &&
        pd_editor_visual_state_adjust_shadow_offset(
            &app_context->visual_state,
            0.0f,
            PD_APP_VIEWPORT_CONTROLLER_SHADOW_OFFSET_STEP) != PD_CORE_RESULT_OK) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    return PD_CORE_RESULT_OK;
}

static void pd_app_viewport_controller_local_update_panel_shortcuts(PdEditorPanelState* panel_state)
{
    if (panel_state == 0) {
        return;
    }

    if (IsKeyPressed(KEY_F1)) {
        (void)pd_editor_panel_state_set_active(panel_state, PD_EDITOR_PANEL_KIND_MODELING);
    }

    if (IsKeyPressed(KEY_F2)) {
        (void)pd_editor_panel_state_set_active(panel_state, PD_EDITOR_PANEL_KIND_TRANSFORM);
    }

    if (IsKeyPressed(KEY_F3)) {
        (void)pd_editor_panel_state_set_active(panel_state, PD_EDITOR_PANEL_KIND_VISUAL);
    }

    if (IsKeyPressed(KEY_F4)) {
        (void)pd_editor_panel_state_set_active(panel_state, PD_EDITOR_PANEL_KIND_LIGHTING);
    }

    if (IsKeyPressed(KEY_TAB)) {
        pd_editor_panel_state_toggle(panel_state);
    }
}

static PdCoreResult pd_app_viewport_controller_local_rebuild_face_highlight(
    PdRenderMeshBuffer* face_highlight_buffer,
    Model* face_highlight_model,
    int* has_face_highlight_model,
    const PdCoreMeshEntity* mesh_entity,
    uint32_t face_index,
    PdRenderFaceHighlightConfig face_highlight_config);

static void pd_app_viewport_controller_local_draw_panel_tabs(PdEditorPanelState* panel_state, Rectangle panel_rect)
{
    Rectangle tab_rect;
    float tab_width = (panel_rect.width - 24.0f) / 4.0f;

    if (panel_state == 0) {
        return;
    }

    tab_rect = (Rectangle){ panel_rect.x + 10.0f, panel_rect.y + 34.0f, tab_width, 28.0f };
    if (pd_app_viewport_controller_local_panel_button(
            tab_rect,
            "Model",
            panel_state->active_panel == PD_EDITOR_PANEL_KIND_MODELING)) {
        (void)pd_editor_panel_state_set_active(panel_state, PD_EDITOR_PANEL_KIND_MODELING);
    }

    tab_rect.x += tab_width + 2.0f;
    if (pd_app_viewport_controller_local_panel_button(
            tab_rect,
            "Move",
            panel_state->active_panel == PD_EDITOR_PANEL_KIND_TRANSFORM)) {
        (void)pd_editor_panel_state_set_active(panel_state, PD_EDITOR_PANEL_KIND_TRANSFORM);
    }

    tab_rect.x += tab_width + 2.0f;
    if (pd_app_viewport_controller_local_panel_button(
            tab_rect,
            "Visual",
            panel_state->active_panel == PD_EDITOR_PANEL_KIND_VISUAL)) {
        (void)pd_editor_panel_state_set_active(panel_state, PD_EDITOR_PANEL_KIND_VISUAL);
    }

    tab_rect.x += tab_width + 2.0f;
    if (pd_app_viewport_controller_local_panel_button(
            tab_rect,
            "Light",
            panel_state->active_panel == PD_EDITOR_PANEL_KIND_LIGHTING)) {
        (void)pd_editor_panel_state_set_active(panel_state, PD_EDITOR_PANEL_KIND_LIGHTING);
    }
}

static PdCoreResult pd_app_viewport_controller_local_rebuild_panel_models(
    PdAppContextEntity* app_context,
    PdRenderMeshBuffer* render_mesh_buffer,
    Model* cube_model,
    PdRenderMeshBuffer* face_highlight_buffer,
    Model* face_highlight_model,
    int* has_face_highlight_model,
    PdRenderFaceHighlightConfig face_highlight_config)
{
    uint32_t face_index = PD_CORE_MESH_ENTITY_INVALID_INDEX;

    if (app_context == 0 || render_mesh_buffer == 0 || cube_model == 0 || face_highlight_buffer == 0 ||
        face_highlight_model == 0 || has_face_highlight_model == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (pd_app_viewport_controller_local_rebuild_cube_model(
            render_mesh_buffer,
            cube_model,
            &app_context->active_mesh) != PD_CORE_RESULT_OK) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (app_context->selection_state.kind == PD_EDITOR_SELECTION_KIND_FACE &&
        app_context->selection_state.primary_index < app_context->active_mesh.face_count) {
        face_index = app_context->selection_state.primary_index;
    }

    return pd_app_viewport_controller_local_rebuild_face_highlight(
        face_highlight_buffer,
        face_highlight_model,
        has_face_highlight_model,
        &app_context->active_mesh,
        face_index,
        face_highlight_config);
}

static PdCoreResult pd_app_viewport_controller_local_draw_modeling_panel(
    PdAppContextEntity* app_context,
    PdRenderMeshBuffer* render_mesh_buffer,
    Model* cube_model,
    PdRenderMeshBuffer* face_highlight_buffer,
    Model* face_highlight_model,
    int* has_face_highlight_model,
    PdRenderFaceHighlightConfig face_highlight_config,
    float panel_x,
    float* panel_y)
{
    Rectangle button_rect;
    PdEditorToolKind tool_kind = PD_EDITOR_TOOL_KIND_VIEW;
    int should_apply = 0;

    if (app_context == 0 || panel_y == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    pd_app_viewport_controller_local_draw_panel_text("Modeling", panel_x, *panel_y, 12);
    *panel_y += 20.0f;

    button_rect = (Rectangle){ panel_x, *panel_y, 132.0f, 28.0f };
    if (pd_app_viewport_controller_local_panel_button(button_rect, "Inset", 0)) {
        tool_kind = PD_EDITOR_TOOL_KIND_INSET;
        should_apply = 1;
    }

    button_rect.x += 142.0f;
    if (pd_app_viewport_controller_local_panel_button(button_rect, "Extrude", 0)) {
        tool_kind = PD_EDITOR_TOOL_KIND_EXTRUDE;
        should_apply = 1;
    }

    *panel_y += 36.0f;
    button_rect = (Rectangle){ panel_x, *panel_y, 132.0f, 28.0f };
    if (pd_app_viewport_controller_local_panel_button(button_rect, "Bevel", 0)) {
        tool_kind = PD_EDITOR_TOOL_KIND_BEVEL;
        should_apply = 1;
    }

    button_rect.x += 142.0f;
    if (pd_app_viewport_controller_local_panel_button(button_rect, "Loop cut", 0)) {
        tool_kind = PD_EDITOR_TOOL_KIND_LOOP_CUT;
        should_apply = 1;
    }

    *panel_y += 46.0f;
    pd_app_viewport_controller_local_draw_panel_text(
        "Select a face, then click a modeling button.",
        panel_x,
        *panel_y,
        10);
    *panel_y += 18.0f;

    if (should_apply &&
        pd_app_viewport_controller_local_apply_modeling_command(app_context, tool_kind) == PD_CORE_RESULT_OK) {
        return pd_app_viewport_controller_local_rebuild_panel_models(
            app_context,
            render_mesh_buffer,
            cube_model,
            face_highlight_buffer,
            face_highlight_model,
            has_face_highlight_model,
            face_highlight_config);
    }

    return PD_CORE_RESULT_OK;
}

static void pd_app_viewport_controller_local_draw_transform_panel(
    PdAppContextEntity* app_context,
    float panel_x,
    float* panel_y)
{
    Rectangle slider_rect;
    Rectangle button_rect;

    if (app_context == 0 || panel_y == 0) {
        return;
    }

    pd_app_viewport_controller_local_draw_panel_text("Transform", panel_x, *panel_y, 12);
    *panel_y += 34.0f;
    slider_rect = (Rectangle){ panel_x, *panel_y, 274.0f, 12.0f };
    (void)pd_app_viewport_controller_local_panel_slider(
        slider_rect,
        "Position X",
        -3.0f,
        3.0f,
        &app_context->transform_state.position[0]);
    slider_rect.y += 38.0f;
    (void)pd_app_viewport_controller_local_panel_slider(
        slider_rect,
        "Position Y",
        -3.0f,
        3.0f,
        &app_context->transform_state.position[1]);
    slider_rect.y += 38.0f;
    (void)pd_app_viewport_controller_local_panel_slider(
        slider_rect,
        "Position Z",
        -3.0f,
        3.0f,
        &app_context->transform_state.position[2]);
    slider_rect.y += 38.0f;
    (void)pd_app_viewport_controller_local_panel_slider(
        slider_rect,
        "Rotate Y",
        -180.0f,
        180.0f,
        &app_context->transform_state.rotation_degrees[1]);
    slider_rect.y += 38.0f;
    (void)pd_app_viewport_controller_local_panel_slider(
        slider_rect,
        "Scale",
        0.2f,
        2.5f,
        &app_context->transform_state.scale[0]);
    app_context->transform_state.scale[1] = app_context->transform_state.scale[0];
    app_context->transform_state.scale[2] = app_context->transform_state.scale[0];

    *panel_y = slider_rect.y + 30.0f;
    button_rect = (Rectangle){ panel_x, *panel_y, 132.0f, 28.0f };
    if (pd_app_viewport_controller_local_panel_button(button_rect, "Reset", 0)) {
        (void)pd_editor_transform_state_reset(&app_context->transform_state);
    }
    *panel_y += 38.0f;
}

static PdCoreResult pd_app_viewport_controller_local_draw_visual_panel(
    PdAppContextEntity* app_context,
    PdRenderMeshBuffer* render_mesh_buffer,
    Model* cube_model,
    float panel_x,
    float* panel_y)
{
    Rectangle slider_rect;
    int needs_cube_rebuild = 0;

    if (app_context == 0 || render_mesh_buffer == 0 || cube_model == 0 || panel_y == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    pd_app_viewport_controller_local_draw_panel_text("Face color", panel_x, *panel_y, 12);
    *panel_y += 34.0f;
    slider_rect = (Rectangle){ panel_x, *panel_y, 274.0f, 12.0f };
    needs_cube_rebuild |= pd_app_viewport_controller_local_panel_u8_slider(
        slider_rect,
        "Face R",
        &app_context->visual_state.face_color[0]);
    slider_rect.y += 38.0f;
    needs_cube_rebuild |= pd_app_viewport_controller_local_panel_u8_slider(
        slider_rect,
        "Face G",
        &app_context->visual_state.face_color[1]);
    slider_rect.y += 38.0f;
    needs_cube_rebuild |= pd_app_viewport_controller_local_panel_u8_slider(
        slider_rect,
        "Face B",
        &app_context->visual_state.face_color[2]);
    if (needs_cube_rebuild) {
        (void)pd_editor_tool_state_set_active(&app_context->tool_state, PD_EDITOR_TOOL_KIND_COLOR);
        if (pd_app_viewport_controller_local_apply_selected_face_color(app_context) != PD_CORE_RESULT_OK ||
            pd_app_viewport_controller_local_rebuild_cube_model(render_mesh_buffer, cube_model, &app_context->active_mesh) !=
                PD_CORE_RESULT_OK) {
            return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
        }
    }

    *panel_y = slider_rect.y + 34.0f;
    pd_app_viewport_controller_local_draw_panel_text("Background", panel_x, *panel_y, 12);
    *panel_y += 34.0f;
    slider_rect = (Rectangle){ panel_x, *panel_y, 274.0f, 12.0f };
    (void)pd_app_viewport_controller_local_panel_u8_slider(
        slider_rect,
        "BG R",
        &app_context->visual_state.background_color[0]);
    slider_rect.y += 38.0f;
    (void)pd_app_viewport_controller_local_panel_u8_slider(
        slider_rect,
        "BG G",
        &app_context->visual_state.background_color[1]);
    slider_rect.y += 38.0f;
    (void)pd_app_viewport_controller_local_panel_u8_slider(
        slider_rect,
        "BG B",
        &app_context->visual_state.background_color[2]);

    *panel_y = slider_rect.y + 34.0f;
    pd_app_viewport_controller_local_draw_panel_text("Outline", panel_x, *panel_y, 12);
    *panel_y += 34.0f;
    slider_rect = (Rectangle){ panel_x, *panel_y, 274.0f, 12.0f };
    (void)pd_app_viewport_controller_local_panel_slider(
        slider_rect,
        "Radius",
        0.5f,
        3.0f,
        &app_context->visual_state.edge_sample_radius);
    slider_rect.y += 38.0f;
    (void)pd_app_viewport_controller_local_panel_slider(
        slider_rect,
        "Depth",
        0.001f,
        0.08f,
        &app_context->visual_state.edge_depth_threshold);
    slider_rect.y += 38.0f;
    (void)pd_app_viewport_controller_local_panel_slider(
        slider_rect,
        "Normal",
        0.02f,
        1.0f,
        &app_context->visual_state.edge_normal_threshold);

    *panel_y = slider_rect.y + 30.0f;
    return PD_CORE_RESULT_OK;
}

static void pd_app_viewport_controller_local_draw_lighting_panel(
    PdAppContextEntity* app_context,
    float panel_x,
    float* panel_y)
{
    Rectangle slider_rect;
    float shadow_alpha;
    int light_changed = 0;

    if (app_context == 0 || panel_y == 0) {
        return;
    }

    pd_app_viewport_controller_local_draw_panel_text("Lighting", panel_x, *panel_y, 12);
    *panel_y += 34.0f;
    slider_rect = (Rectangle){ panel_x, *panel_y, 274.0f, 12.0f };
    light_changed |= pd_app_viewport_controller_local_panel_slider(
        slider_rect,
        "Light X",
        -1.0f,
        1.0f,
        &app_context->visual_state.light_direction[0]);
    slider_rect.y += 38.0f;
    light_changed |= pd_app_viewport_controller_local_panel_slider(
        slider_rect,
        "Light Y",
        -1.0f,
        1.0f,
        &app_context->visual_state.light_direction[1]);
    slider_rect.y += 38.0f;
    light_changed |= pd_app_viewport_controller_local_panel_slider(
        slider_rect,
        "Light Z",
        -1.0f,
        1.0f,
        &app_context->visual_state.light_direction[2]);
    if (light_changed) {
        pd_app_viewport_controller_local_normalize_light_direction(app_context->visual_state.light_direction);
    }

    slider_rect.y += 38.0f;
    (void)pd_app_viewport_controller_local_panel_slider(
        slider_rect,
        "Dark",
        0.05f,
        0.9f,
        &app_context->visual_state.dark_intensity);
    slider_rect.y += 38.0f;
    shadow_alpha = (float)app_context->visual_state.shadow_color[3];
    if (pd_app_viewport_controller_local_panel_slider(slider_rect, "Shadow", 0.0f, 255.0f, &shadow_alpha)) {
        app_context->visual_state.shadow_color[3] = pd_app_viewport_controller_local_float_to_u8(shadow_alpha);
    }
    slider_rect.y += 38.0f;
    (void)pd_app_viewport_controller_local_panel_slider(
        slider_rect,
        "Shadow X",
        -3.0f,
        3.0f,
        &app_context->visual_state.shadow_offset_x);
    slider_rect.y += 38.0f;
    (void)pd_app_viewport_controller_local_panel_slider(
        slider_rect,
        "Shadow Z",
        -3.0f,
        3.0f,
        &app_context->visual_state.shadow_offset_z);
    *panel_y = slider_rect.y + 30.0f;
}

static PdCoreResult pd_app_viewport_controller_local_update_and_draw_panel(
    PdAppContextEntity* app_context,
    PdRenderMeshBuffer* render_mesh_buffer,
    Model* cube_model,
    PdRenderMeshBuffer* face_highlight_buffer,
    Model* face_highlight_model,
    int* has_face_highlight_model,
    PdRenderFaceHighlightConfig face_highlight_config)
{
    Rectangle panel_rect;
    float panel_x;
    float panel_y;
    char title[96];

    if (app_context == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (!app_context->panel_state.is_open) {
        DrawRectangle(12, GetScreenHeight() - 34, 188, 24, (Color){ 20u, 23u, 28u, 172u });
        DrawText("Panel hidden - press Tab", 20, GetScreenHeight() - 28, 10, (Color){ 236u, 240u, 244u, 220u });
        return PD_CORE_RESULT_OK;
    }

    panel_rect = pd_app_viewport_controller_local_get_panel_rect();
    panel_x = panel_rect.x + 16.0f;
    panel_y = panel_rect.y + 74.0f;

    DrawRectangleRec(panel_rect, (Color){ 18u, 21u, 27u, 212u });
    DrawRectangleLinesEx(panel_rect, 1.0f, (Color){ 232u, 238u, 245u, 88u });
    (void)snprintf(
        title,
        sizeof(title),
        "Controls  %s",
        pd_editor_panel_state_get_name(app_context->panel_state.active_panel));
    pd_app_viewport_controller_local_draw_panel_text(title, panel_rect.x + 12.0f, panel_rect.y + 12.0f, 13);
    pd_app_viewport_controller_local_draw_panel_tabs(&app_context->panel_state, panel_rect);

    switch (app_context->panel_state.active_panel) {
        case PD_EDITOR_PANEL_KIND_MODELING:
            return pd_app_viewport_controller_local_draw_modeling_panel(
                app_context,
                render_mesh_buffer,
                cube_model,
                face_highlight_buffer,
                face_highlight_model,
                has_face_highlight_model,
                face_highlight_config,
                panel_x,
                &panel_y);
        case PD_EDITOR_PANEL_KIND_TRANSFORM:
            pd_app_viewport_controller_local_draw_transform_panel(app_context, panel_x, &panel_y);
            return PD_CORE_RESULT_OK;
        case PD_EDITOR_PANEL_KIND_VISUAL:
            return pd_app_viewport_controller_local_draw_visual_panel(
                app_context,
                render_mesh_buffer,
                cube_model,
                panel_x,
                &panel_y);
        case PD_EDITOR_PANEL_KIND_LIGHTING:
            pd_app_viewport_controller_local_draw_lighting_panel(app_context, panel_x, &panel_y);
            return PD_CORE_RESULT_OK;
        default:
            return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }
}

static void pd_app_viewport_controller_local_draw_overlay_line(const char* text, int* y)
{
    if (text == 0 || y == 0) {
        return;
    }

    DrawText(text, 20, *y, 10, (Color){ 236u, 240u, 244u, 235u });
    *y += 14;
}

static void pd_app_viewport_controller_local_draw_overlay(
    const PdAppContextEntity* app_context,
    PdRenderVisualConfig visual_config,
    PdRenderShadowConfig shadow_config)
{
    char line[128];
    int y = 20;
    const PdEditorTransformState* transform_state;

    if (app_context == 0) {
        return;
    }

    transform_state = &app_context->transform_state;
    DrawRectangle(12, 12, 372, 170, (Color){ 20u, 23u, 28u, 168u });
    DrawRectangleLines(12, 12, 372, 170, (Color){ 245u, 245u, 245u, 64u });

    (void)snprintf(line, sizeof(line), "tool %s", pd_editor_tool_state_get_name(app_context->tool_state.active_tool));
    pd_app_viewport_controller_local_draw_overlay_line(line, &y);

    (void)snprintf(
        line,
        sizeof(line),
        "result %s",
        pd_app_viewport_controller_local_get_result_name(app_context->tool_state.last_result));
    pd_app_viewport_controller_local_draw_overlay_line(line, &y);

    if (app_context->selection_state.kind == PD_EDITOR_SELECTION_KIND_FACE) {
        (void)snprintf(
            line,
            sizeof(line),
            "selection face=%u",
            (unsigned int)app_context->selection_state.primary_index);
    } else {
        (void)snprintf(line, sizeof(line), "selection none");
    }
    pd_app_viewport_controller_local_draw_overlay_line(line, &y);

    (void)snprintf(
        line,
        sizeof(line),
        "pos %.2f %.2f %.2f  rotY %.1f  scale %.2f",
        transform_state->position[0],
        transform_state->position[1],
        transform_state->position[2],
        transform_state->rotation_degrees[1],
        transform_state->scale[0]);
    pd_app_viewport_controller_local_draw_overlay_line(line, &y);

    (void)snprintf(
        line,
        sizeof(line),
        "face rgb %u %u %u  bg rgb %u %u %u",
        app_context->visual_state.face_color[0],
        app_context->visual_state.face_color[1],
        app_context->visual_state.face_color[2],
        app_context->visual_state.background_color[0],
        app_context->visual_state.background_color[1],
        app_context->visual_state.background_color[2]);
    pd_app_viewport_controller_local_draw_overlay_line(line, &y);

    (void)snprintf(
        line,
        sizeof(line),
        "edge radius %.2f  depth %.4f  normal %.3f",
        visual_config.edge_sample_radius,
        visual_config.edge_depth_threshold,
        visual_config.edge_normal_threshold);
    pd_app_viewport_controller_local_draw_overlay_line(line, &y);

    (void)snprintf(
        line,
        sizeof(line),
        "light %.2f %.2f %.2f  dark %.2f",
        visual_config.light_direction.x,
        visual_config.light_direction.y,
        visual_config.light_direction.z,
        visual_config.dark_intensity);
    pd_app_viewport_controller_local_draw_overlay_line(line, &y);

    (void)snprintf(
        line,
        sizeof(line),
        "shadow alpha %u  offset %.2f %.2f",
        shadow_config.color.a,
        shadow_config.offset_x,
        shadow_config.offset_z);
    pd_app_viewport_controller_local_draw_overlay_line(line, &y);
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

static PdCoreResult pd_app_viewport_controller_local_resize_targets(
    PdRenderTargetController* target_controller,
    int width,
    int height,
    Shader edge_shader,
    int edge_texel_size_location,
    Vector2* edge_texel_size)
{
    int render_width;
    int render_height;

    if (target_controller == 0 || edge_texel_size == 0 || width <= 0 || height <= 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    render_width = width * PD_APP_VIEWPORT_CONTROLLER_RENDER_SCALE;
    render_height = height * PD_APP_VIEWPORT_CONTROLLER_RENDER_SCALE;

    if (target_controller->config.width == render_width && target_controller->config.height == render_height &&
        pd_render_target_controller_is_ready(target_controller)) {
        return PD_CORE_RESULT_OK;
    }

    pd_render_target_controller_free(target_controller);
    if (pd_render_target_controller_init(target_controller, pd_render_target_config_make(render_width, render_height)) !=
        PD_CORE_RESULT_OK) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    SetTextureFilter(target_controller->color_depth_target.texture, TEXTURE_FILTER_BILINEAR);
    edge_texel_size->x = 1.0f / (float)render_width;
    edge_texel_size->y = 1.0f / (float)render_height;
    SetShaderValue(edge_shader, edge_texel_size_location, edge_texel_size, SHADER_UNIFORM_VEC2);
    return PD_CORE_RESULT_OK;
}

static void pd_app_viewport_controller_local_render_targets(
    PdRenderTargetController* target_controller,
    PdEngineCameraState camera_state,
    PdRenderVisualConfig visual_config,
    PdRenderShadowConfig shadow_config,
    Model* cube_model,
    Model* face_highlight_model,
    int has_face_highlight_model,
    Matrix object_transform,
    Shader hardstep_shader,
    Shader depth_shader,
    Shader normal_shader)
{
    Vector3 origin = { 0.0f, 0.0f, 0.0f };
    Color normal_background = { 128u, 128u, 255u, 255u };
    Color depth_background = { 255u, 255u, 255u, 255u };

    cube_model->materials[0].shader = hardstep_shader;
    cube_model->transform = object_transform;
    BeginTextureMode(target_controller->color_depth_target);
    ClearBackground(visual_config.background_color);
    BeginMode3D(camera_state.camera);
    pd_app_viewport_controller_local_draw_shadow(shadow_config);
    DrawModel(*cube_model, origin, 1.0f, WHITE);
    if (has_face_highlight_model) {
        face_highlight_model->materials[0].shader = hardstep_shader;
        face_highlight_model->transform = object_transform;
        BeginBlendMode(BLEND_ALPHA);
        DrawModel(*face_highlight_model, origin, 1.0f, WHITE);
        EndBlendMode();
    }
    EndMode3D();
    EndTextureMode();

    cube_model->materials[0].shader = depth_shader;
    cube_model->transform = object_transform;
    BeginTextureMode(target_controller->depth_target);
    ClearBackground(depth_background);
    BeginMode3D(camera_state.camera);
    DrawModel(*cube_model, origin, 1.0f, WHITE);
    EndMode3D();
    EndTextureMode();

    cube_model->materials[0].shader = normal_shader;
    cube_model->transform = object_transform;
    BeginTextureMode(target_controller->normal_target);
    ClearBackground(normal_background);
    BeginMode3D(camera_state.camera);
    DrawModel(*cube_model, origin, 1.0f, WHITE);
    EndMode3D();
    EndTextureMode();
}

static void pd_app_viewport_controller_local_unload_face_highlight_model(
    Model* face_highlight_model,
    int* has_face_highlight_model)
{
    if (face_highlight_model == 0 || has_face_highlight_model == 0 || !*has_face_highlight_model) {
        return;
    }

    face_highlight_model->materials[0].shader = (Shader){ 0 };
    UnloadModel(*face_highlight_model);
    *face_highlight_model = (Model){ 0 };
    *has_face_highlight_model = 0;
}

static PdCoreResult pd_app_viewport_controller_local_rebuild_face_highlight(
    PdRenderMeshBuffer* face_highlight_buffer,
    Model* face_highlight_model,
    int* has_face_highlight_model,
    const PdCoreMeshEntity* mesh_entity,
    uint32_t face_index,
    PdRenderFaceHighlightConfig face_highlight_config)
{
    Mesh face_highlight_mesh;

    if (face_highlight_buffer == 0 || face_highlight_model == 0 || has_face_highlight_model == 0 ||
        mesh_entity == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    pd_app_viewport_controller_local_unload_face_highlight_model(face_highlight_model, has_face_highlight_model);
    pd_render_mesh_buffer_free(face_highlight_buffer);
    if (face_index == PD_CORE_MESH_ENTITY_INVALID_INDEX) {
        return PD_CORE_RESULT_OK;
    }

    if (pd_render_face_highlight_buffer_build_for_face(
            face_highlight_buffer,
            mesh_entity,
            face_index,
            face_highlight_config) != PD_CORE_RESULT_OK) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    face_highlight_mesh = pd_app_viewport_controller_local_make_mesh(face_highlight_buffer);
    *face_highlight_model = LoadModelFromMesh(face_highlight_mesh);
    *has_face_highlight_model = 1;
    return PD_CORE_RESULT_OK;
}

static PdCoreResult pd_app_viewport_controller_local_pick_face(
    PdAppContextEntity* app_context,
    PdRenderMeshBuffer* face_highlight_buffer,
    Model* face_highlight_model,
    int* has_face_highlight_model,
    PdRenderFaceHighlightConfig face_highlight_config,
    Camera3D camera,
    const PdEditorTransformState* transform_state)
{
    Ray mouse_ray;
    PdEditorPickServiceHit hit;
    Matrix object_transform;
    Matrix inverse_transform;
    Vector3 local_origin;
    Vector3 local_end;
    Vector3 local_direction;
    Vector3 world_end;
    float ray_origin[3];
    float ray_direction[3];
    uint32_t face_index = PD_CORE_MESH_ENTITY_INVALID_INDEX;

    if (app_context == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    mouse_ray = GetMouseRay(GetMousePosition(), camera);
    object_transform = pd_app_viewport_controller_local_make_object_transform(transform_state);
    inverse_transform = MatrixInvert(object_transform);
    world_end = pd_app_viewport_controller_local_add(mouse_ray.position, mouse_ray.direction);
    local_origin = Vector3Transform(mouse_ray.position, inverse_transform);
    local_end = Vector3Transform(world_end, inverse_transform);
    local_direction = Vector3Normalize(pd_app_viewport_controller_local_subtract(local_end, local_origin));
    ray_origin[0] = local_origin.x;
    ray_origin[1] = local_origin.y;
    ray_origin[2] = local_origin.z;
    ray_direction[0] = local_direction.x;
    ray_direction[1] = local_direction.y;
    ray_direction[2] = local_direction.z;

    if (pd_editor_pick_service_pick_face(&app_context->active_mesh, ray_origin, ray_direction, &hit) !=
        PD_CORE_RESULT_OK) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (hit.has_hit) {
        face_index = hit.face_index;
        if (pd_editor_selection_state_select_face(&app_context->selection_state, face_index) != PD_CORE_RESULT_OK) {
            return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
        }
    } else {
        pd_editor_selection_state_clear(&app_context->selection_state);
    }

    return pd_app_viewport_controller_local_rebuild_face_highlight(
        face_highlight_buffer,
        face_highlight_model,
        has_face_highlight_model,
        &app_context->active_mesh,
        face_index,
        face_highlight_config);
}

int main(int argc, char** argv)
{
    PdAppContextEntity app_context;
    PdEngineWindowConfig window_config = pd_engine_window_config_default();
    PdEngineCameraState camera_state = pd_engine_camera_controller_make_default();
    PdRenderFaceHighlightConfig face_highlight_config = pd_render_face_highlight_config_default();
    PdRenderVisualConfig visual_config;
    PdRenderShadowConfig shadow_config;
    PdRenderHardstepShaderConfig shader_config = pd_render_hardstep_shader_config_default();
    PdRenderDepthShaderConfig depth_shader_config = pd_render_depth_shader_config_default();
    PdRenderNormalShaderConfig normal_shader_config = pd_render_normal_shader_config_default();
    PdRenderEdgeShaderConfig edge_shader_config = pd_render_edge_shader_config_default();
    PdRenderTargetController target_controller = { 0 };
    PdRenderMeshBuffer render_mesh_buffer = { 0 };
    PdRenderMeshBuffer face_highlight_buffer = { 0 };
    Mesh cube_mesh;
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
    int edge_sample_radius_location;
    int edge_depth_threshold_location;
    int edge_normal_threshold_location;
    Vector2 edge_texel_size;
    Rectangle screen_source;
    Rectangle screen_destination;
    Vector2 screen_position = { 0.0f, 0.0f };
    int is_interactive = pd_app_viewport_controller_local_has_argument(argc, argv, "--interactive");
    const char* smoke_case = pd_app_viewport_controller_local_get_argument_value(argc, argv, "--smoke-case");
    int has_face_highlight_model = 0;
    int needs_cube_model_rebuild = 0;
    int needs_face_highlight_rebuild = 0;
    int run_result = 0;
    char capture_path[256] = "captures/phase2_cube.png";

    if (pd_app_lifecycle_controller_init(&app_context) != PD_CORE_RESULT_OK) {
        return 1;
    }

    if (pd_editor_selection_state_select_face(&app_context.selection_state, 1u) != PD_CORE_RESULT_OK) {
        pd_app_lifecycle_controller_shutdown(&app_context);
        return 1;
    }

    if (pd_app_viewport_controller_local_apply_smoke_case(&app_context, smoke_case) != PD_CORE_RESULT_OK) {
        pd_app_lifecycle_controller_shutdown(&app_context);
        return 1;
    }

    if (smoke_case != 0 && smoke_case[0] != '\0') {
        (void)snprintf(capture_path, sizeof(capture_path), "captures/%s.png", smoke_case);
    }

    if (pd_render_mesh_buffer_build_from_mesh(&render_mesh_buffer, &app_context.active_mesh) != PD_CORE_RESULT_OK) {
        pd_app_lifecycle_controller_shutdown(&app_context);
        return 1;
    }

    SetConfigFlags((is_interactive ? PD_APP_VIEWPORT_CONTROLLER_INTERACTIVE_WINDOW_FLAGS : FLAG_WINDOW_HIDDEN) |
                   FLAG_MSAA_4X_HINT);
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
    visual_config = pd_app_viewport_controller_local_make_visual_config(&app_context.visual_state);
    shadow_config = pd_app_viewport_controller_local_make_object_shadow_config(
        pd_app_viewport_controller_local_make_shadow_config(&app_context.visual_state),
        &app_context.transform_state);
    shader_config.light_direction = visual_config.light_direction;
    shader_config.dark_intensity = visual_config.dark_intensity;
    SetShaderValue(hardstep_shader, light_direction_location, &visual_config.light_direction, SHADER_UNIFORM_VEC3);
    SetShaderValue(hardstep_shader, dark_intensity_location, &visual_config.dark_intensity, SHADER_UNIFORM_FLOAT);
    edge_normal_texture_location = GetShaderLocation(edge_shader, "normalTexture");
    edge_depth_texture_location = GetShaderLocation(edge_shader, "depthTexture");
    edge_texel_size_location = GetShaderLocation(edge_shader, "texelSize");
    edge_sample_radius_location = GetShaderLocation(edge_shader, "edgeSampleRadius");
    edge_depth_threshold_location = GetShaderLocation(edge_shader, "edgeDepthThreshold");
    edge_normal_threshold_location = GetShaderLocation(edge_shader, "edgeNormalThreshold");
    edge_texel_size.x = 1.0f / (float)window_config.width;
    edge_texel_size.y = 1.0f / (float)window_config.height;
    SetShaderValue(edge_shader, edge_texel_size_location, &edge_texel_size, SHADER_UNIFORM_VEC2);
    edge_shader_config.edge_sample_radius = visual_config.edge_sample_radius;
    edge_shader_config.edge_depth_threshold = visual_config.edge_depth_threshold;
    edge_shader_config.edge_normal_threshold = visual_config.edge_normal_threshold;
    SetShaderValue(edge_shader, edge_sample_radius_location, &visual_config.edge_sample_radius, SHADER_UNIFORM_FLOAT);
    SetShaderValue(edge_shader, edge_depth_threshold_location, &visual_config.edge_depth_threshold, SHADER_UNIFORM_FLOAT);
    SetShaderValue(edge_shader, edge_normal_threshold_location, &visual_config.edge_normal_threshold, SHADER_UNIFORM_FLOAT);

    cube_mesh = pd_app_viewport_controller_local_make_mesh(&render_mesh_buffer);
    cube_model = LoadModelFromMesh(cube_mesh);
    if (pd_app_viewport_controller_local_rebuild_face_highlight(
            &face_highlight_buffer,
            &face_highlight_model,
            &has_face_highlight_model,
            &app_context.active_mesh,
            app_context.selection_state.primary_index,
            face_highlight_config) != PD_CORE_RESULT_OK) {
        UnloadModel(cube_model);
        UnloadShader(edge_shader);
        UnloadShader(normal_shader);
        UnloadShader(depth_shader);
        UnloadShader(hardstep_shader);
        pd_render_target_controller_free(&target_controller);
        CloseWindow();
        pd_render_mesh_buffer_free(&render_mesh_buffer);
        pd_app_lifecycle_controller_shutdown(&app_context);
        return 1;
    }

    do {
        int screen_width = GetScreenWidth();
        int screen_height = GetScreenHeight();

        if (is_interactive) {
            needs_cube_model_rebuild = 0;
            needs_face_highlight_rebuild = 0;
            pd_app_viewport_controller_local_update_panel_shortcuts(&app_context.panel_state);
            pd_app_viewport_controller_local_update_camera(&camera_state);
            pd_app_viewport_controller_local_update_transform_tool(&app_context.tool_state);
            pd_app_viewport_controller_local_update_transform(&app_context.transform_state);
            if (pd_app_viewport_controller_local_update_visual_controls(
                    &app_context,
                    &needs_cube_model_rebuild) != PD_CORE_RESULT_OK) {
                run_result = 1;
                break;
            }

            if (pd_app_viewport_controller_local_update_modeling_controls(
                    &app_context,
                    &needs_cube_model_rebuild,
                    &needs_face_highlight_rebuild) != PD_CORE_RESULT_OK) {
                run_result = 1;
                break;
            }

            if (needs_cube_model_rebuild &&
                pd_app_viewport_controller_local_rebuild_cube_model(
                    &render_mesh_buffer,
                    &cube_model,
                    &app_context.active_mesh) != PD_CORE_RESULT_OK) {
                run_result = 1;
                break;
            }

            if (needs_face_highlight_rebuild &&
                pd_app_viewport_controller_local_rebuild_face_highlight(
                    &face_highlight_buffer,
                    &face_highlight_model,
                    &has_face_highlight_model,
                    &app_context.active_mesh,
                    app_context.selection_state.primary_index,
                    face_highlight_config) != PD_CORE_RESULT_OK) {
                run_result = 1;
                break;
            }

            if (!pd_app_viewport_controller_local_is_panel_mouse_target(&app_context) &&
                IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
                pd_app_viewport_controller_local_pick_face(
                    &app_context,
                    &face_highlight_buffer,
                    &face_highlight_model,
                    &has_face_highlight_model,
                    face_highlight_config,
                    camera_state.camera,
                    &app_context.transform_state) != PD_CORE_RESULT_OK) {
                run_result = 1;
                break;
            }
        }

        visual_config = pd_app_viewport_controller_local_make_visual_config(&app_context.visual_state);
        shadow_config = pd_app_viewport_controller_local_make_object_shadow_config(
            pd_app_viewport_controller_local_make_shadow_config(&app_context.visual_state),
            &app_context.transform_state);
        SetShaderValue(hardstep_shader, light_direction_location, &visual_config.light_direction, SHADER_UNIFORM_VEC3);
        SetShaderValue(hardstep_shader, dark_intensity_location, &visual_config.dark_intensity, SHADER_UNIFORM_FLOAT);
        SetShaderValue(
            edge_shader,
            edge_sample_radius_location,
            &visual_config.edge_sample_radius,
            SHADER_UNIFORM_FLOAT);
        SetShaderValue(
            edge_shader,
            edge_depth_threshold_location,
            &visual_config.edge_depth_threshold,
            SHADER_UNIFORM_FLOAT);
        SetShaderValue(
            edge_shader,
            edge_normal_threshold_location,
            &visual_config.edge_normal_threshold,
            SHADER_UNIFORM_FLOAT);

        if (pd_app_viewport_controller_local_resize_targets(
                &target_controller,
                screen_width,
                screen_height,
                edge_shader,
                edge_texel_size_location,
                &edge_texel_size) != PD_CORE_RESULT_OK) {
            run_result = 1;
            break;
        }

        pd_app_viewport_controller_local_render_targets(
            &target_controller,
            camera_state,
            visual_config,
            shadow_config,
            &cube_model,
            &face_highlight_model,
            has_face_highlight_model,
            pd_app_viewport_controller_local_make_object_transform(&app_context.transform_state),
            hardstep_shader,
            depth_shader,
            normal_shader);

        screen_source = (Rectangle){ 0.0f,
                                     0.0f,
                                     (float)target_controller.color_depth_target.texture.width,
                                     -(float)target_controller.color_depth_target.texture.height };
        screen_destination = (Rectangle){ 0.0f, 0.0f, (float)screen_width, (float)screen_height };
        BeginDrawing();
        ClearBackground(visual_config.background_color);
        BeginShaderMode(edge_shader);
        SetShaderValueTexture(edge_shader, edge_normal_texture_location, target_controller.normal_target.texture);
        SetShaderValueTexture(edge_shader, edge_depth_texture_location, target_controller.depth_target.texture);
        DrawTexturePro(
            target_controller.color_depth_target.texture,
            screen_source,
            screen_destination,
            screen_position,
            0.0f,
            WHITE);
        EndShaderMode();
        if (is_interactive) {
            pd_app_viewport_controller_local_draw_overlay(&app_context, visual_config, shadow_config);
            if (pd_app_viewport_controller_local_update_and_draw_panel(
                    &app_context,
                    &render_mesh_buffer,
                    &cube_model,
                    &face_highlight_buffer,
                    &face_highlight_model,
                    &has_face_highlight_model,
                    face_highlight_config) != PD_CORE_RESULT_OK) {
                run_result = 1;
            }
        }
        EndDrawing();
        if (run_result != 0) {
            break;
        }
    } while (is_interactive && !WindowShouldClose());

    if (run_result == 0 && !is_interactive) {
        MakeDirectory("captures");
        TakeScreenshot(capture_path);
    }

    cube_model.materials[0].shader = (Shader){ 0 };
    pd_app_viewport_controller_local_unload_face_highlight_model(&face_highlight_model, &has_face_highlight_model);
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
    return run_result;
}
