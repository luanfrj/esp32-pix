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

  gpio_set_level(LCD_CS, 0);
  
  const char *text = "00020101021226940014BR.GOV.BCB.PIX2572pix-qr.mercadopago.com/instore/o/v2/73055cb8-ceb7-4c9a-8328-298b0630c6c85204000053039865802BR5904Luan6009SAO PAULO62070503***63042300";
  uint8_t qrcode[qrcodegen_BUFFER_LEN_MAX];
  uint8_t tempBuffer[qrcodegen_BUFFER_LEN_MAX];
  printf("Gerando QR Code\n");
  bool ok = qrcodegen_encodeText(text, tempBuffer, qrcode,
    qrcodegen_Ecc_HIGH, qrcodegen_VERSION_MIN, qrcodegen_VERSION_MAX, qrcodegen_Mask_AUTO, true);
  if (ok)
    printQr(qrcode);
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
          write_pixel((x+4)*module_size + j, (y+4)*module_size + i + 80, r, g, b);
        }
      }
		}
	}
}
