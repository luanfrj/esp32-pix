set(component_srcs "http_client.c")

idf_component_register(SRCS "${component_srcs}" 
	INCLUDE_DIRS "." 
	PRIV_REQUIRES esp-tls
	PRIV_REQUIRES esp_http_client
)
