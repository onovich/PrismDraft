#include "prismdraft/export/pd_export_glb_writer.h"

#include <float.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static FILE* pd_export_glb_writer_local_open_write_binary(const char* file_path)
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

static PdCoreResult pd_export_glb_writer_local_checked_add_size(size_t left, size_t right, size_t* result)
{
    if (result == 0 || left > (SIZE_MAX - right)) {
        return PD_CORE_RESULT_ERROR_OUT_OF_MEMORY;
    }

    *result = left + right;
    return PD_CORE_RESULT_OK;
}

static PdCoreResult pd_export_glb_writer_local_checked_mul_size(size_t left, size_t right, size_t* result)
{
    if (result == 0 || (right != 0u && left > (SIZE_MAX / right))) {
        return PD_CORE_RESULT_ERROR_OUT_OF_MEMORY;
    }

    *result = left * right;
    return PD_CORE_RESULT_OK;
}

static PdCoreResult pd_export_glb_writer_local_pad_to_four(size_t byte_count, size_t* padded_byte_count)
{
    size_t addend;

    if (padded_byte_count == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    addend = (4u - (byte_count % 4u)) % 4u;
    return pd_export_glb_writer_local_checked_add_size(byte_count, addend, padded_byte_count);
}

static void pd_export_glb_writer_local_compute_position_bounds(
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

static PdCoreResult pd_export_glb_writer_local_build_binary(
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
    result = pd_export_glb_writer_local_checked_mul_size(vertex_count, 3u * sizeof(float), position_byte_length);
    if (result != PD_CORE_RESULT_OK) {
        return result;
    }

    result = pd_export_glb_writer_local_checked_mul_size(vertex_count, 3u * sizeof(float), normal_byte_length);
    if (result != PD_CORE_RESULT_OK) {
        return result;
    }

    result = pd_export_glb_writer_local_checked_mul_size(vertex_count, 4u * sizeof(float), color_byte_length);
    if (result != PD_CORE_RESULT_OK) {
        return result;
    }

    result = pd_export_glb_writer_local_checked_add_size(*position_byte_length, *normal_byte_length, &color_offset);
    if (result != PD_CORE_RESULT_OK) {
        return result;
    }

    result = pd_export_glb_writer_local_checked_add_size(color_offset, *color_byte_length, total_byte_length);
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

static PdCoreResult pd_export_glb_writer_local_format_json(
    const PdExportMeshBuffer* export_mesh_buffer,
    size_t position_byte_length,
    size_t normal_byte_length,
    size_t color_byte_length,
    size_t total_byte_length,
    char** json_text,
    size_t* json_text_length)
{
    float min_position[3];
    float max_position[3];
    size_t normal_byte_offset;
    size_t color_byte_offset;
    int required_length;
    char* text;

    if (export_mesh_buffer == 0 || json_text == 0 || json_text_length == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    pd_export_glb_writer_local_compute_position_bounds(export_mesh_buffer, min_position, max_position);
    normal_byte_offset = position_byte_length;
    color_byte_offset = position_byte_length + normal_byte_length;

    required_length = snprintf(
        0,
        0u,
        "{"
        "\"asset\":{\"version\":\"2.0\",\"generator\":\"PrismDraft\"},"
        "\"scene\":0,"
        "\"scenes\":[{\"nodes\":[0]}],"
        "\"nodes\":[{\"mesh\":0}],"
        "\"meshes\":[{\"primitives\":[{\"attributes\":{\"POSITION\":0,\"NORMAL\":1,\"COLOR_0\":2},\"mode\":4}]}],"
        "\"buffers\":[{\"byteLength\":%llu}],"
        "\"bufferViews\":["
        "{\"buffer\":0,\"byteOffset\":0,\"byteLength\":%llu},"
        "{\"buffer\":0,\"byteOffset\":%llu,\"byteLength\":%llu},"
        "{\"buffer\":0,\"byteOffset\":%llu,\"byteLength\":%llu}"
        "],"
        "\"accessors\":["
        "{\"bufferView\":0,\"componentType\":5126,\"count\":%u,\"type\":\"VEC3\",\"min\":[%g,%g,%g],\"max\":[%g,%g,%g]},"
        "{\"bufferView\":1,\"componentType\":5126,\"count\":%u,\"type\":\"VEC3\"},"
        "{\"bufferView\":2,\"componentType\":5126,\"count\":%u,\"type\":\"VEC4\"}"
        "]"
        "}",
        (unsigned long long)total_byte_length,
        (unsigned long long)position_byte_length,
        (unsigned long long)normal_byte_offset,
        (unsigned long long)normal_byte_length,
        (unsigned long long)color_byte_offset,
        (unsigned long long)color_byte_length,
        (unsigned int)export_mesh_buffer->vertex_count,
        min_position[0],
        min_position[1],
        min_position[2],
        max_position[0],
        max_position[1],
        max_position[2],
        (unsigned int)export_mesh_buffer->vertex_count,
        (unsigned int)export_mesh_buffer->vertex_count);
    if (required_length < 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    text = (char*)malloc((size_t)required_length + 1u);
    if (text == 0) {
        return PD_CORE_RESULT_ERROR_OUT_OF_MEMORY;
    }

    snprintf(
        text,
        (size_t)required_length + 1u,
        "{"
        "\"asset\":{\"version\":\"2.0\",\"generator\":\"PrismDraft\"},"
        "\"scene\":0,"
        "\"scenes\":[{\"nodes\":[0]}],"
        "\"nodes\":[{\"mesh\":0}],"
        "\"meshes\":[{\"primitives\":[{\"attributes\":{\"POSITION\":0,\"NORMAL\":1,\"COLOR_0\":2},\"mode\":4}]}],"
        "\"buffers\":[{\"byteLength\":%llu}],"
        "\"bufferViews\":["
        "{\"buffer\":0,\"byteOffset\":0,\"byteLength\":%llu},"
        "{\"buffer\":0,\"byteOffset\":%llu,\"byteLength\":%llu},"
        "{\"buffer\":0,\"byteOffset\":%llu,\"byteLength\":%llu}"
        "],"
        "\"accessors\":["
        "{\"bufferView\":0,\"componentType\":5126,\"count\":%u,\"type\":\"VEC3\",\"min\":[%g,%g,%g],\"max\":[%g,%g,%g]},"
        "{\"bufferView\":1,\"componentType\":5126,\"count\":%u,\"type\":\"VEC3\"},"
        "{\"bufferView\":2,\"componentType\":5126,\"count\":%u,\"type\":\"VEC4\"}"
        "]"
        "}",
        (unsigned long long)total_byte_length,
        (unsigned long long)position_byte_length,
        (unsigned long long)normal_byte_offset,
        (unsigned long long)normal_byte_length,
        (unsigned long long)color_byte_offset,
        (unsigned long long)color_byte_length,
        (unsigned int)export_mesh_buffer->vertex_count,
        min_position[0],
        min_position[1],
        min_position[2],
        max_position[0],
        max_position[1],
        max_position[2],
        (unsigned int)export_mesh_buffer->vertex_count,
        (unsigned int)export_mesh_buffer->vertex_count);

    *json_text = text;
    *json_text_length = (size_t)required_length;
    return PD_CORE_RESULT_OK;
}

static PdCoreResult pd_export_glb_writer_local_write_u32_le(FILE* file, uint32_t value)
{
    unsigned char bytes[4];

    if (file == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    bytes[0] = (unsigned char)(value & 0xFFu);
    bytes[1] = (unsigned char)((value >> 8u) & 0xFFu);
    bytes[2] = (unsigned char)((value >> 16u) & 0xFFu);
    bytes[3] = (unsigned char)((value >> 24u) & 0xFFu);

    return fwrite(bytes, 1u, sizeof(bytes), file) == sizeof(bytes) ? PD_CORE_RESULT_OK
                                                                   : PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
}

static PdCoreResult pd_export_glb_writer_local_write_padding(FILE* file, size_t padding_count, unsigned char value)
{
    size_t padding_index;

    if (file == 0) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    for (padding_index = 0u; padding_index < padding_count; padding_index++) {
        if (fputc((int)value, file) == EOF) {
            return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
        }
    }

    return PD_CORE_RESULT_OK;
}

PdCoreResult pd_export_glb_writer_write_file(const char* file_path, const PdExportMeshBuffer* export_mesh_buffer)
{
    FILE* file;
    unsigned char* binary_bytes = 0;
    char* json_text = 0;
    size_t position_byte_length = 0u;
    size_t normal_byte_length = 0u;
    size_t color_byte_length = 0u;
    size_t binary_byte_length = 0u;
    size_t binary_chunk_length = 0u;
    size_t json_text_length = 0u;
    size_t json_chunk_length = 0u;
    size_t file_byte_length = 0u;
    PdCoreResult result;

    if (file_path == 0 || export_mesh_buffer == 0 ||
        (export_mesh_buffer->vertex_count > 0u && export_mesh_buffer->vertices == 0)) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    result = pd_export_glb_writer_local_build_binary(
        export_mesh_buffer,
        &binary_bytes,
        &position_byte_length,
        &normal_byte_length,
        &color_byte_length,
        &binary_byte_length);
    if (result != PD_CORE_RESULT_OK) {
        return result;
    }

    result = pd_export_glb_writer_local_format_json(
        export_mesh_buffer,
        position_byte_length,
        normal_byte_length,
        color_byte_length,
        binary_byte_length,
        &json_text,
        &json_text_length);
    if (result != PD_CORE_RESULT_OK) {
        free(binary_bytes);
        return result;
    }

    result = pd_export_glb_writer_local_pad_to_four(json_text_length, &json_chunk_length);
    if (result == PD_CORE_RESULT_OK) {
        result = pd_export_glb_writer_local_pad_to_four(binary_byte_length, &binary_chunk_length);
    }
    if (result == PD_CORE_RESULT_OK) {
        result = pd_export_glb_writer_local_checked_add_size(12u, 8u + json_chunk_length, &file_byte_length);
    }
    if (result == PD_CORE_RESULT_OK) {
        result = pd_export_glb_writer_local_checked_add_size(file_byte_length, 8u + binary_chunk_length, &file_byte_length);
    }
    if (result != PD_CORE_RESULT_OK || file_byte_length > UINT32_MAX || json_chunk_length > UINT32_MAX ||
        binary_chunk_length > UINT32_MAX) {
        free(json_text);
        free(binary_bytes);
        return result == PD_CORE_RESULT_OK ? PD_CORE_RESULT_ERROR_OUT_OF_MEMORY : result;
    }

    file = pd_export_glb_writer_local_open_write_binary(file_path);
    if (file == 0) {
        free(json_text);
        free(binary_bytes);
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    result = pd_export_glb_writer_local_write_u32_le(file, 0x46546C67u);
    if (result == PD_CORE_RESULT_OK) {
        result = pd_export_glb_writer_local_write_u32_le(file, 2u);
    }
    if (result == PD_CORE_RESULT_OK) {
        result = pd_export_glb_writer_local_write_u32_le(file, (uint32_t)file_byte_length);
    }
    if (result == PD_CORE_RESULT_OK) {
        result = pd_export_glb_writer_local_write_u32_le(file, (uint32_t)json_chunk_length);
    }
    if (result == PD_CORE_RESULT_OK) {
        result = pd_export_glb_writer_local_write_u32_le(file, 0x4E4F534Au);
    }
    if (result == PD_CORE_RESULT_OK &&
        fwrite(json_text, 1u, json_text_length, file) != json_text_length) {
        result = PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }
    if (result == PD_CORE_RESULT_OK) {
        result = pd_export_glb_writer_local_write_padding(file, json_chunk_length - json_text_length, 0x20u);
    }
    if (result == PD_CORE_RESULT_OK) {
        result = pd_export_glb_writer_local_write_u32_le(file, (uint32_t)binary_chunk_length);
    }
    if (result == PD_CORE_RESULT_OK) {
        result = pd_export_glb_writer_local_write_u32_le(file, 0x004E4942u);
    }
    if (result == PD_CORE_RESULT_OK &&
        fwrite(binary_bytes, 1u, binary_byte_length, file) != binary_byte_length) {
        result = PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }
    if (result == PD_CORE_RESULT_OK) {
        result = pd_export_glb_writer_local_write_padding(file, binary_chunk_length - binary_byte_length, 0u);
    }

    fclose(file);
    free(json_text);
    free(binary_bytes);
    return result;
}
