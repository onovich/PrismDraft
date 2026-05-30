#ifndef PRISMDRAFT_CORE_PD_CORE_RESULT_ENTITY_H
#define PRISMDRAFT_CORE_PD_CORE_RESULT_ENTITY_H

typedef enum PdCoreResult {
    PD_CORE_RESULT_OK = 0,
    PD_CORE_RESULT_ERROR_INVALID_ARGUMENT,
    PD_CORE_RESULT_ERROR_OUT_OF_MEMORY,
    PD_CORE_RESULT_ERROR_TOPOLOGY_INVALID
} PdCoreResult;

const char* pd_core_result_entity_to_string(PdCoreResult result);

#endif
