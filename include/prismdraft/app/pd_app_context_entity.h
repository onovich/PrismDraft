#ifndef PRISMDRAFT_APP_PD_APP_CONTEXT_ENTITY_H
#define PRISMDRAFT_APP_PD_APP_CONTEXT_ENTITY_H

#include "prismdraft/core/pd_core_mesh_entity.h"

typedef struct PdAppContextEntity {
    PdCoreMeshEntity active_mesh;
    int is_running;
} PdAppContextEntity;

#endif
