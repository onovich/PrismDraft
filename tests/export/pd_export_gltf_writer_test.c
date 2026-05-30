#include "prismdraft/export/pd_export_gltf_writer.h"

#include "prismdraft/core/pd_core_cube_fixture.h"
#include "prismdraft/core/pd_core_mesh_storage_controller.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static FILE* pd_export_gltf_writer_test_local_open_read_binary(const char* file_path)
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

static char* pd_export_gltf_writer_test_local_read_file(const char* file_path)
{
    FILE* file = pd_export_gltf_writer_test_local_open_read_binary(file_path);
    long file_size;
    char* text;

    assert(file != 0);
    assert(fseek(file, 0, SEEK_END) == 0);
    file_size = ftell(file);
    assert(file_size >= 0);
    assert(fseek(file, 0, SEEK_SET) == 0);

    text = (char*)malloc((size_t)file_size + 1u);
    assert(text != 0);
    assert(fread(text, 1u, (size_t)file_size, file) == (size_t)file_size);
    text[file_size] = '\0';
    fclose(file);
    return text;
}

static void pd_export_gltf_writer_test_local_writes_gltf_json(void)
{
    const char* file_path = "pd_export_gltf_writer_test_output.gltf";
    PdCoreMeshEntity mesh_entity = { 0 };
    PdExportMeshBuffer export_mesh_buffer = { 0 };
    char* text;

    assert(pd_core_cube_fixture_build(&mesh_entity) == PD_CORE_RESULT_OK);
    assert(pd_export_mesh_buffer_build_from_mesh(&export_mesh_buffer, &mesh_entity) == PD_CORE_RESULT_OK);
    assert(pd_export_gltf_writer_write_file(file_path, &export_mesh_buffer) == PD_CORE_RESULT_OK);

    text = pd_export_gltf_writer_test_local_read_file(file_path);
    assert(strstr(text, "\"version\": \"2.0\"") != 0);
    assert(strstr(text, "\"POSITION\": 0") != 0);
    assert(strstr(text, "\"NORMAL\": 1") != 0);
    assert(strstr(text, "\"COLOR_0\": 2") != 0);
    assert(strstr(text, "\"componentType\": 5126") != 0);
    assert(strstr(text, "\"count\": 36") != 0);
    assert(strstr(text, "data:application/octet-stream;base64,") != 0);

    free(text);
    remove(file_path);
    pd_export_mesh_buffer_free(&export_mesh_buffer);
    pd_core_mesh_storage_controller_free(&mesh_entity);
}

static void pd_export_gltf_writer_test_local_rejects_invalid_arguments(void)
{
    PdExportMeshBuffer export_mesh_buffer = { 0 };

    assert(pd_export_gltf_writer_write_file(0, &export_mesh_buffer) == PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);
    assert(pd_export_gltf_writer_write_file("unused.gltf", 0) == PD_CORE_RESULT_ERROR_INVALID_ARGUMENT);
}

int main(void)
{
    pd_export_gltf_writer_test_local_writes_gltf_json();
    pd_export_gltf_writer_test_local_rejects_invalid_arguments();
    return 0;
}
