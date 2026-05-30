#include "prismdraft/editor/pd_editor_pick_service.h"

#include "prismdraft/core/pd_core_mesh_validator.h"

#include <float.h>
#include <math.h>

#define PD_EDITOR_PICK_SERVICE_EPSILON 0.000001f

static void pd_editor_pick_service_local_clear_hit(PdEditorPickServiceHit* hit)
{
    hit->has_hit = 0;
    hit->face_index = PD_CORE_MESH_ENTITY_INVALID_INDEX;
    hit->distance = FLT_MAX;
    hit->position[0] = 0.0f;
    hit->position[1] = 0.0f;
    hit->position[2] = 0.0f;
}

static void pd_editor_pick_service_local_subtract(const float left[3], const float right[3], float result[3])
{
    result[0] = left[0] - right[0];
    result[1] = left[1] - right[1];
    result[2] = left[2] - right[2];
}

static void pd_editor_pick_service_local_cross(const float left[3], const float right[3], float result[3])
{
    result[0] = (left[1] * right[2]) - (left[2] * right[1]);
    result[1] = (left[2] * right[0]) - (left[0] * right[2]);
    result[2] = (left[0] * right[1]) - (left[1] * right[0]);
}

static float pd_editor_pick_service_local_dot(const float left[3], const float right[3])
{
    return (left[0] * right[0]) + (left[1] * right[1]) + (left[2] * right[2]);
}

static int pd_editor_pick_service_local_ray_hits_triangle(
    const float ray_origin[3],
    const float ray_direction[3],
    const float vertex_a[3],
    const float vertex_b[3],
    const float vertex_c[3],
    float* distance)
{
    float edge_ab[3];
    float edge_ac[3];
    float cross_direction_ac[3];
    float determinant;
    float inverse_determinant;
    float origin_to_a[3];
    float barycentric_u;
    float cross_origin_ab[3];
    float barycentric_v;
    float ray_distance;

    pd_editor_pick_service_local_subtract(vertex_b, vertex_a, edge_ab);
    pd_editor_pick_service_local_subtract(vertex_c, vertex_a, edge_ac);
    pd_editor_pick_service_local_cross(ray_direction, edge_ac, cross_direction_ac);
    determinant = pd_editor_pick_service_local_dot(edge_ab, cross_direction_ac);

    if (fabsf(determinant) < PD_EDITOR_PICK_SERVICE_EPSILON) {
        return 0;
    }

    inverse_determinant = 1.0f / determinant;
    pd_editor_pick_service_local_subtract(ray_origin, vertex_a, origin_to_a);
    barycentric_u = inverse_determinant * pd_editor_pick_service_local_dot(origin_to_a, cross_direction_ac);
    if (barycentric_u < 0.0f || barycentric_u > 1.0f) {
        return 0;
    }

    pd_editor_pick_service_local_cross(origin_to_a, edge_ab, cross_origin_ab);
    barycentric_v = inverse_determinant * pd_editor_pick_service_local_dot(ray_direction, cross_origin_ab);
    if (barycentric_v < 0.0f || (barycentric_u + barycentric_v) > 1.0f) {
        return 0;
    }

    ray_distance = inverse_determinant * pd_editor_pick_service_local_dot(edge_ac, cross_origin_ab);
    if (ray_distance <= PD_EDITOR_PICK_SERVICE_EPSILON) {
        return 0;
    }

    *distance = ray_distance;
    return 1;
}

static void pd_editor_pick_service_local_write_position(
    const float ray_origin[3],
    const float ray_direction[3],
    float distance,
    float position[3])
{
    position[0] = ray_origin[0] + (ray_direction[0] * distance);
    position[1] = ray_origin[1] + (ray_direction[1] * distance);
    position[2] = ray_origin[2] + (ray_direction[2] * distance);
}

static void pd_editor_pick_service_local_test_face(
    const PdCoreMeshEntity* mesh_entity,
    const float ray_origin[3],
    const float ray_direction[3],
    uint32_t face_index,
    PdEditorPickServiceHit* hit)
{
    uint32_t first_half_edge_index = mesh_entity->faces[face_index].half_edge;
    uint32_t previous_half_edge_index = mesh_entity->half_edges[first_half_edge_index].next_half_edge;
    uint32_t current_half_edge_index = mesh_entity->half_edges[previous_half_edge_index].next_half_edge;

    while (current_half_edge_index != first_half_edge_index) {
        const float* first_position =
            mesh_entity->vertices[mesh_entity->half_edges[first_half_edge_index].vertex].position;
        const float* previous_position =
            mesh_entity->vertices[mesh_entity->half_edges[previous_half_edge_index].vertex].position;
        const float* current_position =
            mesh_entity->vertices[mesh_entity->half_edges[current_half_edge_index].vertex].position;
        float distance = 0.0f;

        if (pd_editor_pick_service_local_ray_hits_triangle(
                ray_origin,
                ray_direction,
                first_position,
                previous_position,
                current_position,
                &distance) &&
            distance < hit->distance) {
            hit->has_hit = 1;
            hit->face_index = face_index;
            hit->distance = distance;
            pd_editor_pick_service_local_write_position(ray_origin, ray_direction, distance, hit->position);
        }

        previous_half_edge_index = current_half_edge_index;
        current_half_edge_index = mesh_entity->half_edges[current_half_edge_index].next_half_edge;
    }
}

PdCoreResult pd_editor_pick_service_pick_face(
    const PdCoreMeshEntity* mesh_entity,
    const float ray_origin[3],
    const float ray_direction[3],
    PdEditorPickServiceHit* hit)
{
    uint32_t face_index;
    PdCoreResult result;

    if (mesh_entity == 0 || ray_origin == 0 || ray_direction == 0 || hit == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    result = pd_core_mesh_validator_check(mesh_entity);
    if (result != PD_CORE_RESULT_OK) {
        return result;
    }

    pd_editor_pick_service_local_clear_hit(hit);
    for (face_index = 0u; face_index < mesh_entity->face_count; face_index++) {
        pd_editor_pick_service_local_test_face(mesh_entity, ray_origin, ray_direction, face_index, hit);
    }

    return PD_CORE_RESULT_OK;
}
