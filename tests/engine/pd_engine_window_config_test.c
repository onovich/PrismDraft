#include "prismdraft/engine/pd_engine_window_config.h"

#include <assert.h>
#include <string.h>

int main(void)
{
    PdEngineWindowConfig config = pd_engine_window_config_default();

    assert(config.width == 1280);
    assert(config.height == 720);
    assert(strcmp(config.title, "PrismDraft") == 0);

    return 0;
}
