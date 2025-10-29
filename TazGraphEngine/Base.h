#pragma once

#include <RenderEngine.h>

//!on Base.h define useful macros/functions for whole project
#include "./PlatformDetection.h"

#ifdef TAZ_PLATFORM_WINDOWS
#define safe_sprintf sprintf_s
#elif defined(TAZ_PLATFORM_LINUX)
#define safe_sprintf sprintf
#endif

#ifdef TAZ_PLATFORM_WINDOWS
#define safe_sscanf sscanf_s
#elif defined(TAZ_PLATFORM_LINUX)
#define safe_sscanf sscanf
#endif

#ifdef TAZ_PLATFORM_WINDOWS
#define safe_putenv _putenv
#elif defined(TAZ_PLATFORM_LINUX)
#define safe_putenv putenv
#endif

#include "./DefineFactory.h"
