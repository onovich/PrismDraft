#include "prismdraft/export/pd_export_glb_writer.h"

#include "prismdraft/core/pd_core_cube_fixture.h"
#include "prismdraft/core/pd_core_mesh_storage_controller.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static FILE* pd_export_glb_writer_test_local_open_read_binary(const char* file_path)
{
#if defined(_MSC_VER)
    FILE* file = 0;
    if (fopen_s(&file, file_path, "rb") != 0) {
        return 0;
    }
    return file;
#else
    return fopen(file_path, "rb");
#endif
}

static unsigned char* pd_export_glb_writer_test_local_read_file(const char* file_path, size_t* file_size)
{
    FILE* file = pd_export_glb_writer_test_local_open_read_binary(file_path);
    long local_file_size;
    unsigned char* bytes;

    assert(file != 0);
    assert(file_size != 0);
    assert(fseek(file, 0, SEEK_END) == 0);
    local_file_size = ftell(file);
    assert(local_file_size >= 0);
    assert(fseek(file, 0, SEEK_SET) == 0);

    bytes = (unsigned char*)malloc((size_t)local_file_size + 1u);
    assert(bytes != 0);
    assert(fread(bytes, 1u, (size_t)local_file_size, file) == (size_t)local_file_size);
    bytes[local_file_size] = '\0';
    fclose(file);
    *file_size = (size_t)local_file_size;
    return bytes;
}

static uint32_t pd_export_glb_writer_test_local_read_u32_le(const unsigned char* bytes, size_t byte_offset)
{
    return ((uint32_t)bytes[byte_offset]) | ((uint32_t)bytes[byte_offset + 1u] << 8u) |
           ((uint32_t)bytes[byte_offset + 2u] << 16u) | ((uint32_t)bytes[byte_offset + 3u] << 24u);
}

static void pd_export_glb_writer_test_local_writes_glb_binary(void)
{
    const char* file_path = "pd_export_glb_writer_test_output.glb";
    PdCoreMeshEntity mesh_entity = { 0 };
    PdExportMeshBuffer export_mesh_buffer = { 0 };
    unsigned char* bytes;
    char* json_text;
    size_t file_size;
    uint32_t json_chunk_length;
    uint32_t binary_chunk_offset;
    uint32_t binary_chunk_length;

    assert(pd_core_cube_fixture_build(&mesh_entity) == PD_CORE_RESULT_OK);
    assert(pd_export_mesh_buffer_build_from_mesh(&export_mesh_buffer, &mesh_entity) == PD_CORE_RESULT_OK);
    assert(pd_export_glb_writer_write_file(file_path, &export_mesh_buffer) == PD_CORE_RESULT_OK);

    bytes = pd_export_glb_writer_test_local_read_file(file_path, &file_size);
    assert(file_size > 28u);
    assert(pd_export_glb_writer_test_local_read_u32_le(bytes, 0u) == 0x46546C67u);
    assert(pd_export_glb_writer_test_local_read_u32_le(bytes, 4u) == 2u);
    assert(pd_export_glb_writer_test_local_read_u32_le(bytes, 8u) == file_size);

    json_chunk_length = pd_export_glb_writer_test_local_read_u32_le(bytes, 12u);
    assert(pd_export_glb_writer_test_local_read_u32_le(bytes, 16u) == 0x4E4F534Au);
    assert(json_chunk_length % 4u == 0u);
    assert(20u + json_chunk_length + 8u <= file_size);

    json_text = (char*)malloc((size_t)json_chunk_length + 1u);
    assert(json_text != 0);
    memcpy(json_text, bytes + 20u, json_chunk_length);
    json_text[json_chunk_length] = '\0';
    assert(strstr(json_text, "\"version\":\"2.0\"") != 0);
    assert(strstr(json_text, "\"POSITION\":0") != 0);
    assert(strstr(json_text, "\"NORMAL\":1") != 0);
    assert(strstr(json_text, "\"COLOR_0\":2") != 0);
    assert(strstr(json_text, "\"componentType\":5126") != 0);
    assert(strstr(json_text, "\"count\":36") != 0);

    binary_chunk_offset = 20u + json_chunk_length;
    binary_chunk_length = pd_export_glb_writer_test_local_read_u32_le(bytes, binary_chunk_offset);
    assert(pd_export_glb_writer_test_local_read_u32_le(bytes, binary_chunk_offset + 4u) == 0x004E4942u);
    assert(binary_chunk_length % 4u == 0u);
    assert((size_t)binary_chunk_offset + 8u + binary_chunk_length == file_size);
    assert(binary_chunk_length >= export_mesh_buffer.vertex_count * 10u * sizeof(float));

    free(json_text);
    free(bytes);
    remove(file_path);
    pd_export_mesh_buffer_free(&export_mesh_buffer);
    pd_core_mesh_storage_controller_free(&mesh_entity);
}

static void pd_export_glb_writer_test_local_rejects_invalid_arguments(void)
{
    PdExportMeshBuffer export_mesh_buffer = { 0 };

    assert(pd_export_glb_writer_write_file(0, &export_mesh_buffer) == PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);
    assert(pd_export_glb_writer_write_file("unused.glb", 0) == PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);
}

int main(void)
{
    pd_export_glb_writer_test_local_writes_glb_binary();
    pd_export_glb_writer_test_local_rejects_invalid_arguments();
    return 0;
}
