# Esp32-pix
Integração entre esp32 e api pix.

Este projeto foi desenvolvido usando o framework [ESP-IDF](https://github.com/espressif/esp-idf).

## Configuração

Antes de compilar é preciso ajustar configurações ambientais do sistema. Para isso é preciso executar os seguintes comandos:
```
idf.py set-target esp32

idf.py menuconfig
```
No menu **PIX Configuration** ajuste a configuração de conexão com o pix:

1. PIX_GATEWAY_HOST: _hostname_ do gateway;

No menu **WIFI Configuration** ajuste a configuração de conexão WiFi:

1. LOAD_FROM_SD_CARD: Define se a configuração do WiFi será feita via arquivo ou via menuconfig;
2. ESP_WIFI_SSID: nome da conexão wifi;
3. ESP_WIFI_PASSWORD: senha da conexão wifi;

Caso a configuração seja feita via arquivo é preciso colocar o arquivo config.txt na raiz do cartão SD contendo duas linhas, a primeira com o SSID e a segunda com a Senha.
```
SSID
senha
```

No menu **SD CARD SPI Configuration** ajuste a pinagem de conexão com o SD Card.

No menu **TFT Configuration** escolha o driver e a pinagem do display LCD.

## Compilação

Para compilar o código basta executar os comandos a seguir na raiz do projeto:

```
idf.py build
```

## Módulos principais

1. main: código principal do _firmware_;
2. tft_library: Biblioteca para comunição o display LCD. Desenvolvida por https://github.com/nopnop2002/esp-idf-parallel-tft;
3. qrcodegen: biblioteca usada para gerar o QR Code;
4. http_client: biblioteca para encapsular as chamadas HTTP ao _gateway_;
5. wifi_station: biblioteca que encapsula as configurações do adaptador wifi;
6. sdcard: biblioteca para comunicar com o cartão sd e ler o arquivo de configuração;

## Funcionamento

Um vídeo do sistema em funcionamento pode ser visto em [https://youtu.be/LkkqwxMjYC8](https://youtu.be/LkkqwxMjYC8)