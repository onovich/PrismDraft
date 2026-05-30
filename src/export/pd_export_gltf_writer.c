#include "prismdraft/export/pd_export_gltf_writer.h"

#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char PD_EXPORT_GLTF_WRITER_BASE64_TABLE[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static FILE* pd_export_gltf_writer_local_open_write_binary(const char* file_path)
{
#if defined(_MSC_VER)
    FILE* file = 0;
    if (fopen_s(&file, file_path, "wb") != 0) {
        return 0;
    }
    return file;
#else
    return fopen(file_path, "wb");
#endif
}

static PdCoreResult pd_export_gltf_writer_local_checked_add_size(size_t left, size_t right, size_t* result)
{
    if (result == 0 || left > (SIZE_MAX - right)) {
        return PD_CORE_RESULT_ERROR_OUT_OF_MEMORY;
    }

    *result = left + right;
    return PD_CORE_RESULT_OK;
}

static PdCoreResult pd_export_gltf_writer_local_checked_mul_size(size_t left, size_t right, size_t* result)
{
    if (result == 0 || (right != 0u && left > (SIZE_MAX / right))) {
        return PD_CORE_RESULT_ERROR_OUT_OF_MEMORY;
    }

    *result = left * right;
    return PD_CORE_RESULT_OK;
}

static PdCoreResult pd_export_gltf_writer_local_encode_base64(
    const unsigned char* bytes,
    size_t byte_count,
    char** output_text)
{
    size_t output_length;
    size_t input_index = 0u;
    size_t output_index = 0u;
    char* text;

    if (output_text == 0 || (bytes == 0 && byte_count > 0u)) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (pd_export_gltf_writer_local_checked_mul_size((byte_count + 2u) / 3u, 4u, &output_length) !=
        PD_CORE_RESULT_OK) {
        return PD_CORE_RESULT_ERROR_OUT_OF_MEMORY;
    }

    text = (char*)malloc(output_length + 1u);
    if (text == 0) {
        return PD_CORE_RESULT_ERROR_OUT_OF_MEMORY;
    }

    while (input_index < byte_count) {
        size_t remaining = byte_count - input_index;
        unsigned int octet_a = bytes[input_index++];
        unsigned int octet_b = remaining > 1u ? bytes[input_index++] : 0u;
        unsigned int octet_c = remaining > 2u ? bytes[input_index++] : 0u;
        unsigned int triple = (octet_a << 16u) | (octet_b << 8u) | octet_c;

        text[output_index++] = PD_EXPORT_GLTF_WRITER_BASE64_TABLE[(triple >> 18u) & 0x3Fu];
        text[output_index++] = PD_EXPORT_GLTF_WRITER_BASE64_TABLE[(triple >> 12u) & 0x3Fu];
        text[output_index++] = remaining > 1u ? PD_EXPORT_GLTF_WRITER_BASE64_TABLE[(triple >> 6u) & 0x3Fu] : '=';
        text[output_index++] = remaining > 2u ? PD_EXPORT_GLTF_WRITER_BASE64_TABLE[triple & 0x3Fu] : '=';
    }

    text[output_length] = '\0';
    *output_text = text;
    return PD_CORE_RESULT_OK;
}

static void pd_export_gltf_writer_local_compute_position_bounds(
    const PdExportMeshBuffer* export_mesh_buffer,
    float min_position[3],
    float max_position[3])
{
    uint32_t vertex_index;

    if (export_mesh_buffer->vertex_count == 0u) {
        min_position[0] = 0.0f;
        min_position[1] = 0.0f;
        min_position[2] = 0.0f;
        max_position[0] = 0.0f;
        max_position[1] = 0.0f;
        max_position[2] = 0.0f;
        return;
    }

    min_position[0] = FLT_MAX;
    min_position[1] = FLT_MAX;
    min_position[2] = FLT_MAX;
    max_position[0] = -FLT_MAX;
    max_position[1] = -FLT_MAX;
    max_position[2] = -FLT_MAX;

    for (vertex_index = 0u; vertex_index < export_mesh_buffer->vertex_count; vertex_index++) {
        const PdExportMeshBufferVertex* vertex = &export_mesh_buffer->vertices[vertex_index];
        int component_index;

        for (component_index = 0; component_index < 3; component_index++) {
            if (vertex->position[component_index] < min_position[component_index]) {
                min_position[component_index] = vertex->position[component_index];
            }

            if (vertex->position[component_index] > max_position[component_index]) {
                max_position[component_index] = vertex->position[component_index];
            }
        }
    }
}

static PdCoreResult pd_export_gltf_writer_local_build_binary(
    const PdExportMeshBuffer* export_mesh_buffer,
    unsigned char** binary_bytes,
    size_t* position_byte_length,
    size_t* normal_byte_length,
    size_t* color_byte_length,
    size_t* total_byte_length)
{
    size_t vertex_count;
    size_t vertex_index;
    size_t normal_offset;
    size_t color_offset;
    unsigned char* bytes;
    PdCoreResult result;

    if (export_mesh_buffer == 0 || binary_bytes == 0 || position_byte_length == 0 || normal_byte_length == 0 ||
        color_byte_length == 0 || total_byte_length == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    vertex_count = (size_t)export_mesh_buffer->vertex_count;
    result = pd_export_gltf_writer_local_checked_mul_size(vertex_count, 3u * sizeof(float), position_byte_length);
    if (result != PD_CORE_RESULT_OK) {
        return result;
    }

    result = pd_export_gltf_writer_local_checked_mul_size(vertex_count, 3u * sizeof(float), normal_byte_length);
    if (result != PD_CORE_RESULT_OK) {
        return result;
    }

    result = pd_export_gltf_writer_local_checked_mul_size(vertex_count, 4u * sizeof(float), color_byte_length);
    if (result != PD_CORE_RESULT_OK) {
        return result;
    }

    result = pd_export_gltf_writer_local_checked_add_size(*position_byte_length, *normal_byte_length, &color_offset);
    if (result != PD_CORE_RESULT_OK) {
        return result;
    }

    result = pd_export_gltf_writer_local_checked_add_size(color_offset, *color_byte_length, total_byte_length);
    if (result != PD_CORE_RESULT_OK) {
        return result;
    }

    bytes = (unsigned char*)malloc(*total_byte_length == 0u ? 1u : *total_byte_length);
    if (bytes == 0) {
        return PD_CORE_RESULT_ERROR_OUT_OF_MEMORY;
    }

    normal_offset = *position_byte_length;
    for (vertex_index = 0u; vertex_index < vertex_count; vertex_index++) {
        const PdExportMeshBufferVertex* vertex = &export_mesh_buffer->vertices[vertex_index];
        memcpy(bytes + (vertex_index * 3u * sizeof(float)), vertex->position, 3u * sizeof(float));
        memcpy(bytes + normal_offset + (vertex_index * 3u * sizeof(float)), vertex->normal, 3u * sizeof(float));
        memcpy(bytes + color_offset + (vertex_index * 4u * sizeof(float)), vertex->color, 4u * sizeof(float));
    }

    *binary_bytes = bytes;
    return PD_CORE_RESULT_OK;
}

PdCoreResult pd_export_gltf_writer_write_file(const char* file_path, const PdExportMeshBuffer* export_mesh_buffer)
{
    FILE* file;
    unsigned char* binary_bytes = 0;
    char* base64_text = 0;
    float min_position[3];
    float max_position[3];
    size_t position_byte_length = 0u;
    size_t normal_byte_length = 0u;
    size_t color_byte_length = 0u;
    size_t total_byte_length = 0u;
    size_t normal_byte_offset;
    size_t color_byte_offset;
    PdCoreResult result;

    if (file_path == 0 || export_mesh_buffer == 0 ||
        (export_mesh_buffer->vertex_count > 0u && export_mesh_buffer->vertices == 0)) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    result = pd_export_gltf_writer_local_build_binary(
        export_mesh_buffer,
        &binary_bytes,
        &position_byte_length,
        &normal_byte_length,
        &color_byte_length,
        &total_byte_length);
    if (result != PD_CORE_RESULT_OK) {
        return result;
    }

    result = pd_export_gltf_writer_local_encode_base64(binary_bytes, total_byte_length, &base64_text);
    if (result != PD_CORE_RESULT_OK) {
        free(binary_bytes);
        return result;
    }

    pd_export_gltf_writer_local_compute_position_bounds(export_mesh_buffer, min_position, max_position);
    normal_byte_offset = position_byte_length;
    color_byte_offset = position_byte_length + normal_byte_length;

    file = pd_export_gltf_writer_local_open_write_binary(file_path);
    if (file == 0) {
        free(base64_text);
        free(binary_bytes);
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    fprintf(file, "{\n");
    fprintf(file, "  \"asset\": { \"version\": \"2.0\", \"generator\": \"PrismDraft\" },\n");
    fprintf(file, "  \"scene\": 0,\n");
    fprintf(file, "  \"scenes\": [{ \"nodes\": [0] }],\n");
    fprintf(file, "  \"nodes\": [{ \"mesh\": 0 }],\n");
    fprintf(file, "  \"meshes\": [{ \"primitives\": [{ \"attributes\": { \"POSITION\": 0, \"NORMAL\": 1, \"COLOR_0\": 2 }, \"mode\": 4 }] }],\n");
    fprintf(file, "  \"buffers\": [{ \"byteLength\": %llu, \"uri\": \"data:application/octet-stream;base64,%s\" }],\n",
        (unsigned long long)total_byte_length,
        base64_text);
    fprintf(file, "  \"bufferViews\": [\n");
    fprintf(file, "    { \"buffer\": 0, \"byteOffset\": 0, \"byteLength\": %llu },\n",
        (unsigned long long)position_byte_length);
    fprintf(file, "    { \"buffer\": 0, \"byteOffset\": %llu, \"byteLength\": %llu },\n",
        (unsigned long long)normal_byte_offset,
        (unsigned long long)normal_byte_length);
    fprintf(file, "    { \"buffer\": 0, \"byteOffset\": %llu, \"byteLength\": %llu }\n",
        (unsigned long long)color_byte_offset,
        (unsigned long long)color_byte_length);
    fprintf(file, "  ],\n");
    fprintf(file, "  \"accessors\": [\n");
    fprintf(file,
        "    { \"bufferView\": 0, \"componentType\": 5126, \"count\": %u, \"type\": \"VEC3\", \"min\": [%g, %g, %g], \"max\": [%g, %g, %g] },\n",
        (unsigned int)export_mesh_buffer->vertex_count,
        min_position[0],
        min_position[1],
        min_position[2],
        max_position[0],
        max_position[1],
        max_position[2]);
    fprintf(file, "    { \"bufferView\": 1, \"componentType\": 5126, \"count\": %u, \"type\": \"VEC3\" },\n",
        (unsigned int)export_mesh_buffer->vertex_count);
    fprintf(file, "    { \"bufferView\": 2, \"componentType\": 5126, \"count\": %u, \"type\": \"VEC4\" }\n",
        (unsigned int)export_mesh_buffer->vertex_count);
    fprintf(file, "  ]\n");
    fprintf(file, "}\n");

    fclose(file);
    free(base64_text);
    free(binary_bytes);
    return PD_CORE_RESULT_OK;
}
