set(srcs "main.c"
	"qrcodegen.c"
	"wifi_station.c"
    "http_client.c"
)

idf_component_register(SRCS ${srcs} INCLUDE_DIRS ".")
# idf_component_register(SRCS ${srcs} INCLUDE_DIRS "." EMBED_TXTFILES luan_eng_br.pem)