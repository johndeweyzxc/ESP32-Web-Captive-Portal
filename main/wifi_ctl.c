#include "wifi_ctl.h"

#include "esp_wifi.h"
#include "lwip/ip_addr.h"
#include "web_server.h"

static esp_netif_t* netif;
uint8_t num_connect_clients = 0;

static void wifi_ctl_event_handler(void* arg, esp_event_base_t event_base,
                                   int32_t event_id, void* event_data) {
  if (event_id == WIFI_EVENT_AP_STACONNECTED) {
    wifi_event_ap_staconnected_t* event =
        (wifi_event_ap_staconnected_t*)event_data;
    uint8_t* b = event->mac;

    num_connect_clients++;
    printf("{WIFI,CONNECT,%02X:%02X:%02X:%02X:%02X:%02X,}\n", b[0], b[1], b[2],
           b[3], b[4], b[5]);
  } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
    wifi_event_ap_stadisconnected_t* event =
        (wifi_event_ap_stadisconnected_t*)event_data;
    uint8_t* b = event->mac;

    num_connect_clients--;
    printf("{WIFI,DISCONNECT,%02X:%02X:%02X:%02X:%02X:%02X,}\n", b[0], b[1],
           b[2], b[3], b[4], b[5]);
    if (num_connect_clients == 0) {
      web_server_stop();
      web_server_start();
    }
  }
}

void wifi_ctl_ap_stop() {
  ESP_ERROR_CHECK(esp_wifi_stop());
  printf("{WIFI,STOPPED,}\n");
}

void wifi_ctl_ap_start() {
  ESP_ERROR_CHECK(esp_netif_dhcps_stop(netif));
  esp_netif_ip_info_t ip_info;
  IP4_ADDR(&ip_info.ip, 172, 217, 28, 1);
  IP4_ADDR(&ip_info.gw, 172, 217, 28, 1);
  IP4_ADDR(&ip_info.netmask, 255, 255, 255, 0);
  ESP_ERROR_CHECK(esp_netif_set_ip_info(netif, &ip_info));
  esp_netif_dhcps_start(netif);

  ESP_ERROR_CHECK(esp_wifi_start());
  printf("{WIFI,STARTED,}\n");
}

void wifi_ctl_create_ap() {
  wifi_config_t wifi_config = {
      .ap = {.ssid = "ESP32 Wi-Fi",
             .password = "12345678",
             .ssid_len = strlen("ESP32 Wi-Fi"),
             .channel = 1,
             .authmode = WIFI_AUTH_WPA_WPA2_PSK,
             .max_connection = 4},
  };

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
}

void wifi_ctl_init() {
  ESP_ERROR_CHECK(esp_netif_init());
  netif = esp_netif_create_default_wifi_ap();
  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_ctl_event_handler, NULL, NULL));

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
}