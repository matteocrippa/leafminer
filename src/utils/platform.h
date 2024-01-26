#ifndef PLATFORM_H
#define PLATFORM_H

// Device
#if defined(ESP8266_D)
std::string DEVICE = "esp8266";
#elif defined(GEEKMAGICCLOCK_SMALLTV)
std::string DEVICE = "geekmagic-smalltv";
#elif defined(ESP32_WROOM)
std::string DEVICE = "esp32";
#elif defined(ESP32_S2)
std::string DEVICE = "esp32-s2";
#elif defined(ESP32_S3)
std::string DEVICE = "esp32-s3";
#elif defined(DLILYGO_T_S3)
std::string DEVICE = "lilygo-t-display-s3";
#endif

// Memory macros
#if defined(ESP32)
#define MEM_ATTR DRAM_ATTR
#define RAM_ATTR IRAM_ATTR
#define MEM_ALIGN_32 alignas(32)
#define MEM_ALIGN_4 alignas(4)
#else
#define MEM_ATTR
#define RAM_ATTR
#define MEM_ALIGN_32
#define MEM_ALIGN_4
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