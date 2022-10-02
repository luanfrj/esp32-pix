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

## Arquivos principais

1. main.c: código principal do _firmware_;
2. ili9488.c: bibliteca para comunicação com o LCD;
3. qrcodegen.c: biblioteca usada para gerar o QR Code;
http_client.c: biblioteca para encapsular as chamadas HTTP ao _gateway_;
4. wifi_station.c: biblioteca que encapsula as configurações do adaptador wifi;

## Funcionamento

Um vídeo do sistema em funcionamento pode ser visto em [https://youtu.be/LkkqwxMjYC8](https://youtu.be/LkkqwxMjYC8)