idf_component_register(
    SRCS "dns_poison.c" "wifi_ctl.c" "main.c" "web_server.c"
    INCLUDE_DIRS "include"
    PRIV_REQUIRES esp_event esp_netif esp_http_server esp_wifi
)