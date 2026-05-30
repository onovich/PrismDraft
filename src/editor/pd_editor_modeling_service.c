#include "prismdraft/editor/pd_editor_modeling_service.h"

#include "prismdraft/core/pd_core_face_bevel_controller.h"
#include "prismdraft/core/pd_core_face_extrude_controller.h"
#include "prismdraft/core/pd_core_face_inset_controller.h"
#include "prismdraft/core/pd_core_face_loop_cut_controller.h"

PdEditorModelingServiceConfig pd_editor_modeling_service_config_default(void)
{
    PdEditorModelingServiceConfig config;

    config.inset_ratio = 0.2f;
    config.extrude_distance = 0.25f;
    config.bevel_inset_ratio = 0.15f;
    config.bevel_distance = 0.16f;
    return config;
}

PdCoreResult pd_editor_modeling_service_apply(
    PdCoreMeshEntity* mesh_entity,
    uint32_t face_index,
    PdEditorToolKind tool_kind,
    PdEditorModelingServiceConfig config)
{
    if (mesh_entity == 0 || face_index == PD_CORE_MESH_ENTITY_INVALID_INDEX || face_index >= mesh_entity->face_count) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    switch (tool_kind) {
        case PD_EDITOR_TOOL_KIND_INSET:
            return pd_core_face_inset_controller_apply(mesh_entity, face_index, config.inset_ratio);
        case PD_EDITOR_TOOL_KIND_EXTRUDE:
            return pd_core_face_extrude_controller_apply(mesh_entity, face_index, config.extrude_distance);
        case PD_EDITOR_TOOL_KIND_BEVEL:
            return pd_core_face_bevel_controller_apply(
                mesh_entity,
                face_index,
                config.bevel_inset_ratio,
                config.bevel_distance);
        case PD_EDITOR_TOOL_KIND_LOOP_CUT:
            return pd_core_face_loop_cut_controller_apply_quad(mesh_entity, face_index);
        default:
            return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }
}
