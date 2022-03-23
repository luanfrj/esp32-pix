# Esp32-pix
Integração entre esp32 e api pix.

Este projeto foi desenvolvido usando o framework [ESP-IDF](https://github.com/espressif/esp-idf).

## Configuração

Antes de compilar é preciso ajustar configurações ambientais do sistema. Para isso é preciso executar o seguinte comando:
```
idf.py menuconfig
```
E ajustar os seguintes parâmetros no menu **Project Configuration**

1. PIX_GATEWAY_HOST: _hostname_ do gateway;
2. ESP_WIFI_SSID: nome da conexão wifi;
3. ESP_WIFI_PASSWORD: senha da conexão wifi;

## Compilação

Para compilar o código basta executar o comando a seguir na raiz do projeto:

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