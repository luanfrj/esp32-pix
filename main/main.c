/* WiFi station Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/


#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "wifi_station.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/timer.h"
#include "sdkconfig.h"
#include "ili9488.h"
#include "qrcodegen.h"
#include "wifi_station.h"
#include "http_client.h"
#include "esp_task_wdt.h"

#define STATUS_WAIT_USER_INPUT  0
#define STATUS_REQUEST_QRCODE   1
#define STATUS_CHECK_PAYMENT    2
#define STATUS_ACTUATE_ON_GPIO  3

#define BUTTON_INPUT            0

uint8_t current_status;
uint32_t tickNumber = 0;
uint8_t segundos = 0;
uint8_t minutos = 0;
uint8_t horas = 0;

uint8_t order_status;

bool generate_qrcode;
bool get_order_status;

char info[20];

static const char *TAG = "Main";

void IRAM_ATTR timer_tick_func(void *para);

static void print_qrcode(const uint8_t qrcode[]);

static void timer_configure(void);

void app_main()
{
  // Define o botão de entrada
  gpio_set_direction(BUTTON_INPUT, GPIO_MODE_INPUT);

  setup_lcd_pins();
  delay_ms(100);

  ESP_LOGI(TAG,"Iniciando LCD");
  
  init_lcd();

  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
  wifi_init_sta();

  write_string("WiFi Conectado", 10, 10, 255, 255, 255);

  char *buffer;
  uint8_t qrcode[qrcodegen_BUFFER_LEN_MAX];
  uint8_t tempBuffer[qrcodegen_BUFFER_LEN_MAX];

  current_status = STATUS_WAIT_USER_INPUT;

  timer_configure();

  generate_qrcode = false;

  tickNumber = 0;

  while(1) 
  {
    //vTaskDelay(100 / portTICK_PERIOD_MS);
    vTaskDelay(1);
    switch (current_status)
    {
      case STATUS_WAIT_USER_INPUT:
        if (gpio_get_level(BUTTON_INPUT) == 0) 
        {
          if (tickNumber == 50) {
            current_status = STATUS_REQUEST_QRCODE;
            generate_qrcode = true;
          }
        } else {
          tickNumber = 0;
        }
        break;
      case STATUS_REQUEST_QRCODE:
        buffer = malloc(300);
        http_get_qrcode(buffer);
        ESP_LOGI(TAG, "QR CODE: %s", buffer);
        ESP_LOGI(TAG, "Gerando QR Code...");
        bool ok = qrcodegen_encodeText(buffer, tempBuffer, qrcode,
          qrcodegen_Ecc_HIGH, qrcodegen_VERSION_MIN, qrcodegen_VERSION_MAX, qrcodegen_Mask_AUTO, true);
        free(buffer);
        if (ok) {
          print_qrcode(qrcode);
        }
        ESP_LOGI(TAG, "QR Code gerado e Impresso");
        current_status = STATUS_CHECK_PAYMENT;
        tickNumber = 0;
        break;
      case STATUS_CHECK_PAYMENT:
        if (tickNumber > 1000) {
          ESP_LOGI(TAG, "STATUS_CHECK_PAYMENT");
          tickNumber = 0;
          segundos++;
          if (segundos == 60) {
            segundos = 0;
            minutos++;
            if (minutos == 60) {
              minutos = 0;
              horas++;
            }
          }
          
          sprintf(info, "%02d:%02d:%02d", horas, minutos, segundos);
          write_string(info, 30, 400, 0, 255, 0);
          if (segundos % 10 == 0) {
            get_order_status = false;
            ESP_LOGI(TAG, "Verficando Pagamento");
            order_status = http_get_order_status(1);
            if (order_status == 1) {
              ESP_LOGI(TAG, "Pagamento efetuado");
            } else {
              ESP_LOGI(TAG, "Pagamento não efetuado");
            }
          }
        }
        if (minutos == 2) {
          current_status = STATUS_WAIT_USER_INPUT;
        }
        break;
      default:
        break;
    }
    // if (current_status == STATUS_REQUEST_QRCODE && generate_qrcode == true) 
    // {
    //   generate_qrcode = false;
    //   http_get_qrcode(buffer);
    //   ESP_LOGI(TAG, "QR CODE: %s", buffer);
    //   ESP_LOGI(TAG, "Gerando QR Code...");
    //   bool ok = qrcodegen_encodeText(buffer, tempBuffer, qrcode,
    //     qrcodegen_Ecc_HIGH, qrcodegen_VERSION_MIN, qrcodegen_VERSION_MAX, qrcodegen_Mask_AUTO, true);
    //   if (ok) {
    //     print_qrcode(qrcode);
    //   }
    //   ESP_LOGI(TAG, "QR Code gerado e Impresso");
    //   current_status = STATUS_CHECK_PAYMENT;
    // }

    // if (current_status == STATUS_CHECK_PAYMENT && get_order_status == true)
    // {
    //   get_order_status = false;
    //   ESP_LOGI(TAG, "Verficando Pagamento");
    //   order_status = http_get_order_status(1);
    //   if (order_status == 1) {
    //     ESP_LOGI(TAG, "Pagamento efetuado");
    //   } else {
    //     ESP_LOGI(TAG, "Pagamento não efetuado");
    //   }
    // }
  }

}

static void timer_configure(void)
{
  //int timer_group = TIMER_GROUP_0;
  int timer_idx = TIMER_0;

  timer_config_t config;
  config.alarm_en = 1;
  config.auto_reload = 1;
  config.counter_dir = TIMER_COUNT_UP;
  config.divider = 80;
  config.intr_type = TIMER_INTR_LEVEL;
  config.counter_en = TIMER_PAUSE;

  /*Configure timer*/
  timer_init(TIMER_GROUP_0, TIMER_0, &config);
  /*Stop timer counter*/
  timer_pause(TIMER_GROUP_0, TIMER_0);
  /*Load counter value */
  timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0x00000000ULL);
  /*Set alarm value*/
  timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 1000);
  /*Enable timer interrupt*/
  timer_enable_intr(TIMER_GROUP_0, TIMER_0);
  /*Set ISR handler*/
  timer_isr_register(TIMER_GROUP_0, TIMER_0, timer_tick_func, (void*) timer_idx, ESP_INTR_FLAG_IRAM, NULL);
  /*Start timer counter*/
  timer_start(TIMER_GROUP_0, TIMER_0);
}

void IRAM_ATTR timer_tick_func(void *para)
{
  int timer_idx = (int) para;
  uint32_t intr_status = TIMERG0.int_st_timers.val;

  if((intr_status & BIT(timer_idx)) && timer_idx == TIMER_0) 
  {
    TIMERG0.hw_timer[timer_idx].update = 1;
    TIMERG0.int_clr_timers.t0 = 1;

    tickNumber++;

    // esp_task_wdt_reset();

    // char info[20];

    // switch (current_status)
    // {
    //   case STATUS_WAIT_USER_INPUT:

    //     if (gpio_get_level(BUTTON_INPUT) == 0) 
    //     {
    //       tickNumber++;
    //       if (tickNumber == 50) {
    //         current_status = STATUS_REQUEST_QRCODE;
    //         generate_qrcode = true;
    //       }
    //     } else {
    //       tickNumber = 0;
    //     }
    //     break;
    //   case STATUS_REQUEST_QRCODE:
    //     break;
    //   case STATUS_CHECK_PAYMENT:
    //     tickNumber++;
    //     if (tickNumber == 1000) {
    //       tickNumber = 0;
    //       segundos++;
    //       if (segundos == 60) {
    //         segundos = 0;
    //         minutos++;
    //         if (minutos == 60) {
    //           minutos = 0;
    //           horas++;
    //         }
    //       }
          
    //       // sprintf(info, "%02d:%02d:%02d", horas, minutos, segundos);
    //       // write_string(info, 30, 400, 0, 255, 0);
    //     }
    //     if (segundos % 10 == 0 && tickNumber == 0) {
    //       get_order_status = true;
    //     }
    //     if (minutos == 2) {
    //       current_status = STATUS_WAIT_USER_INPUT;
    //     }
    //   default:
    //     break;
    // }
    
    TIMERG0.hw_timer[timer_idx].config.alarm_en = 1;
  }

}

static void print_qrcode(const uint8_t qrcode[]) 
{
	int size = qrcodegen_getSize(qrcode);
	char border = 4;
  char module_size = 4;
  unsigned char r, g, b;
	for (int y = -border; y < size + border; y++) 
  {
		for (int x = -border; x < size + border; x++) 
    {
      if (qrcodegen_getModule(qrcode, x, y)) {
        r = g = b = 0;
      } else {
        r = g = b = 255;
      }
      for (int i = 0; i < module_size; i++) {
        for (int j = 0; j < module_size; j++) {
          write_pixel((x+4)*module_size + j + 6, (y+4)*module_size + i + 86, r, g, b);
        }
      }
		}
	}
}
