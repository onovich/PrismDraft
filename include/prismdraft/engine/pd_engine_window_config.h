#ifndef PRISMDRAFT_ENGINE_PD_ENGINE_WINDOW_CONFIG_H
#define PRISMDRAFT_ENGINE_PD_ENGINE_WINDOW_CONFIG_H

typedef struct PdEngineWindowConfig {
    int width;
    int height;
    const char* title;
} PdEngineWindowConfig;

PdEngineWindowConfig pd_engine_window_config_default(void);

#endif
