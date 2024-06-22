
#include "dns_poison.h"
#include "esp_err.h"
#include "esp_event.h"
#include "web_server.h"
#include "wifi_ctl.h"

void app_main(void) {
  printf("{ESP_STARTED,}\n");
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  wifi_ctl_init();
  wifi_ctl_create_ap();
  wifi_ctl_ap_start();
  web_server_start();
  dns_poison_create();
}