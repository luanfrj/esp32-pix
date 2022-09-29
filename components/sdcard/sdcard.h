#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"

#define MOUNT_POINT "/sdcard"

#define PIN_NUM_MISO  CONFIG_PIN_MISO
#define PIN_NUM_MOSI  CONFIG_PIN_MOSI
#define PIN_NUM_CLK   CONFIG_PIN_CLK
#define PIN_NUM_CS    CONFIG_PIN_CS

#if CONFIG_IDF_TARGET_ESP32S2
#define SPI_DMA_CHAN    host.slot
#elif CONFIG_IDF_TARGET_ESP32C3
#define SPI_DMA_CHAN    SPI_DMA_CH_AUTO
#else
#define SPI_DMA_CHAN    1
#endif

void sdcard_init(sdmmc_card_t *card);
