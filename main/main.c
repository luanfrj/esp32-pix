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
#include "sdkconfig.h"
#include "ili9488.h"
#include "qrcodegen.h"
#include "wifi_station.h"
#include "http_client.h"

#define STATUS_WAIT_USER_INPUT  0
#define STATUS_REQUEST_QRCODE   1
#define STATUS_CHECK_PAYMENT    2
#define STATUS_ACTUATE_ON_GPIO  3

uint8_t current_status;
uint32_t tickNumber = 0;
uint8_t segundos = 0;
uint8_t minutos = 0;
uint8_t horas = 0;

bool  inTick = false;

void tickFunc(void);

static void printQr(const uint8_t qrcode[]);

void app_main()
{
  //Initialize NVS
  // esp_err_t ret = nvs_flash_init();
  // if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
  //   ESP_ERROR_CHECK(nvs_flash_erase());
  //   ret = nvs_flash_init();
  // }
  // ESP_ERROR_CHECK(ret);

  // ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
  // wifi_init_sta();

  setup_lcd_pins();
  delay_ms(100);

  printf("Iniciando LCD\n");
  
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

  ESP_LOGI(TAG, "Connected to AP, begin http example");

  write_string("Enviando request", 10, 30, 0, 0, 255);

  char buffer[300];
  https_with_hostname_path(buffer);

  //printf("Resposta %s", buffer);
  //write_string(buffer, 10, 42, 255, 0, 0);
  
  const char *text = "00020101021226940014BR.GOV.BCB.PIX2572pix-qr.mercadopago.com/instore/o/v2/c5231ccc-9dbd-4cbf-bc47-1dc2530bfd8d5204000053039865802BR5904Luan6009SAO PAULO62070503***6304E64F";
  uint8_t qrcode[qrcodegen_BUFFER_LEN_MAX];
  uint8_t tempBuffer[qrcodegen_BUFFER_LEN_MAX];
  printf("Gerando QR Code\n");
  bool ok = qrcodegen_encodeText(text, tempBuffer, qrcode,
    qrcodegen_Ecc_HIGH, qrcodegen_VERSION_MIN, qrcodegen_VERSION_MAX, qrcodegen_Mask_AUTO, true);
  if (ok)
    printQr(qrcode);
  printf("QR Code gerado e Impresso\n");

  write_string("HELLO WORLD! BRASIL", 10, 450, 255, 0, 0);

  current_status = STATUS_WAIT_USER_INPUT;

  while(1) 
  {
    if (!inTick) {
      tickFunc();
    }
  }

}

void tickFunc(void)
{
  inTick = true;
  tickNumber++;

  char info[20];

  if (tickNumber == 1000) {
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
  }

  // if ()
  // switch (current_status)
  // {
  //   case STATUS_WAIT_USER_INPUT:
  //     /* code */
  //     break;
    
  //   default:
  //     break;
  // }
  inTick = false;
}

static void printQr(const uint8_t qrcode[]) 
{
	int size = qrcodegen_getSize(qrcode);
  printf("Size: %d\n", size);
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
