#include "prismdraft/core/pd_core_result_entity.h"

const char* pd_core_result_entity_to_string(PdCoreResult result)
{
    switch (result) {
    case PD_CORE_RESULT_OK:
        return "PD_CORE_RESULT_OK";
    case PD_CORE_RESULT_ERROR_INVALID_ARGUMENT:
        return "PD_CORE_RESULT_ERROR_INVALID_ARGUMENT";
    case PD_CORE_RESULT_ERROR_OUT_OF_MEMORY:
        return "PD_CORE_RESULT_ERROR_OUT_OF_MEMORY";
    case PD_CORE_RESULT_ERROR_TOPOLOGY_INVALID:
        return "PD_CORE_RESULT_ERROR_TOPOLOGY_INVALID";
    default:
        return "PD_CORE_RESULT_UNKNOWN";
    }
}
