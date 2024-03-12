#ifndef PLATFORM_H
#define PLATFORM_H

// Memory macros
#if defined(ESP32)
#define MEM_ATTR DRAM_ATTR
#define RAM_ATTR IRAM_ATTR
#else
#define MEM_ATTR
#define RAM_ATTR
#endif

// Core auto configuration
#if defined(ESP8266)
#define CORE 1
#else
#ifdef CONFIG_FREERTOS_UNICORE
#define CORE 1
#else
#define CORE 2
#endif
#endif

#endif