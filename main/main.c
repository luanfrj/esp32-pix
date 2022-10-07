#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/timer.h"
#include "sdkconfig.h"
//#include "ili9488.h"
#include "qrcodegen.h"
#include "wifi_station.h"
#include "http_client.h"
#include "esp_task_wdt.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_vfs.h"
#include "esp_spiffs.h"

#include "lcd_com.h"
#include "lcd_lib.h"
#include "fontx.h"

#include "sdcard.h"

#if CONFIG_INTERFACE_I2S
#define INTERFACE INTERFACE_I2S
#elif CONFIG_INTERFACE_GPIO
#define INTERFACE INTERFACE_GPIO
#elif CONFIG_INTERFACE_REG
#define INTERFACE INTERFACE_REG
#endif

#if CONFIG_ILI9225
#include "ili9225.h"
#define DRIVER "ILI9225"
#define INIT_FUNCTION(a, b, c, d, e) ili9225_lcdInit(a, b, c, d, e)

#elif CONFIG_ILI9226
#include "ili9225.h"
#define DRIVER "ILI9226"
#define INIT_FUNCTION(a, b, c, d, e) ili9225_lcdInit(a, b, c, d, e)

#elif CONFIG_ILI9320
#include "ili9320.h"
#define DRIVER "ILI9320"
#define INIT_FUNCTION(a, b, c, d, e) ili9320_lcdInit(a, b, c, d, e)

#elif CONFIG_ILI9325
#include "ili9325.h"
#define DRIVER "ILI9325"
#define INIT_FUNCTION(a, b, c, d, e) ili9325_lcdInit(a, b, c, d, e)

#elif CONFIG_ILI9327
#include "ili9327.h"
#define DRIVER "ILI9327"
#define INIT_FUNCTION(a, b, c, d, e) ili9327_lcdInit(a, b, c, d, e)

#elif CONFIG_ILI9340
#include "ili9341.h"
#define DRIVER "ILI9340"
#define INIT_FUNCTION(a, b, c, d, e) ili9341_lcdInit(a, b, c, d, e)

#elif CONFIG_ILI9341
#include "ili9341.h"
#define DRIVER "ILI9341"
#define INIT_FUNCTION(a, b, c, d, e) ili9341_lcdInit(a, b, c, d, e)

#elif CONFIG_ILI9342
#include "ili9342.h"
#define DRIVER "ILI9342"
#define INIT_FUNCTION(a, b, c, d, e) ili9342_lcdInit(a, b, c, d, e)

#elif CONFIG_ILI9481
#include "ili9481.h"
#define DRIVER "ILI9481"
#define INIT_FUNCTION(a, b, c, d, e) ili9481_lcdInit(a, b, c, d, e)

#elif CONFIG_ILI9486
#include "ili9486.h"
#define DRIVER "ILI9486"
#define INIT_FUNCTION(a, b, c, d, e) ili9486_lcdInit(a, b, c, d, e)

#elif CONFIG_ILI9488
#include "ili9488.h"
#define DRIVER "ILI9488"
#define INIT_FUNCTION(a, b, c, d, e) ili9488_lcdInit(a, b, c, d, e)

#elif CONFIG_SPFD5408
#include "ili9320.h"
#define DRIVER "SPFD5408"
#define INIT_FUNCTION(a, b, c, d, e) ili9320_lcdInit(a, b, c, d, e)

#elif CONFIG_R61505
#include "ili9320.h"
#define DRIVER "R61505"
#define INIT_FUNCTION(a, b, c, d, e) ili9320_lcdInit(a, b, c, d, e)

#elif CONFIG_R61509
#include "r61509.h"
#define DRIVER "R61509"
#define INIT_FUNCTION(a, b, c, d, e) r61509_lcdInit(a, b, c, d, e)

#elif CONFIG_LGDP4532
#include "lgdp4532.h"
#define DRIVER "LGDP4532"
#define INIT_FUNCTION(a, b, c, d, e) lgdp4532_lcdInit(a, b, c, d, e)

#elif CONFIG_ST7775
#include "ili9225.h"
#define DRIVER "ST7775"
#define INIT_FUNCTION(a, b, c, d, e) ili9225_lcdInit(a, b, c, d, e)

#elif CONFIG_ST7781
#include "st7781.h"
#define DRIVER "ST7781"
#define INIT_FUNCTION(a, b, c, d, e) st7781_lcdInit(a, b, c, d, e)

#elif CONFIG_ST7783
#include "st7781.h"
#define DRIVER "ST7783"
#define INIT_FUNCTION(a, b, c, d, e) st7781_lcdInit(a, b, c, d, e)

#elif CONFIG_ST7793
#include "r61509.h"
#define DRIVER "ST7793"
#define INIT_FUNCTION(a, b, c, d, e) r61509_lcdInit(a, b, c, d, e)

#elif CONFIG_ST7796
#include "ili9486.h"
#define DRIVER "ST7796"
#define INIT_FUNCTION(a, b, c, d, e) ili9486_lcdInit(a, b, c, d, e)

#elif CONFIG_S6D1121
#include "s6d1121.h"
#define DRIVER "S6D1121"
#define INIT_FUNCTION(a, b, c, d, e) s6d1121_lcdInit(a, b, c, d, e)

#elif CONFIG_HX8347A
#include "hx8347.h"
#define DRIVER "HX8347A"
#define INIT_FUNCTION(a, b, c, d, e) hx8347_lcdInit(a, b, c, d, e)

#elif CONFIG_HX8347D
#include "hx8347.h"
#define DRIVER "HX8347D"
#define INIT_FUNCTION(a, b, c, d, e) hx8347_lcdInit(a, b, c, d, e)

#elif CONFIG_HX8347G
#include "hx8347.h"
#define DRIVER "HX8347G"
#define INIT_FUNCTION(a, b, c, d, e) hx8347_lcdInit(a, b, c, d, e)

#elif CONFIG_HX8347I
#include "hx8347.h"
#define DRIVER "HX8347I"
#define INIT_FUNCTION(a, b, c, d, e) hx8347_lcdInit(a, b, c, d, e)

#endif

#define INTERVAL		400
#define WAIT	vTaskDelay(INTERVAL)

#define STATUS_WAIT_USER_INPUT  0
#define STATUS_REQUEST_QRCODE   1
#define STATUS_CHECK_PAYMENT    2
#define STATUS_ACTUATE_ON_GPIO  3

#define BUTTON_INPUT            35
#define ATUADOR                 15

#define LAST_ID_KEY             "last_id"

uint8_t current_status;
uint32_t tickNumber = 0;
uint8_t segundos = 0;
uint8_t minutos = 0;
uint8_t horas = 0;

uint8_t order_status;

bool generate_qrcode;
bool get_order_status;
bool primeira_vez = true;

char info[30];

uint32_t last_id;

static const char *TAG = "Main";

FontxFile fx16G[2];
FontxFile fx24G[2];
FontxFile fx32G[2];

FontxFile fx16M[2];
FontxFile fx24M[2];
FontxFile fx32M[2];

void IRAM_ATTR timer_tick_func(void *para);

static void print_qrcode(TFT_t* dev, const uint8_t qrcode[]);

static void timer_configure(void);

uint32_t read_nvs_data(nvs_handle_t *nvs_handle);

void save_nvs_data(nvs_handle_t *nvs_handle, uint32_t data);

void print_last_order(TFT_t* dev, char * info, uint32_t last_order)
{
  sprintf(info, "Ultima Compra: %d", last_order);
  lcdDrawString(dev, fx24G, 10, 460, (uint8_t *) info, BLUE);
}

static void SPIFFS_Directory(char * path) {
	DIR* dir = opendir(path);
	assert(dir != NULL);
	while (true) {
		struct dirent*pe = readdir(dir);
		if (!pe) break;
		ESP_LOGI(__FUNCTION__,"d_name=%s d_ino=%d d_type=%x", pe->d_name,pe->d_ino, pe->d_type);
	}
	closedir(dir);
}

void app_main()
{
  ESP_LOGI(TAG, "Initializing SPIFFS");
	esp_vfs_spiffs_conf_t conf = {
		.base_path = "/spiffs",
		.partition_label = NULL,
		.max_files = 10,
		.format_if_mount_failed =true
	};

	// Use settings defined above toinitialize and mount SPIFFS filesystem.
	// Note: esp_vfs_spiffs_register is anall-in-one convenience function.
	esp_err_t ret = esp_vfs_spiffs_register(&conf);

	if (ret != ESP_OK) {
		if (ret == ESP_FAIL) {
			ESP_LOGE(TAG, "Failed to mount or format filesystem");
		} else if (ret == ESP_ERR_NOT_FOUND) {
			ESP_LOGE(TAG, "Failed to find SPIFFS partition");
		} else {
			ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)",esp_err_to_name(ret));
		}
		return;
	}

	size_t total = 0, used = 0;
	ret = esp_spiffs_info(NULL, &total,&used);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG,"Failed to get SPIFFS partition information (%s)",esp_err_to_name(ret));
	} else {
		ESP_LOGI(TAG,"Partition size: total: %d, used: %d", total, used);
	}

	SPIFFS_Directory("/spiffs/");

#ifdef CONFIG_LOAD_FROM_SD_CARD
  ESP_LOGI(TAG, "Carregando configurações do Cartão Micro SD");
  
  sdmmc_card_t card;
  sdcard_init(&card);

  char wifi_ssid[32];
  char wifi_password[64];

  sdcard_load_config(wifi_ssid, wifi_password);

#else
  char wifi_ssid[] = CONFIG_ESP_WIFI_SSID;
  char wifi_password[] = CONFIG_ESP_WIFI_PASSWORD;
#endif

  // Define o botão de entrada
  gpio_set_direction(BUTTON_INPUT, GPIO_MODE_INPUT);

  // Define o pino do atuador como saída
  gpio_set_direction(ATUADOR, GPIO_MODE_OUTPUT);

  // Define a saída do atuador para 0;
  gpio_set_level(ATUADOR, 0);

  // setup_lcd_pins();
  // delay_ms(100);

  ESP_LOGI(TAG,"Iniciando LCD");
  
  // init_lcd();

  	// set font file

	InitFontx(fx16G,"/spiffs/ILGH16XB.FNT",""); // 8x16Dot Gothic
	InitFontx(fx24G,"/spiffs/ILGH24XB.FNT",""); // 12x24Dot Gothic
	InitFontx(fx32G,"/spiffs/ILGH32XB.FNT",""); // 16x32Dot Gothic

	InitFontx(fx16M,"/spiffs/ILMH16XB.FNT",""); // 8x16Dot Mincyo
	InitFontx(fx24M,"/spiffs/ILMH24XB.FNT",""); // 12x24Dot Mincyo
	InitFontx(fx32M,"/spiffs/ILMH32XB.FNT",""); // 16x32Dot Mincyo
	
	TFT_t dev;
	lcd_interface_cfg(&dev, INTERFACE);

	INIT_FUNCTION(&dev, CONFIG_WIDTH, CONFIG_HEIGHT, CONFIG_OFFSETX, CONFIG_OFFSETY);

  // Preenche o LCD com o Fundo Preto
  lcdFillScreen(&dev, BLACK);

  // Define a direção da dos textos
  lcdSetFontDirection(&dev, 0);

  lcdSetFontFill(&dev, BLACK);

  ret = nvs_flash_init();
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

  print_last_order(&dev, info, last_id);

  s_wifi_event_group = xEventGroupCreate();

  ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
  wifi_init_sta(wifi_ssid, wifi_password);

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
          if (primeira_vez == true) {
            sprintf(info, "PRESSIONE O BOTAO");
            lcdDrawString(&dev, fx24G, 6, 46, (uint8_t *) info, YELLOW);
            sprintf(info, "PARA PAGAR");
            lcdDrawString(&dev, fx24G, 6, 76, (uint8_t *) info, YELLOW);
            primeira_vez = false;
          }
          if (gpio_get_level(BUTTON_INPUT) == 0) 
          {
            if (tickNumber == 50) {
              current_status = STATUS_REQUEST_QRCODE;
              last_id++;
              save_nvs_data(&nvs_handle, last_id);
              print_last_order(&dev, info, last_id);
            }
          } else {
            tickNumber = 0;
          }
          break;
        case STATUS_REQUEST_QRCODE:
          http_get_qrcode(buffer, last_id);
          // http_get_qrcode_test(buffer);
          ESP_LOGI(TAG, "QR CODE: %s", buffer);
          ESP_LOGI(TAG, "Gerando QR Code...");
          bool ok = qrcodegen_encodeText(buffer, tempBuffer, qrcode,
            qrcodegen_Ecc_HIGH, qrcodegen_VERSION_MIN, qrcodegen_VERSION_MAX, qrcodegen_Mask_AUTO, true);
          if (ok) {
            print_qrcode(&dev, qrcode);
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
            }
            
            sprintf(info, "%02d:%02d:%02d", horas, minutos, segundos);
            lcdDrawString(&dev, fx16G, 30, 420, (uint8_t *) info, GREEN);

            if (minutos == 2) {
              current_status = STATUS_WAIT_USER_INPUT;
              primeira_vez = true;
              segundos = minutos = horas = 0;
              tickNumber = 0;
              // Preenche o LCD com o Fundo Preto
              lcdFillScreen(&dev, BLACK);
              print_last_order(&dev, info, last_id);
              sprintf(info, "TEMPO ESGOTADO!!!");
              lcdDrawString(&dev, fx32G, 30, 136, (uint8_t *) info, RED);
            } else {
              if (segundos % 10 == 0) {
                ESP_LOGI(TAG, "Verficando Pagamento");
                order_status = http_get_order_status(last_id);
                if (order_status == 1) {
                  ESP_LOGI(TAG, "Pagamento efetuado");
                  // Preenche o LCD com o Fundo Preto
                  lcdFillScreen(&dev, BLACK);
                  print_last_order(&dev, info, last_id);
                  sprintf(info, "PAGAMENTO EFETUADO!!!");
                  lcdDrawString(&dev, fx32G, 30, 136, (uint8_t *) info, GREEN);
                  gpio_set_level(ATUADOR, 1);
                  current_status = STATUS_ACTUATE_ON_GPIO;
                  segundos = minutos = horas = 0;
                  tickNumber = 0;
                } else {
                  ESP_LOGI(TAG, "Pagamento não efetuado");
                }
              }
            }
          }
          break;
        case STATUS_ACTUATE_ON_GPIO:
          if (tickNumber > 1000) {
            tickNumber = 0;
            segundos++;
            if (segundos == 60) {
              segundos = 0;
              minutos++;
            }
            sprintf(info, "%02d:%02d:%02d", horas, minutos, segundos);
            lcdDrawString(&dev, fx16G, 30, 420, (uint8_t *) info, GREEN);
            if (segundos == 10) {
              current_status = STATUS_WAIT_USER_INPUT;
              primeira_vez = true;
              segundos = minutos = horas = 0;
              tickNumber = 0;
              // Preenche o LCD com o Fundo Preto
              lcdFillScreen(&dev, BLACK);
              print_last_order(&dev, info, last_id);
              gpio_set_level(ATUADOR, 0);
            }
          }
          break;
        default:
          break;
      }
    } else if (bits & WIFI_FAIL_BIT) {
      if (primeira_vez == true) {
        sprintf(info, "Falha ao conectar ao WiFi!!!");
        lcdDrawString(&dev, fx32G, 30, 136, (uint8_t *) info, RED);
        ESP_LOGI(TAG, "Failed to connect to SSID:%s", EXAMPLE_ESP_WIFI_SSID);
        primeira_vez = false;
      }
        // ESP_LOGI(TAG, "Failed to connect to SSID:%s",
                //  EXAMPLE_ESP_WIFI_SSID);
    } else {
        // ESP_LOGE(TAG, "UNEXPECTED EVENT");
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
  TIMERG0.hw_timer[TIMER_0].update = 1;
  TIMERG0.int_clr_timers.t0 = 1;

  tickNumber++;
  TIMERG0.hw_timer[TIMER_0].config.alarm_en = 1;
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

static void print_qrcode(TFT_t* dev, const uint8_t qrcode[]) 
{
	int size = qrcodegen_getSize(qrcode);
	char border = 4;
  char module_size = 4;
  uint16_t color;
  // unsigned char r, g, b;
	for (int y = -border; y < size + border; y++) 
  {
		for (int x = -border; x < size + border; x++) 
    {
      if (qrcodegen_getModule(qrcode, x, y)) {
        color = 0x0000;
      } else {
        color = 0xFFFF;
      }
      for (int i = 0; i < module_size; i++) {
        for (int j = 0; j < module_size; j++) {
          lcdDrawPixel(dev, (x+4)*module_size + j + 6, (y+4)*module_size + i + 86, color);
        }
      }
		}
	}
}
