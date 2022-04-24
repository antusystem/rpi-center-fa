/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "AM2301.h"
#include "sdkconfig.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "freertos/task.h"

// Queue Handler
QueueHandle_t xQueue_temp;

EventGroupHandle_t event_group = NULL;

// Bit for tasks
const int TEMP_BIT = BIT1;
const int HTTP_BIT = BIT2;

void app_main(void)
{
    printf("Hello world!\n");

    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU core(s), WiFi%s%s, ",
            CONFIG_IDF_TARGET,
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());
    ESP_LOGE("app_main", "Error");
    ESP_LOGW("app_main", "Warning");
    ESP_LOGI("app_main", "Info");
    ESP_LOGD("app_main", "Debug");
    ESP_LOGV("app_main", "Verbose");    

    event_group = xEventGroupCreate();
    // Creting Queue for Temperature Data
    xQueue_temp = xQueueCreate(1, sizeof(AM2301_data_t));
    xTaskCreatePinnedToCore(&Temp_Task, "Temp_Task", 1024*4, NULL, 5, NULL, 1);

    // Initiating Temp_Task
    xEventGroupSetBits(event_group, TEMP_BIT);
}
