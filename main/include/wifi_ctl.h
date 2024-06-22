#pragma once

/**
 * @brief Stops the running access point
 */
void wifi_ctl_ap_stop();

/**
 * @brief Starts the access point
 */
void wifi_ctl_ap_start();

/**
 * @brief Creates and configures and access point to be run
 */
void wifi_ctl_create_ap();

/**
 * @brief Initializes the Wi-Fi controller of the ESP32
 */
void wifi_ctl_init();