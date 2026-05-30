#include "prismdraft/engine/pd_engine_window_config.h"

PdEngineWindowConfig pd_engine_window_config_default(void)
{
    PdEngineWindowConfig config;
    config.width = 1280;
    config.height = 720;
    config.title = "PrismDraft";
    return config;
}
