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
#include "esp_heap_trace.h"

#define STATUS_WAIT_USER_INPUT  0
#define STATUS_REQUEST_QRCODE   1
#define STATUS_CHECK_PAYMENT    2
#define STATUS_ACTUATE_ON_GPIO  3

#define BUTTON_INPUT            0
#define ATUADOR                 33

#define LAST_ID_KEY             "last_id"

uint8_t current_status;
uint32_t tickNumber = 0;
uint8_t segundos = 0;
uint8_t minutos = 0;
uint8_t horas = 0;

uint8_t order_status;

bool generate_qrcode;
bool get_order_status;

char info[30];

uint32_t last_id;

static const char *TAG = "Main";

void IRAM_ATTR timer_tick_func(void *para);

static void print_qrcode(const uint8_t qrcode[]);

static void timer_configure(void);

uint32_t read_nvs_data(nvs_handle_t *nvs_handle);

void save_nvs_data(nvs_handle_t *nvs_handle, uint32_t data);

void print_last_order(char * info, uint32_t last_order)
{
  sprintf(info, "Ultima Compra: %d", last_order);
  write_string(info, 10, 465, 0, 0, 255);
}

void app_main()
{
  // Define o botão de entrada
  gpio_set_direction(BUTTON_INPUT, GPIO_MODE_INPUT);

  // Define o pino do atuador como saída
  gpio_set_direction(ATUADOR, GPIO_MODE_OUTPUT);

  setup_lcd_pins();
  delay_ms(100);

  ESP_LOGI(TAG,"Iniciando LCD");
  
  init_lcd();

  heap_trace_start(HEAP_TRACE_ALL);

  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);


  ESP_LOGI(TAG, "Opening Non-Volatile Storage (NVS) handle... ");
  nvs_handle_t nvs_handle;
  esp_err_t err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
  if (err == ESP_OK) {
    ESP_LOGI(TAG, "Pronto");
  } else {
    ESP_LOGE(TAG, "Falha ao abrir NVS");
  }

  last_id = read_nvs_data(&nvs_handle);

  print_last_order(info, last_id);

  s_wifi_event_group = xEventGroupCreate();

  ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
  wifi_init_sta();

  write_string("WiFi Conectado", 10, 10, 255, 255, 255);

  char *buffer;
  buffer = (char *) malloc(300);
  uint8_t qrcode[qrcodegen_BUFFER_LEN_MAX];
  uint8_t tempBuffer[qrcodegen_BUFFER_LEN_MAX];

  current_status = STATUS_WAIT_USER_INPUT;

  timer_configure();

  generate_qrcode = false;

  tickNumber = 0;

  while(1) 
  {
    vTaskDelay(10 / portTICK_PERIOD_MS);

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        // ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
        //          EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
      switch (current_status)
      {
        case STATUS_WAIT_USER_INPUT:
          if (gpio_get_level(BUTTON_INPUT) == 0) 
          {
            if (tickNumber == 50) {
              current_status = STATUS_REQUEST_QRCODE;
              last_id++;
              save_nvs_data(&nvs_handle, last_id);
              print_last_order(info, last_id);
            }
          } else {
            tickNumber = 0;
          }
          break;
        case STATUS_REQUEST_QRCODE:
          http_get_qrcode(buffer, last_id);
          ESP_LOGI(TAG, "QR CODE: %s", buffer);
          ESP_LOGI(TAG, "Gerando QR Code...");
          bool ok = qrcodegen_encodeText(buffer, tempBuffer, qrcode,
            qrcodegen_Ecc_HIGH, qrcodegen_VERSION_MIN, qrcodegen_VERSION_MAX, qrcodegen_Mask_AUTO, true);
          if (ok) {
            print_qrcode(qrcode);
          }
          ESP_LOGI(TAG, "QR Code gerado e Impresso");
          current_status = STATUS_CHECK_PAYMENT;
          tickNumber = 0;
          break;
        case STATUS_CHECK_PAYMENT:
          if (tickNumber > 1000) {
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
            if (minutos == 2) {
              current_status = STATUS_WAIT_USER_INPUT;
              segundos = minutos = horas = 0;
              tickNumber = 0;
              set_bgcolor(0, 0, 0);
              print_last_order(info, last_id);
              delay_ms(100);
              sprintf(info, "TEMPO ESGOTADO!!!");
              for (uint8_t j = 0; j < 10; j++) {
                write_string(info, 30, 86 + 17*j, 255, 0, 0);
              }
            } else {
              if (segundos % 10 == 0) {
                ESP_LOGI(TAG, "Verficando Pagamento");
                order_status = http_get_order_status(last_id);
                if (order_status == 1) {
                  ESP_LOGI(TAG, "Pagamento efetuado");
                  set_bgcolor(0, 0, 0);
                  print_last_order(info, last_id);
                  delay_ms(100);
                  sprintf(info, "PAGAMENTO EFETUADO!!!");
                  for (uint8_t j = 0; j < 10; j++) {
                    write_string(info, 30, 86 + 17*j, 0, 255, 0);
                  }
                  current_status = STATUS_WAIT_USER_INPUT;
                  segundos = minutos = horas = 0;
                  tickNumber = 0;
                } else {
                  ESP_LOGI(TAG, "Pagamento não efetuado");
                }
              }
            }
          }
          break;
        default:
          break;
      }
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }


  }

  free(buffer);
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
  timer_isr_register(TIMER_GROUP_0, TIMER_0, &timer_tick_func, (void*) &timer_idx, ESP_INTR_FLAG_IRAM, NULL);
  /*Start timer counter*/
  timer_start(TIMER_GROUP_0, TIMER_0);
}

void IRAM_ATTR timer_tick_func(void *para)
{
  // int timer_idx = * (int *) para;
  // uint32_t intr_status = TIMERG0.int_st_timers.val;

  // if(timer_idx == TIMER_0) 
  // {
  //   ESP_LOGI(TAG, "timer_idx = %d", timer_idx);
    TIMERG0.hw_timer[TIMER_0].update = 1;
    TIMERG0.int_clr_timers.t0 = 1;

    tickNumber++;
    TIMERG0.hw_timer[TIMER_0].config.alarm_en = 1;
  // }

}

uint32_t read_nvs_data(nvs_handle_t *nvs_handle) {
  ESP_LOGI(TAG, "Reading restart counter from NVS ... ");
  uint32_t last_id = 0; 
  esp_err_t err = nvs_get_u32(*nvs_handle, LAST_ID_KEY, &last_id);
  switch (err) {
    case ESP_OK:
      ESP_LOGI(TAG, "Done");
      ESP_LOGI(TAG, "Last Id = %d", last_id);
      break;
    case ESP_ERR_NVS_NOT_FOUND:
      ESP_LOGE(TAG, "The value is not initialized yet!");
      break;
    default :
      ESP_LOGE(TAG, "Error (%s) reading!", esp_err_to_name(err));
  }
  return last_id;
}

void save_nvs_data(nvs_handle_t *nvs_handle, uint32_t data) {
  ESP_LOGI(TAG, "Updating last_id in NVS ... ");
  esp_err_t err = nvs_set_u32(*nvs_handle, LAST_ID_KEY, data);
  if (err == ESP_OK) {
    ESP_LOGI(TAG, "Pronto");
  } else {
    ESP_LOGE(TAG, "Falha ao escrever no NVS");
  }

  ESP_LOGI(TAG, "Committing updates in NVS ... ");
  err = nvs_commit(*nvs_handle);
  if (err == ESP_OK) {
    ESP_LOGI(TAG, "Pronto");
  } else {
    ESP_LOGE(TAG, "Falha ao escrever no NVS");
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
