#ifndef PRISMDRAFT_EXPORT_PD_EXPORT_GLB_WRITER_H
#define PRISMDRAFT_EXPORT_PD_EXPORT_GLB_WRITER_H

#include "prismdraft/core/pd_core_result_entity.h"
#include "prismdraft/export/pd_export_mesh_buffer.h"

PdCoreResult pd_export_glb_writer_write_file(
    const char* file_path,
    const PdExportMeshBuffer* export_mesh_buffer);

#endif
