#include "prismdraft/core/pd_core_face_bevel_controller.h"

#include "prismdraft/core/pd_core_face_extrude_controller.h"
#include "prismdraft/core/pd_core_face_inset_controller.h"
#include "prismdraft/core/pd_core_mesh_validator.h"

PdCoreResult pd_core_face_bevel_controller_apply(
    PdCoreMeshEntity* mesh_entity,
    uint32_t face_index,
    float inset_ratio,
    float distance)
{
    PdCoreResult result;

    /*
     * Topology proof for face bevel:
     * Input invariants: mesh_entity validates, face_index is a closed face accepted by the inset and extrude
     * controllers, inset_ratio is within (0, 1), and distance is positive. Allocated additions: the inset step creates
     * an inner face and planar ring; the extrude step offsets that inner face and creates side faces. next_half_edge
     * rewiring plan: inset rewires the original boundary into ring quads, then extrude rewires the inset inner face
     * into an offset cap with side quads. pair_half_edge rewiring plan: both delegated topology controllers restore
     * pair symmetry for their generated half-edges before returning. Output invariants: the selected face remains the
     * final raised cap, surrounding bevel/step faces are closed, and pd_core_mesh_validator_check must return OK.
     */

    if (mesh_entity == 0 || face_index == PD_CORE_MESH_ENTITY_INVALID_INDEX || inset_ratio <= 0.0f ||
        inset_ratio >= 1.0f || distance <= 0.0f) {
        return PD_CORE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    result = pd_core_mesh_validator_check(mesh_entity);
    if (result != PD_CORE_RESULT_OK) {
        return result;
    }

    result = pd_core_face_inset_controller_apply(mesh_entity, face_index, inset_ratio);
    if (result != PD_CORE_RESULT_OK) {
        return result;
    }

    result = pd_core_face_extrude_controller_apply(mesh_entity, face_index, distance);
    if (result != PD_CORE_RESULT_OK) {
        return result;
    }

    return pd_core_mesh_validator_check(mesh_entity);
}
