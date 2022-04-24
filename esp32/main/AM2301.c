/* AM2301
 * Se encarga de comunicarse con el sensor AM2301, organizar los datos de temperatura y humedad
 * para luego enviarlos por cola
*/
#include <string.h>
#include "AM2301.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "nvs_flash.h"


/*Handlers externos*/
extern EventGroupHandle_t event_group;
extern QueueHandle_t xQueue_temp;

/*Bits externos para los eventos*/
extern const int TEMP_BIT;
extern const int HTTP_BIT;

static portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

static char T_Tag[13] = "Sensor_AM2301";

esp_err_t wait_Time(gpio_num_t pin, uint32_t timeout,
					int valor_esperado, uint32_t *contador_us){
	// Funcion para medir tiempo en us
    gpio_set_direction(pin, GPIO_MODE_INPUT);
    for (uint32_t i = 0; i < timeout; i += us_retardo){
        ets_delay_us(us_retardo);
        if(gpio_get_level(pin) == valor_esperado){
            if(contador_us) *contador_us = i;
            return ESP_OK;
        }
    }
    return ESP_ERR_TIMEOUT;
}

esp_err_t catch_Data(gpio_num_t pin, uint8_t datos[numDHT_bytes]){
	// Funcion para capturar bits del DHT11
    uint32_t tiempo_low;
    uint32_t tiempo_high;
    // Inicio de la secuencia de lectura
    gpio_set_direction(pin, GPIO_MODE_OUTPUT_OD);
    gpio_set_level(pin, 0);
    ets_delay_us(1000);
    gpio_set_level(pin, 1);
    // Se espera hasta 220us por un flanco de bajada desde el AM2301
    if(wait_Time(pin, 220, 0, NULL)!=ESP_OK)return ESP_ERR_TIMEOUT;
    // Se espera hasta 90us por un flanco de subida
    if(wait_Time(pin, 90, 1, NULL)!=ESP_OK)return ESP_ERR_TIMEOUT;
    // Se espera hasta 90us por un flanco de bajada
    if(wait_Time(pin, 90, 0, NULL)!=ESP_OK)return ESP_ERR_TIMEOUT;
    // Si la respuesta fue satisfactoria, se comienzan a recibir los datos
    for (int i = 0; i < numDHT_bits; i++){
    	if(wait_Time(pin, 60, 1, &tiempo_low)!=ESP_OK)return ESP_ERR_TIMEOUT;
    	if(wait_Time(pin, 80, 0, &tiempo_high)!=ESP_OK)return ESP_ERR_TIMEOUT;
        uint8_t b = i / 8, m = i % 8;
        if (!m)datos[b] = 0;
        datos[b] |= (tiempo_high > tiempo_low) << (7 - m);
    }
    return ESP_OK;
}

esp_err_t read_DHT(gpio_num_t pin, uint8_t *humedad, uint8_t *decimal_hum,
				   uint8_t *temperatura, uint8_t *decimal_temp,
				   uint8_t *signo_temp){
    uint8_t datos[numDHT_bytes] = {0,0,0,0,0};
    gpio_set_direction(pin, GPIO_MODE_OUTPUT_OD);
    gpio_set_level(pin, 1);

    portENTER_CRITICAL(&mux);
    esp_err_t resultado = catch_Data(pin, datos);
    portEXIT_CRITICAL(&mux);

    gpio_set_direction(pin, GPIO_MODE_OUTPUT_OD);
    gpio_set_level(pin, 1);
    if(resultado != ESP_OK)return resultado;
    if(datos[4] != ((datos[0] + datos[1] + datos[2] + datos[3]) & 0xFF)){
        ESP_LOGE("Sensor_AM2301","Checksum Error");
        return ESP_ERR_INVALID_CRC;
    }
    *humedad = datos[0];
    *decimal_hum = datos[1];
    *signo_temp  = (datos[2]&0x80) >> 7;
    *temperatura  = datos[2]&0x7F;
    *decimal_temp = datos[3];
    return ESP_OK;
}

void Temp_Task(void *P){
	ESP_LOGI(T_Tag, "****** Starting  Temp_Task ****** \r\n");
	uint8_t temperature = 0, decimal_temp = 0, sign_temp = 0;
    uint8_t humidity = 0, decimal_hum = 0;
	uint8_t work_status, cycle_id = 0;
    uint16_t hum_aux1 = 0, temp_aux1 = 0;
    int hum_aux2 = 0, temp_aux2 = 0;
    float prom_temp = 0, prom_hum = 0;
    Thum.first_cycle = 1;
    // Position in the averages array of temperature and humidity
    Thum.pos_temp = 0;
    for(;;){
		// Waiting for signal to begin
    	xEventGroupWaitBits(event_group, TEMP_BIT, pdFALSE, true, portMAX_DELAY);
    	work_status = 0;
		// 16 cycles to read the temperature
    	for (int j1 = 0; j1 < 16; j1++ ){
    		ESP_LOGD(T_Tag,"Waiting 3s");
    		vTaskDelay(3000 / portTICK_PERIOD_MS);
    		if (read_DHT(DHTpin, &humidity, &decimal_hum, &temperature, &decimal_temp, &sign_temp) == ESP_OK){
    		//  Identifying humidity
    		switch (humidity){
    		case 0:
    			hum_aux1 = 0;
    			break;
    		case 1:
    			hum_aux1 = 256;
    			break;
    		case 2:
    			hum_aux1 = 512;
    			break;
    		}
    		// Identifying temperature
    		switch (temperature){
    		case 0:
    			temp_aux1 = 0;
    			break;
    		case 1:
    			temp_aux1 = 256;
    			break;
    		}
    		// Saving humidity value as int
    		hum_aux2 = (hum_aux1 + decimal_hum);
    		// Saving temperature value as int
    		temp_aux2 = (temp_aux1 + decimal_temp);
    		ESP_LOGI(T_Tag, "Saving data in struct");
    		Thum.Humidity[j1] = (float) hum_aux2/10;
    		Thum.Temperature[j1] = (float) temp_aux2/10;
    		ESP_LOGW(T_Tag,
			"Humidity is: %.1f %% \r\n Temperature is: %.1f C",
			Thum.Humidity[j1], Thum.Temperature[j1]);
    		work_status = 0;
    		// cycle_error indicates how many continous cycles have been and error
    		Thum.cycle_error = 0;
    		cycle_id++;
    		ESP_LOGI(T_Tag, "Cycle id is: %d", cycle_id);
			// Averaging the values obtained
    		for (int i = 0; i < 16; i++ ){
    			prom_hum += Thum.Humidity[i];
    			prom_temp += Thum.Temperature[i];
    		}
    		ESP_LOGI(T_Tag,"Humidity average is: %f", prom_hum);
    		ESP_LOGI(T_Tag,"Temperature average is: %f", prom_temp);
			// Updating average value
    		Thum.Prom_hum[Thum.pos_temp] = prom_hum/16;
    		Thum.Prom_temp[Thum.pos_temp] = prom_temp/16;
			// Reseting local average value
    		prom_hum =  0.0;
    		prom_temp = 0.0;
			// Incresing position
    		Thum.pos_temp++;
			// Updating Queue with new data
    		xQueueOverwrite(xQueue_temp, &Thum);
    	} else{
            ESP_LOGE(T_Tag, "Unable to read data from AM2301");
            work_status = 1;
            Thum.cycle_error++;
            // Verifying for 10 consecutive errors. Error 10 raise a flag
            if (Thum.cycle_error >= 10){
            	Thum.cycle_error = 0;
            	Thum.error_temp = 1;
				// Updating Queue with new data
            	xQueueOverwrite(xQueue_temp, &Thum);
				ESP_LOGW(T_Tag, "Giving signal to HTTP Task");
            	xEventGroupSetBits(event_group, HTTP_BIT);
            	break;
            }
        }
		// Verifying if it was the last cycle (16) and everything is OK
        if (work_status == 0 && cycle_id >=16){
        	Thum.first_cycle = 0;
        	cycle_id = 0;
			// Giving signal to HTTP Task
			ESP_LOGW(T_Tag, "Giving signal to HTTP Task");
        	xEventGroupSetBits(event_group, HTTP_BIT);
    		break;
        }
        }
    }
}