#include "prismdraft/core/pd_core_result_entity.h"

#include <assert.h>
#include <string.h>

static void pd_core_result_test_local_returns_names_for_known_results(void)
{
    assert(strcmp(pd_core_result_entity_to_string(PD_CORE_RESULT_OK), "PD_CORE_RESULT_OK") == 0);
    assert(strcmp(pd_core_result_entity_to_string(PD_CORE_RESULT_ERROR_INVALID_ARGUMENT), "PD_CORE_RESULT_ERROR_INVALID_ARGUMENT") == 0);
    assert(strcmp(pd_core_result_entity_to_string(PD_CORE_RESULT_ERROR_OUT_OF_MEMORY), "PD_CORE_RESULT_ERROR_OUT_OF_MEMORY") == 0);
    assert(strcmp(pd_core_result_entity_to_string(PD_CORE_RESULT_ERROR_TOPOLOGY_INVALID), "PD_CORE_RESULT_ERROR_TOPOLOGY_INVALID") == 0);
}

static void pd_core_result_test_local_returns_unknown_for_invalid_result(void)
{
    assert(strcmp(pd_core_result_entity_to_string((PdCoreResult)999), "PD_CORE_RESULT_UNKNOWN") == 0);
}

int main(void)
{
    pd_core_result_test_local_returns_names_for_known_results();
    pd_core_result_test_local_returns_unknown_for_invalid_result();
    return 0;
}
