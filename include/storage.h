#pragma once

#include "configuration.h"
#include "esp_err.h"

/**
 * @brief Initialize storage
 * 
 * @return esp_err_t ESP_OK on success, other values indicate errors
 */
esp_err_t storage_setup();

/**
 * @brief Save configuration to storage
 * 
 * @param conf Configuration struct to save
 */
void storage_save(const Configuration *conf);

/**
 * @brief Load configuration from storage
 * 
 * @param conf Pointer to Configuration struct to load data into
 */
void storage_load(Configuration *conf);

/**
 * @brief Close storage
 * 
 * This function closes any resources opened during storage setup.
 */
void storage_close();
