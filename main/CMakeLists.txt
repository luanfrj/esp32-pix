idf_component_register(SRCS "qrcodegen.c" "ili9488.c" "main.c" "wifi_station.c" "http_client.c"
                    INCLUDE_DIRS "."
                    EMBED_TXTFILES luan_eng_br.pem)