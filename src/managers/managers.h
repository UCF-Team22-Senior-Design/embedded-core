#pragma once

#include "state_manager.h"
#include "network_manager.h"
#include "input_manager.h"
#include "audio_manager.h"
#include "pwm_manager.h"

#ifdef PLATFORM_CONTROLLER
    #include "laser_manager.h"
    #include "display_manager.h"
#endif

#ifdef PLATFORM_TARGET
    #include "lighting_manager.h"
#endif