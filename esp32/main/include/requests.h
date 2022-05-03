#include "esp_event.h"

#define ESP_WIFI_SSID      CONFIG_ESP_WIFI_SSID
#define ESP_WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD
#define ESP_MAXIMUM_RETRY  CONFIG_ESP_MAXIMUM_RETRY

/* Tareas Asociadas*/
void http_Task(void *P);

/*Funciones Asociadas*/
static void event_handler(void* arg, esp_event_base_t event_base,
                          int32_t event_id, void* event_data);
void wifi_init_sta(void);
void request_main(void);